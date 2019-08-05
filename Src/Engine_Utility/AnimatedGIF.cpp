/*-----------------------------------------------------------------------------
 * FILENAME:
 *			AnimatedGIF.cpp
 *
 * DESCRIPTION:
 *			cAnimatedGif 클래스
 *
 *			Picture displaying control with support for the following formats:
 *			GIF (including animated GIF87a and GIF89a), JPEG, BMP, WMF, ICO, CUR
 *
 *			Written by Oleg Bykov (oleg_bykoff@rsdn.ru) Copyright (c) 2001
 *
 * HISTORY:
 *			2004.11.04 - 
 *---------------------------------------------------------------------------*/
#include "StdAfx.h"
#include "AnimatedGIF.h"

//---------------------------------------------------------------------------//

inline int TGIFLSDescriptor::GetPackedValue( enum LSDPackedValues Value )
{
	int nRet = (int)m_cPacked;

	switch (Value)
	{
		case LSD_PACKED_GLOBALCT:
			nRet = nRet >> 7;
			break;

		case LSD_PACKED_CRESOLUTION:
			nRet = ((nRet & 0x70) >> 4) + 1;
			break;

		case LSD_PACKED_SORT:
			nRet = (nRet & 0x08) >> 3;
			break;

		case LSD_PACKED_GLOBALCTSIZE:
			nRet &= 0x07;
			break;
	};

	return nRet;
}

inline int TGIFControlExt::GetPackedValue( enum ControlExtValues Value )
{
	int nRet = (int)m_cPacked;

	switch (Value)
	{
		case GCX_PACKED_DISPOSAL:
			nRet = (nRet & 0x1c) >> 2;
			break;

		case GCX_PACKED_USERINPUT:
			nRet = (nRet & 0x02) >> 1;
			break;

		case GCX_PACKED_TRANSPCOLOR:
			nRet &= 0x01;
			break;
	};

	return nRet;
}

inline int TGIFImageDescriptor::GetPackedValue( enum IDPackedValues Value )
{
	int nRet = (int)m_cPacked;

	switch (Value)
	{
		case ID_PACKED_LOCALCT:
			nRet >>= 7;
			break;

		case ID_PACKED_INTERLACE:
			nRet = ((nRet & 0x40) >> 6);
			break;

		case ID_PACKED_SORT:
			nRet = (nRet & 0x20) >> 5;
			break;

		case ID_PACKED_LOCALCTSIZE:
			nRet &= 0x07;
			break;
	};

	return nRet;
}


//---------------------------------------------------------------------------//

cAnimatedGIF::cAnimatedGIF()
{
	// check structure sizes
	ASSERT( sizeof(TGIFImageDescriptor) == 10 );
	ASSERT( sizeof(TGIFAppExtension)    == 14 );
	ASSERT( sizeof(TGIFPlainTextExt)    == 15 );
	ASSERT( sizeof(TGIFLSDescriptor)    ==  7 );
	ASSERT( sizeof(TGIFControlExt)	    ==  8 );
	ASSERT( sizeof(TGIFCommentExt)	    ==  2 );
	ASSERT( sizeof(TGIFHeader)		    ==  6 );

	m_pRawData			= NULL;					// 파일 데이터
	m_nDataSize			= 0;					// 파일 크기
	m_nCurrOffset		= 0;					// 파일에서 현재 그림 데이터 위치

	m_pGIFHeader		= NULL;					// GIF 헤더
	m_pGIFLSDescriptor	= NULL;					// GIF LS 디스크립터

	m_bIsLoaded			= FALSE;				// 파일 읽기 플래그
	m_bIsGIF			= FALSE;				// GIF 파일 플래그
	m_nBackgroundIndex	= (-1);					// 배경색 인덱스
	m_clrBackground		= RGB(255,255,255);		// 배경색(white by default)
	m_PictureSize.cx	= 0;					// 그림 크기(너비)
	m_PictureSize.cy	= 0;					// 그림 크기(높이)

	ZeroMemory( m_GlobalColorTable, sizeof(m_GlobalColorTable) );
}

cAnimatedGIF::~cAnimatedGIF()
{
	Unload();
}


void cAnimatedGIF::ResetDataPointer()
{
	// skip header and logical screen descriptor
	m_nCurrOffset = sizeof(TGIFHeader) + sizeof(TGIFLSDescriptor) + m_nGlobalCTSize;
}

BOOL cAnimatedGIF::SkipNextGraphicBlock()
{
	if ( !m_pRawData ) return FALSE;

	// GIF header + LSDescriptor [+ GCT] [+ Control block] + Data

	enum GIFBlockTypes nBlock;

	nBlock = GetNextBlock();

	while( (nBlock != BLOCK_CONTROLEXT) &&
		   (nBlock != BLOCK_IMAGE) &&
		   (nBlock != BLOCK_PLAINTEXT) &&
		   (nBlock != BLOCK_UNKNOWN) &&
		   (nBlock != BLOCK_TRAILER) )
	{
		if( !SkipNextBlock() ) return NULL;
		nBlock = GetNextBlock();
	};

	if( (nBlock == BLOCK_UNKNOWN) || (nBlock == BLOCK_TRAILER) ) return FALSE;

	// it's either a control ext.block, an image or a plain text

	if( GetNextBlockLen() <= 0 ) return FALSE;

	if( nBlock == BLOCK_CONTROLEXT )
	{
		if( !SkipNextBlock() ) return FALSE;
		nBlock = GetNextBlock();

		// skip everything until we meet an image block or a plain-text block
		while( (nBlock != BLOCK_IMAGE) &&
			   (nBlock != BLOCK_PLAINTEXT) &&
			   (nBlock != BLOCK_UNKNOWN) &&
			   (nBlock != BLOCK_TRAILER) )
		{
			if( !SkipNextBlock() ) return NULL;
			nBlock = GetNextBlock();
		};

		if( (nBlock == BLOCK_UNKNOWN) || (nBlock == BLOCK_TRAILER) ) return FALSE;
	}

	// skip the found data block (image or plain-text)
	if( !SkipNextBlock() ) return FALSE;

	return TRUE;
}

enum GIFBlockTypes cAnimatedGIF::GetNextBlock() const
{
	switch(m_pRawData[m_nCurrOffset])
	{
		case 0x21:				// extension block
			switch(m_pRawData[m_nCurrOffset+1])
			{
				case 0x01:		// plain text extension
					return BLOCK_PLAINTEXT;
					break;

				case 0xF9:		// graphic control extension
					return BLOCK_CONTROLEXT;
					break;

				case 0xFE:		// comment extension
					return BLOCK_COMMEXT;
					break;

				case 0xFF:		// application extension
					return BLOCK_APPEXT;
					break;
			};
			break;
		
		case 0x3B:				// trailer
			return BLOCK_TRAILER;
			break;

		case 0x2C:				// image data
			return BLOCK_IMAGE;
			break;
	};

	return BLOCK_UNKNOWN;
}

BOOL cAnimatedGIF::SkipNextBlock()
{
	if( !m_pRawData ) return FALSE;

	int nLen = GetNextBlockLen();
	if( ( nLen <= 0 ) || ( ( m_nCurrOffset + nLen ) > m_nDataSize ) ) return FALSE;

	m_nCurrOffset += nLen;
	return TRUE;
}

int cAnimatedGIF::GetNextBlockLen() const
{
	GIFBlockTypes nBlock = GetNextBlock();

	int nTmp;

	switch( nBlock )
	{
		case BLOCK_UNKNOWN:
			return -1;
			break;

		case BLOCK_TRAILER:
			return 1;
			break;

		case BLOCK_APPEXT:
			nTmp = GetSubBlocksLen(m_nCurrOffset+sizeof(TGIFAppExtension));
			if( nTmp > 0 ) return sizeof(TGIFAppExtension)+nTmp;
			break;

		case BLOCK_COMMEXT:
			nTmp = GetSubBlocksLen(m_nCurrOffset+sizeof(TGIFCommentExt));
			if(nTmp > 0) return sizeof(TGIFCommentExt)+nTmp;
			break;

		case BLOCK_CONTROLEXT:
			return sizeof(TGIFControlExt);
			break;

		case BLOCK_PLAINTEXT:
			nTmp = GetSubBlocksLen(m_nCurrOffset+sizeof(TGIFPlainTextExt));
			if( nTmp > 0 ) return sizeof(TGIFPlainTextExt)+nTmp;
			break;

		case BLOCK_IMAGE:
			TGIFImageDescriptor *pIDescr = reinterpret_cast<TGIFImageDescriptor *>(&m_pRawData[m_nCurrOffset]);
			int nLCTSize = (int)(pIDescr->GetPackedValue(ID_PACKED_LOCALCT)*3*power(2,pIDescr->GetPackedValue(ID_PACKED_LOCALCTSIZE)+1));

			int nTmp = GetSubBlocksLen(m_nCurrOffset+sizeof(TGIFImageDescriptor) + nLCTSize + 1);
			if( nTmp > 0 ) return sizeof(TGIFImageDescriptor) + nLCTSize + 1 + nTmp;
			break;
	}

	return 0;
}

UINT cAnimatedGIF::GetSubBlocksLen( UINT nStartingOffset ) const
{
	UINT nRet = 0;
	UINT nCurOffset = nStartingOffset;
	
	while( m_pRawData[nCurOffset] != 0 )
	{
		nRet += m_pRawData[nCurOffset]+1;
		nCurOffset += m_pRawData[nCurOffset]+1;
	}

	return nRet+1;
}

UINT cAnimatedGIF::GetSubBlockData( UINT nStartingOffset, unsigned char *lpData )
{
	UINT nCurOffset = nStartingOffset;
	UINT nData = 0;

	while( m_pRawData[nCurOffset] != 0 )
	{
		CopyMemory( &lpData[nData], &m_pRawData[nCurOffset+1], (SIZE_T)m_pRawData[nCurOffset] );
		nData		+= m_pRawData[nCurOffset];
		nCurOffset	+= m_pRawData[nCurOffset]+1;
	}

	return nData;
}

HGLOBAL cAnimatedGIF::GetNextGraphicBlock( UINT *pBlockLen, UINT *pDelay, SIZE *pBlockSize, SIZE *pBlockOffset, UINT *pDisposal )
{
	if( !m_pRawData ) return NULL;

	// GIF header + LSDescriptor [+ GCT] [+ Control block] + Data

	*pDisposal = 0;
	enum GIFBlockTypes nBlock;
	nBlock = GetNextBlock();

	while (
		(nBlock != BLOCK_CONTROLEXT) &&
		(nBlock != BLOCK_IMAGE) &&
		(nBlock != BLOCK_PLAINTEXT) &&
		(nBlock != BLOCK_UNKNOWN) &&
		(nBlock != BLOCK_TRAILER)	)
	{
		if( !SkipNextBlock() ) return NULL;
		nBlock = GetNextBlock();
	}

	if( (nBlock == BLOCK_UNKNOWN) || (nBlock == BLOCK_TRAILER) ) return NULL;

	// it's either a control ext.block, an image or a plain text

	int nStart    = m_nCurrOffset;
	int nBlockLen = GetNextBlockLen();

	if (nBlockLen <= 0) return NULL;

	if (nBlock == BLOCK_CONTROLEXT)
	{
		// get the following data
		TGIFControlExt *pControl = reinterpret_cast<TGIFControlExt *>( &m_pRawData[m_nCurrOffset] );
		// store delay time
		*pDelay = pControl->m_wDelayTime;
		// store disposal method
		*pDisposal = pControl->GetPackedValue(GCX_PACKED_DISPOSAL);

		if( !SkipNextBlock() ) return NULL;
		nBlock = GetNextBlock();
		
		// skip everything until we find data to display 
		// (image block or plain-text block)
		
		while(
			(nBlock != BLOCK_IMAGE) &&
			(nBlock != BLOCK_PLAINTEXT) &&
			(nBlock != BLOCK_UNKNOWN) &&
			(nBlock != BLOCK_TRAILER)	)
		{
			if( !SkipNextBlock() ) return NULL;
			nBlock     = GetNextBlock();
			nBlockLen += GetNextBlockLen();
		}

		if( ( nBlock == BLOCK_UNKNOWN ) || ( nBlock == BLOCK_TRAILER ) ) return NULL;
		nBlockLen += GetNextBlockLen();
	}
	else
		*pDelay = -1; // to indicate that there was no delay value.

	if( nBlock == BLOCK_IMAGE )
	{
		// store size and offsets
		TGIFImageDescriptor *pImage = reinterpret_cast<TGIFImageDescriptor *>( &m_pRawData[m_nCurrOffset] );
		pBlockSize->cx   = pImage->m_wWidth;
		pBlockSize->cy   = pImage->m_wHeight;
		pBlockOffset->cx = pImage->m_wLeftPos;
		pBlockOffset->cy = pImage->m_wTopPos;
	};

	if( !SkipNextBlock() ) return NULL;

	HGLOBAL hGlobal = GlobalAlloc( GMEM_FIXED, sizeof(TGIFHeader) +	sizeof(TGIFLSDescriptor) + m_nGlobalCTSize + nBlockLen +  1 ); // for the trailer
	if( !hGlobal ) return NULL;

	int nOffset = 0; 

	// GMEM_FIXED means we get a pointer
	unsigned char *pGlobal = reinterpret_cast<unsigned char *>( hGlobal );

	CopyMemory( pGlobal, m_pRawData, sizeof(TGIFHeader) + sizeof(TGIFLSDescriptor) + m_nGlobalCTSize );
	nOffset += sizeof(TGIFHeader) + sizeof(TGIFLSDescriptor) + m_nGlobalCTSize;

	CopyMemory( pGlobal + nOffset, &m_pRawData[nStart], nBlockLen );
	nOffset += nBlockLen;

	pGlobal[nOffset] = 0x3B; // trailer
	nOffset++;

	*pBlockLen = nOffset;

	return hGlobal;
}


//---------------------------------------------------------------------------//

#ifdef GIF_TRACE
void cAnimatedGIF::EnumGIFBlocks()
{
	enum GIFBlockTypes nBlock;

	ResetDataPointer();
	while( m_nCurrOffset < m_nDataSize )
	{
		nBlock = GetNextBlock();
		switch(nBlock)
		{
			case BLOCK_UNKNOWN:
				TRACE( L"- Unknown block\n" );
				return;
				break;

			case BLOCK_TRAILER:
				TRACE( L"- Trailer block\n" );
				break;

			case BLOCK_APPEXT:
				TRACE( L"- Application extension block\n" );
				break;

			case BLOCK_COMMEXT:
				TRACE( L"- Comment extension block\n" );
				break;

			case BLOCK_CONTROLEXT:
				{
				TGIFControlExt *pControl = reinterpret_cast<TGIFControlExt *> (&m_pRawData[m_nCurrOffset]);
				TRACE( L"- Graphic control extension block (delay %d, disposal %d)\n",
						pControl->m_wDelayTime, pControl->GetPackedValue(GCX_PACKED_DISPOSAL) );
				};
				break;

			case BLOCK_PLAINTEXT:
				TRACE( L"- Plain text extension block\n" );
				break;

			case BLOCK_IMAGE:
				TGIFImageDescriptor *pIDescr = reinterpret_cast<TGIFImageDescriptor *> (&m_pRawData[m_nCurrOffset]);
				TRACE( L"- Image data block (%dx%d  %d,%d)\n",
						pIDescr->m_wWidth,
						pIDescr->m_wHeight,
						pIDescr->m_wLeftPos,
						pIDescr->m_wTopPos );
				break;
		};

		SkipNextBlock();	
	};

	TRACE( L"\n" );
}

void cAnimatedGIF::WriteDataOnDisk( LPCSTR szFileName, HGLOBAL hData, DWORD dwSize )
{
	FILE *file;


	if( !( file = _wfopen( szFileName, L"wb" ) ) )
	{
		TRACE( L"WriteData: Error creating file %s\n", szFileName );
		return;
	}

	char *pData = reinterpret_cast<char *>( GlobalLock( hData ) );
	if( !pData )
	{
		TRACE( L"WriteData: Error locking memory\n" );
		fclose( file );
		return;
	}

	if( fwrite( pData, 1, dwSize, file) != dwSize )
	{
		TRACE( L"WriteData: Error writing to the file %s\n", szFileName );
		GlobalUnlock( hData );
		fclose( file );
		return;
	}
	
	GlobalUnlock( hData );
	fclose( file );
}
#endif

//---------------------------------------------------------------------------//

BOOL cAnimatedGIF::Load( LPVOID lpBuffer, DWORD nBufferSize )
{
	Unload();

	if( !( m_pRawData = reinterpret_cast<unsigned char*>( lpBuffer ) ) ) return FALSE;

	m_nDataSize = nBufferSize;
	m_pGIFHeader = reinterpret_cast<TGIFHeader *>( m_pRawData );

	if( ( memcmp( &m_pGIFHeader->m_cSignature, "GIF", 3 ) != 0) &&
		( ( memcmp( &m_pGIFHeader->m_cVersion, "87a", 3 ) != 0) ||
		  ( memcmp( &m_pGIFHeader->m_cVersion, "89a", 3 ) != 0) ) )
	{
		// It's neither GIF87a nor GIF89a.
		m_pRawData = NULL;
		return FALSE;
	}

	// It's a GIF.
	m_bIsGIF = TRUE;
	m_pGIFLSDescriptor = reinterpret_cast<TGIFLSDescriptor *>( m_pRawData + sizeof(TGIFHeader) );

	if( m_pGIFLSDescriptor->GetPackedValue( LSD_PACKED_GLOBALCT ) == 1 )
	{
		// calculate the global color table size
		m_nGlobalCTSize = static_cast<int>(3*power(2,m_pGIFLSDescriptor->GetPackedValue(LSD_PACKED_GLOBALCTSIZE)+1));
		CopyMemory( m_GlobalColorTable, m_pRawData + sizeof(TGIFHeader) + sizeof(TGIFLSDescriptor), m_nGlobalCTSize );

		// get the background color if GCT is present
		m_nBackgroundIndex = m_pGIFLSDescriptor->m_cBkIndex;
		unsigned char *pBkClr = m_pRawData + sizeof(TGIFHeader) + sizeof(TGIFLSDescriptor) + 3*m_nBackgroundIndex;
		m_clrBackground = RGB(pBkClr[0],pBkClr[1],pBkClr[2]);
	};

	// store the picture's size
	m_PictureSize.cx = m_pGIFLSDescriptor->m_wWidth;
	m_PictureSize.cy = m_pGIFLSDescriptor->m_wHeight;

	// determine frame count for this picture
	UINT nFrameCount=0;
	ResetDataPointer();
	while( SkipNextGraphicBlock() )	nFrameCount++;

#ifdef GIF_TRACE
	TRACE(
		L" -= GIF encountered\n"
			L"Logical Screen dimensions = %dx%d\n"
			L"Global color table = %d\n"
			L"Color depth = %d\n"
			L"Sort flag = %d\n"
			L"Size of Global Color Table = %d\n"
			L"Background color index = %d\n"
			L"Pixel aspect ratio = %d\n"
			L"Frame count = %d\n"
			L"Background color = %06Xh\n\n",
		m_pGIFLSDescriptor->m_wWidth,
		m_pGIFLSDescriptor->m_wHeight,
		m_pGIFLSDescriptor->GetPackedValue(LSD_PACKED_GLOBALCT),
		m_pGIFLSDescriptor->GetPackedValue(LSD_PACKED_CRESOLUTION),
		m_pGIFLSDescriptor->GetPackedValue(LSD_PACKED_SORT),
		m_pGIFLSDescriptor->GetPackedValue(LSD_PACKED_GLOBALCTSIZE),
		m_pGIFLSDescriptor->m_cBkIndex,
		m_pGIFLSDescriptor->m_cPixelAspect,
		nFrameCount,
		m_clrBackground
		);
	EnumGIFBlocks();
#endif

	if( nFrameCount == 0 ) // it's an empty GIF!
	{
		m_pRawData = NULL;
		return FALSE;
	};


	{
		TFrame	frame;
		UINT	nBlockLen;
		HGLOBAL	hFrameData;
		UINT	nCurFrame = 0;

		ResetDataPointer();
		while( hFrameData = GetNextGraphicBlock( &nBlockLen, &frame.m_nDelay, &frame.m_frameSize, &frame.m_frameOffset, &frame.m_nDisposal ) )
		{
			#ifdef GIF_TRACE
			//////////////////////////////////////////////
			// uncomment the following strings if you want 
			// to write separate frames on disk
			//
			char szName[_MAX_PATH];
			swprintf( szName, L"%.4d.gif", nCurFrame );
			WriteDataOnDisk( szName, hFrameData, nBlockLen );
			nCurFrame++;
			#endif

			char *pData = reinterpret_cast<char *>( GlobalLock( hFrameData ) );
			if( !pData )
			{
				GlobalFree( hFrameData );
				continue;
			}

			frame.m_frameData		= new char[nBlockLen];
			if( frame.m_frameData == NULL )
			{
				frame.m_frameDataSize = 0;
			}
			else
			{
				frame.m_frameDataSize = nBlockLen;
				CopyMemory( frame.m_frameData, pData, nBlockLen );
			}

			GlobalUnlock( hFrameData );
			GlobalFree( hFrameData );

			// everything went well, add this frame
			m_arrFrames.push_back( frame );
		};

		// clean after ourselves
		// FIXME: m_pRawData = NULL;

		if( m_arrFrames.empty() ) return FALSE;		/* LOADFAILED */
	}

	m_bIsLoaded = TRUE;

	return TRUE;
}

void cAnimatedGIF::Unload()
{
	std::deque<TFrame>::iterator it;
	for( it = m_arrFrames.begin(); it < m_arrFrames.end(); it++ ) SAFE_DELETEn( (*it).m_frameData );
	m_arrFrames.clear();

	m_pRawData			= NULL;					// 파일 데이터
	m_nDataSize			= 0;					// 파일 크기
}

BOOL cAnimatedGIF::Load( LPCSTR szFileName )
{
	FILE *file;
	HGLOBAL hGlobal;
	DWORD dwSize;
	
	if( ( file = fopen(szFileName, "rb")) == NULL ) 
	{
		g_cApplication.DisplayError(false,"CIK File Data Error");
		return FALSE; /* FILENOTFOUND */
	}

	fseek( file, 0, SEEK_END );
	dwSize = ftell( file );
	fseek( file, 0, SEEK_SET );

	hGlobal = GlobalAlloc( GMEM_MOVEABLE | GMEM_NODISCARD, dwSize );
	if (!hGlobal) 
	{
		fclose( file );
		return FALSE;	/* OUTOFMEMORY */
	};
	
	char *pData = reinterpret_cast<char*>( GlobalLock(hGlobal) );
	if (!pData)
	{
		GlobalFree( hGlobal );
		fclose( file );
		return FALSE;	/* LOCKFAILED */
	};

	if( fread( pData, 1, dwSize, file) != dwSize )
	{
		GlobalUnlock( hGlobal );
		GlobalFree(hGlobal);
		fclose(file);
		return FALSE;	/* READFAILED */
	};

	fclose( file );

	BOOL bRetValue = Load( pData, dwSize );

	GlobalUnlock( hGlobal );
	GlobalFree( hGlobal );

	return bRetValue;
}


//---------------------------------------------------------------------------//
// 클래스 멤버 변수 접근 함수들
//---------------------------------------------------------------------------//

BOOL cAnimatedGIF::IsLoaded() const
{
	return m_bIsLoaded;
}

BOOL cAnimatedGIF::IsGIF() const
{
	return m_bIsGIF;
}

SIZE cAnimatedGIF::GetSize() const
{
	return m_PictureSize;
}

BOOL cAnimatedGIF::IsAnimatedGIF() const
{
	return ( m_bIsGIF && ( m_arrFrames.size() > 1 ) );
}

int cAnimatedGIF::GetFrameCount() const
{
	if( !IsLoaded() ) return 0;
	return (int)m_arrFrames.size();
}

TFrame *cAnimatedGIF::GetTFrame( int frame ) const
{
	return (TFrame *)&m_arrFrames[frame];
}

//---------------------------------------------------------------------------//

