/*-----------------------------------------------------------------------------
 * FILENAME:
 *			2DAnimation.cpp
 *
 * DESCRIPTION:
 *			2D 애니메이션 지원 클래스
 *
 * HISTORY:
 *			2004.11.08 - 칼라키를 사용하지 않도록 수정함.
 *			2004.11.08 - 파일 로드 성공 여부를 검사할 수 있는 IsLoaded() 추가함. 
 *----------------------------------------------------------------------------*/
#include "StdAfx.h"
#include "AnimatedGIF.h"
#include "2DAnimation.h"

//---------------------------------------------------------------------------//
// GIF(색인) 이미지 데이터를 텍스져(색상) 이미지 데이터로 변환해주는 모듈
//---------------------------------------------------------------------------//

#define ARGB(a,r,g,b)	((DWORD)(((DWORD)(BYTE)(b))|(((DWORD)(BYTE)(g))<<8)|(((DWORD)(BYTE)(r))<<16)|(((DWORD)(BYTE)(a))<<24)))

BOOL Indexed2ColoredImage( void *lpColoredImage, int nColoredWidth, int nColoredHeight,
						   const void *lpIndexedImage, int nXOffset, int nYOffset, int nIndexedWidth, int nIndexedHeight,
						   const void *lpCTable, const BOOL bTransparent, const int nTransparentIndex, 
						   const BYTE trRed, const BYTE trGreen, const BYTE trBlue )
{
	BYTE index;
	LPGIFCOLORTABLE GCT = (LPGIFCOLORTABLE)lpCTable;

	for( int y = 0; y < nIndexedHeight; y++ )
	{
		for( int x = 0; x < nIndexedWidth; x++ )
		{
			index = ((BYTE*)lpIndexedImage)[ y*nIndexedWidth + x ];

			/* case D3DFMT_A8R8G8B8: */
			if( bTransparent && index == nTransparentIndex )
				((DWORD*)lpColoredImage)[ (y+nYOffset) * nColoredWidth + (x+nXOffset) ] = ARGB( 0x00, trRed, trGreen, trBlue );
			else
				((DWORD*)lpColoredImage)[ (y+nYOffset) * nColoredWidth + (x+nYOffset) ] = ARGB( 0xFF, GCT[index].r, GCT[index].g, GCT[index].b );
		}
	}

	return TRUE;
}

BOOL GetIndexedImage( const BYTE *lpRawData, int nRawDataSize, WORD MinimumCodeSize, BYTE *lpIndexedImage, int nIndexedImageSize )
{
	BOOL bSuccessful	= FALSE;

	WORD ClearCode		= ( 0x0001 << MinimumCodeSize );
	WORD ExitCode		= ClearCode + 1;
	WORD FirstEntry		= ClearCode + 2;
	WORD InitCodeSize	= MinimumCodeSize + 1;

	DWORD nReadKey		= 0;
	DWORD RawCode		= 0;
	WORD NextEntry		= FirstEntry;
	WORD CodeSize		= InitCodeSize;

	WORD Code, PrevCode, OutCode;
	BYTE OutStack[4096+1];
	WORD Prefix[4096];
	BYTE Suffix[4096];
	int  OutIndex;

	BYTE *pData			= lpIndexedImage;
	int  nData			= 0;

	while( TRUE )
	{
		OutIndex = 0;

		// DWORD(4 byte)로 확장
		RawCode = lpRawData[ (nReadKey>>3)     ]       | 
				  lpRawData[ (nReadKey>>3) + 1 ] << 8  | 
				  lpRawData[ (nReadKey>>3) + 2 ] << 16 |
				  lpRawData[ (nReadKey>>3) + 3 ] << 24;
		RawCode >>= (nReadKey&0x00000007);
		Code = (WORD)( RawCode & ( (0x0001<<CodeSize) -1 ) );
		nReadKey += CodeSize;

		if( Code == ExitCode ) break;
		if( Code == ClearCode )
		{
			NextEntry = FirstEntry;
			CodeSize = InitCodeSize;
			PrevCode = Code;
			continue;
		}

		if( Code < NextEntry ) OutCode = Code;
		else
		{
			OutIndex++;
			OutCode = PrevCode;
		}

		while( OutCode >= FirstEntry )
		{
			if( OutIndex >= 4096 ) goto END_OF_FUNC;
			OutStack[OutIndex++] = Suffix[OutCode];
			OutCode = Prefix[OutCode];
		}
		if( OutIndex >= 4096 ) goto END_OF_FUNC;
		OutStack[OutIndex++] = (BYTE)OutCode;

		if( PrevCode != ClearCode )
		{
			Prefix[NextEntry] = PrevCode;
			Suffix[NextEntry] = (BYTE)OutCode;
			NextEntry++;
			if( NextEntry >= (0x0001<<CodeSize) )
			{
				if( CodeSize < 12 ) CodeSize++;
				else /* 다음은 클리어 코드가 온다. */;
			}
		}

		PrevCode = Code;

		for( int n = OutIndex - 1; n >= 0; n-- )
		{
			if( nData >= nIndexedImageSize ) goto END_OF_FUNC;
			pData[nData++] = OutStack[n];
		}
	}

	bSuccessful = TRUE;

END_OF_FUNC:
	return bSuccessful;
}

BYTE *DecodeImage( cAnimatedGIF *picGIF, LPBYTE framebuffer, int nDisposal, BOOL bTransparentColor, int nTransparentColorIndex  )
{
	SIZE BlockSize;
	SIZE BlockOffset;
	BOOL bLocalColorTable;
	BOOL bInterlace;
	BOOL bLocalSort;
	int  nLocalCTSize;
	GIFCOLORTABLE arrayLocalColorTable[256];
	int  nMinimumCodeSize;
	SIZE picSize = picGIF->GetSize();
	COLORREF clrBKGND = picGIF->GetBackgroundColor();

	BYTE *EncodedImage = NULL;			// 인코드된 이미지
	BYTE *DecodedImage = NULL;			// 디코드된 이미지
	BYTE *ColoredImage = NULL;			// 색변환된 이미지
	BYTE *SavedImage   = NULL;			// 복구용 데이터

	// store size and offsets.
	TGIFImageDescriptor *pImage = reinterpret_cast<TGIFImageDescriptor *>( picGIF->GetCurrRawData() );
	
	BlockSize.cx	= pImage->m_wWidth;
	BlockSize.cy	= pImage->m_wHeight;
	BlockOffset.cx	= pImage->m_wLeftPos;
	BlockOffset.cy	= pImage->m_wTopPos;
	bLocalColorTable= pImage->GetPackedValue( ID_PACKED_LOCALCT );
	bInterlace		= pImage->GetPackedValue( ID_PACKED_INTERLACE );
	bLocalSort		= pImage->GetPackedValue( ID_PACKED_SORT );
	if( bLocalColorTable )
	{
		// calculate the global color table size
		nLocalCTSize = static_cast<int>(3*power(2,pImage->GetPackedValue( ID_PACKED_LOCALCTSIZE )+1));
		CopyMemory( arrayLocalColorTable, picGIF->GetCurrRawData() + sizeof(TGIFImageDescriptor), nLocalCTSize );
	}
	else
	{
		nLocalCTSize = 0;
	}
	nMinimumCodeSize = (int)( *(BYTE*)( picGIF->GetCurrRawData() + sizeof(TGIFImageDescriptor) + nLocalCTSize ) );

	int nBlockLength = picGIF->GetSubBlocksLen( picGIF->GetCurrOffset() + sizeof(TGIFImageDescriptor) + nLocalCTSize + 1 );

	if( !( EncodedImage = new BYTE[ nBlockLength + 3/*padding*/ ] ) ) goto FAILED;

	int nImageLength = picGIF->GetSubBlockData( picGIF->GetCurrOffset() + sizeof(TGIFImageDescriptor) + nLocalCTSize + 1, EncodedImage );
	ASSERT( nBlockLength > nImageLength );

	// clear out padding data.
	ZeroMemory( &EncodedImage[nImageLength], nBlockLength + 3 - nImageLength );

	LONG DecodedImageSize = BlockSize.cx * BlockSize.cy;
	if( !( DecodedImage = new BYTE[ DecodedImageSize ] ) ) goto FAILED;

	if( !( GetIndexedImage( EncodedImage, nImageLength, nMinimumCodeSize, DecodedImage, DecodedImageSize ) ) ) goto FAILED;

	//////////////////////////////////////////////////////////////
	// Before continuing to render a frame we should take care of
	// what's behind that frame. m_nDisposal will be our guide:
	//   0 - no disposal specified (do nothing)
	//   1 - do not dispose (again, do nothing)
	//   2 - restore to background color (m_clrBackground)
	//   3 - restore to previous
	int  x, y;

	switch( nDisposal )
	{
		case 0:
		case 1:
		case 2:		// 미리 할 작업 없음
			break;
		case 3:		// 복구용 버퍼 생성
		{
			SAFE_DELETEn( SavedImage );

			if( !( SavedImage = new BYTE[ BlockSize.cx * BlockSize.cy ] ) ) goto FAILED;

			for( y = 0; y < BlockSize.cy; y++ )
				for( x = 0; x < BlockSize.cx; x++ )
				{
					SavedImage[ y*BlockSize.cx + x ] = framebuffer[ (y+BlockOffset.cy)*picSize.cx + (x+BlockOffset.cx) ];
				}
			break;
		}
	}

	// interlace 플래그 방식 처리(나중)
	if( bInterlace )
	{
		ASSERT( !"Interlace is not implemented!" );
		goto FAILED;
	}

	// 해당 프레임 이미지를 프레임 버퍼에 쓴다.
	for( y = 0; y < BlockSize.cy; y++ )
	{
		for( x = 0; x < BlockSize.cx; x++ )
		{
			// Indexed2ColoredImage() 에서 투명색을 변경해 주므로 아래 코드는 필요없다. 오히려 문제를
			// 발생시켜 주석문 처리한다. <-- ???
			if( bTransparentColor && DecodedImage[ y*BlockSize.cx + x ] == nTransparentColorIndex ) continue;
				
			framebuffer[ (y+BlockOffset.cy)*picSize.cx + (x+BlockOffset.cx) ] = DecodedImage[ y*BlockSize.cx + x ];
		}
	}

	// D3DFMT_A8R8G8B8 색상 형식을 저장하기 위해서는 sizeof(DWORD) 바이트가 필요하다.
	ColoredImage = new BYTE[ picSize.cx * picSize.cy * sizeof(DWORD) ];
	if( !ColoredImage ) goto FAILED;

	if( !Indexed2ColoredImage( ColoredImage, picSize.cx, picSize.cy,
						/*DecodedImage, BlockOffset.cx, BlockOffset.cy, BlockSize.cx, BlockSize.cy,*/
						framebuffer, 0, 0, picSize.cx, picSize.cy,
						picGIF->GetGlobalColorTable(), bTransparentColor, nTransparentColorIndex, 
						GetRValue(clrBKGND), GetGValue(clrBKGND), GetBValue(clrBKGND) ) ) goto FAILED;

	// perform dispoal method.
	switch( nDisposal )
	{
		case 0:			// do nothing
		case 1:
			break;
		case 2:			// 배경색으로 복구
		{
			for( y = 0; y < picSize.cy; y++ )
				for( x = 0; x < picSize.cx; x++ )
				{
					if( y < BlockOffset.cy || x < BlockOffset.cx ||
						y >= BlockOffset.cy + BlockSize.cy ||
						x >= BlockOffset.cx + BlockSize.cx ) continue;
					framebuffer[ y*picSize.cx + x ] = picGIF->GetBackgroundColorIndex();
				}
			break;
		}
		case 3:			// 복구 데이터로 복구
		{
			if( !SavedImage ) break;

			for( y = 0; y < BlockSize.cy; y++ )
				for( x = 0; x < BlockSize.cx; x++ )
				{
					framebuffer[ (y+BlockOffset.cy)*picSize.cx + (x+BlockOffset.cx) ] = SavedImage[ y*BlockSize.cx + x ];
				}

			SAFE_DELETEn( SavedImage );
			break;
		}
	}

	SAFE_DELETEn( EncodedImage );
	SAFE_DELETEn( DecodedImage );

	return ColoredImage;

FAILED:
	SAFE_DELETEn( EncodedImage );
	SAFE_DELETEn( DecodedImage );
	SAFE_DELETEn( ColoredImage );

	return FALSE;
}

void *DecodeFrame( TFrame *frame, LPBYTE framebuffer )
{
	BYTE *FrameImage = NULL;			// 프레임 이미지

	cAnimatedGIF picGIF;

	int  nDisposal = 0;
	BOOL bUserInput = FALSE;
	BOOL bTransparentColor = FALSE;
	int  nTransparentColorIndex = 0;
	int  nDelayTime = 0;

	if( !picGIF.Load( frame->m_frameData, frame->m_frameDataSize ) ) return NULL;

	picGIF.ResetDataPointer();

	if( !picGIF.GetRawData() ) return NULL;

	enum GIFBlockTypes nBlock;
	nBlock = picGIF.GetNextBlock();

	while (
		(nBlock != BLOCK_CONTROLEXT) &&
		(nBlock != BLOCK_IMAGE) &&
		(nBlock != BLOCK_PLAINTEXT) &&
		(nBlock != BLOCK_UNKNOWN) &&
		(nBlock != BLOCK_TRAILER)	)
	{
		if( !picGIF.SkipNextBlock() ) return NULL;
		nBlock = picGIF.GetNextBlock();
	}

	if( (nBlock == BLOCK_UNKNOWN) || (nBlock == BLOCK_TRAILER) ) return NULL;

	// it's either a control ext.block, an image or a plain text

	int nStart    = picGIF.GetCurrOffset();
	int nBlockLen = picGIF.GetNextBlockLen();

	if (nBlockLen <= 0) return NULL;

	if (nBlock == BLOCK_CONTROLEXT)
	{
		// get the following data
		TGIFControlExt *pControl = reinterpret_cast<TGIFControlExt *>( picGIF.GetCurrRawData() );

		nDisposal = pControl->GetPackedValue( GCX_PACKED_DISPOSAL );
		bUserInput = pControl->GetPackedValue( GCX_PACKED_USERINPUT );
		bTransparentColor = pControl->GetPackedValue( GCX_PACKED_TRANSPCOLOR );
		nTransparentColorIndex = pControl->m_cTColorIndex;
		nDelayTime = pControl->m_wDelayTime;

		if( !picGIF.SkipNextBlock() ) return NULL;
		nBlock = picGIF.GetNextBlock();
		
		// skip everything until we find data to display 
		// (image block or plain-text block)
		
		while(
			(nBlock != BLOCK_IMAGE) &&
			(nBlock != BLOCK_PLAINTEXT) &&
			(nBlock != BLOCK_UNKNOWN) &&
			(nBlock != BLOCK_TRAILER)	)
		{
			if( !picGIF.SkipNextBlock() ) return NULL;
			nBlock     = picGIF.GetNextBlock();
			nBlockLen += picGIF.GetNextBlockLen();
		}

		if( ( nBlock == BLOCK_UNKNOWN ) || ( nBlock == BLOCK_TRAILER ) ) return NULL;
		nBlockLen += picGIF.GetNextBlockLen();
	}
	else
	{
		nDelayTime = -1;	// to indicate that there was no delay value.
	}

	if( nBlock == BLOCK_IMAGE )
	{
		if( !( FrameImage = DecodeImage( &picGIF, framebuffer, nDisposal, bTransparentColor, nTransparentColorIndex ) ) ) goto FAILED;
	}

	if( !picGIF.SkipNextBlock() ) goto FAILED;

	picGIF.Unload();

	return FrameImage;

FAILED:
	picGIF.Unload();

	SAFE_DELETEn( FrameImage );

	return NULL;
}

//---------------------------------------------------------------------------//
// c2DAnimationFrames class
//---------------------------------------------------------------------------//

c2DAnimationFrames::c2DAnimationFrames()
{
	m_nHandle		= 0;
	m_AnimationSize.cx = 0;
	m_AnimationSize.cy = 0;
	m_nLevel		= 0;
	m_bIsPlaying	= FALSE;
	m_nCurrFrame	= 0;
	m_dwLastFrame	= 0;
	m_bDiscontinued = FALSE;
	m_bEndOfPlay	= FALSE;
	m_bRepeat		= TRUE;
	m_FrameBuffer	= NULL;
}

c2DAnimationFrames::~c2DAnimationFrames()
{
	DeleteFrameBuffer();

	for( int i = 0; i < m_FrameList.GetSize(); i++ )
	{
		c2DAnimationFrame* aniFrame = m_FrameList.GetAt( i );
		SAFE_DELETE( aniFrame );
	}
	m_FrameList.RemoveAll();
}

void c2DAnimationFrames::SetAnimationSize( LONG cx, LONG cy )
{
	m_AnimationSize.cx = cx;
	m_AnimationSize.cy = cy;
}

BOOL c2DAnimationFrames::CreateFrameBuffer( int nBackgroundColorIndex )
{
	if( !( m_FrameBuffer = new BYTE[ m_AnimationSize.cx * m_AnimationSize.cy ] ) ) return FALSE;

	if( nBackgroundColorIndex >= 0 && nBackgroundColorIndex <= 255 )
	{
		memset( m_FrameBuffer, nBackgroundColorIndex, m_AnimationSize.cx * m_AnimationSize.cy );
	}

	return TRUE;
}

void c2DAnimationFrames::DeleteFrameBuffer()
{
	SAFE_DELETEn( m_FrameBuffer );
}

BOOL c2DAnimationFrames::Add( TFrame *Tframe, D3DFORMAT nFormat )
{
	c2DAnimationFrame *aniFrame = new c2DAnimationFrame;
	if( !aniFrame ) return FALSE;

	aniFrame->m_frameSize		= Tframe->m_frameSize;
	aniFrame->m_nDelay			= Tframe->m_nDelay;
	aniFrame->m_nFormat			= nFormat;
	aniFrame->m_pBits			= (DWORD*)DecodeFrame( Tframe, m_FrameBuffer );

	m_FrameList.Add( aniFrame );

	return TRUE;
}

BOOL c2DAnimationFrames::Draw( DWORD *lpBits )
{
	c2DAnimationFrame* aniFrame;

	ASSERT( m_FrameList.GetSize() > 0 );
	m_bEndOfPlay = FALSE;

	if( m_dwLastFrame == 0 )	// 플래이 시작
	{
		m_nCurrFrame = 0;
		aniFrame = m_FrameList.GetAt( m_nCurrFrame );

		DWORD nDelay = ( aniFrame->m_nDelay < 5 ) ? 100 : 10 * aniFrame->m_nDelay;
		m_dwLastFrame = GetTickCount() + nDelay;
	}

	aniFrame = m_FrameList.GetAt( m_nCurrFrame );

#ifdef _UNUSED_CODE_
	int w = m_AnimationSize.cx;
	int h = m_AnimationSize.cy;
	DWORD colorValue;

	for( int y = 0; y < h; y++ )
		for( int x = 0; x < w; x++ )
		{
			colorValue = aniFrame->m_pBits[ y*w + x ];
			lpBits[ y*m_AnimationSize.cx + x ] = colorValue;
		}
#endif

	CopyMemory( lpBits, aniFrame->m_pBits, m_AnimationSize.cx * m_AnimationSize.cy * sizeof(DWORD) );

	if( GetTickCount() >= m_dwLastFrame && !m_bDiscontinued )
	{
		// 다음 프레임으로 이동한다.
		m_nCurrFrame = m_nCurrFrame + 1;
		if( m_nCurrFrame == m_FrameList.GetSize() )
		{
			if( m_bRepeat )
			{
				m_nCurrFrame = 0;
			}
			else
			{
				m_nCurrFrame = m_FrameList.GetSize() - 1;
				m_bEndOfPlay = TRUE;
			}
		}

		aniFrame = m_FrameList.GetAt( m_nCurrFrame );
		DWORD nDelay = ( aniFrame->m_nDelay < 5 ) ? 100 : 10 * aniFrame->m_nDelay;
		m_dwLastFrame = GetTickCount() + nDelay;
	}

	return TRUE;
}

void c2DAnimationFrames::Stop()
{
	m_nCurrFrame = 0;
	m_dwLastFrame = 0;
}

//---------------------------------------------------------------------------//

c2DAnimation::c2DAnimation()
{
	m_bIsLoaded		= FALSE;
	m_bDiscontinued	= FALSE;

	m_Width			= 0;
	m_Height		= 0;
	m_D3DSurface	= NULL;

	m_nFormat		= D3DFMT_A8R8G8B8;
	m_ColorKey		= D3DCOLOR_ARGB(0,0,0,0);
}

c2DAnimation::~c2DAnimation()
{
	Free();
}

BOOL c2DAnimation::Init( LPDIRECT3DDEVICE9 pd3dDevice, UINT width, UINT height )
{
	m_Width  = width;
	m_Height = height;

	HRESULT hr = pd3dDevice->CreateOffscreenPlainSurface(
					m_Width,
					m_Height,
					m_nFormat,
					D3DPOOL_SYSTEMMEM,
					&m_D3DSurface, 
					NULL );
	if( FAILED( hr ) ) return FALSE;

	return TRUE;
}

BOOL c2DAnimation::Free()
{
	m_bIsLoaded		= FALSE;

	for( int i = 0; i < m_AnimationList.GetSize(); i++ )
	{
		c2DAnimationFrames* aniFrames = m_AnimationList.GetAt( i );
		SAFE_DELETE( aniFrames );
	}
	m_AnimationList.RemoveAll();

	SAFE_RELEASE( m_D3DSurface );

	return TRUE;
}

DWORD c2DAnimation::Create( LPCSTR lpwszFile )
{
	cAnimatedGIF aniGIF;

	if( !aniGIF.Load( lpwszFile ) ) return INVALID_HANDLE;

	c2DAnimationFrames *aniFrames = new c2DAnimationFrames;
	if( !aniFrames ) return INVALID_HANDLE;

	SIZE aniSize = aniGIF.GetSize();
	aniFrames->SetAnimationSize( aniSize.cx, aniSize.cy );

	aniFrames->CreateFrameBuffer( aniGIF.GetBackgroundColorIndex() );

	for( int i = 0; i < aniGIF.GetFrameCount(); i++ )
	{
		TFrame *frame = aniGIF.GetTFrame( i );
		if( !aniFrames->Add( frame ) )
		{
			delete aniFrames;
			return INVALID_HANDLE;
		}
	}

	aniFrames->DeleteFrameBuffer();
	m_AnimationList.Add( aniFrames );
	
	m_bIsLoaded	= TRUE;

	return m_AnimationList.GetSize() - 1;
}

BOOL c2DAnimation::Draw( LPDIRECT3DTEXTURE9 lpTexture )
{
	D3DLOCKED_RECT lockedRect;
	m_D3DSurface->LockRect( &lockedRect, NULL, D3DLOCK_DISCARD );
	DWORD* pBits = (DWORD*)lockedRect.pBits;

#ifdef _UNUSED_CODE_
	// blacken out the locked rectangle.
	memset( pBits, 0x00, lockedRect.Pitch/*m_Width * sizeof(DWORD)*/ * m_Height );
#endif

	for( int i = 0; i < m_AnimationList.GetSize(); i++ )
	{
		c2DAnimationFrames* aniFrames = m_AnimationList.GetAt( i );
		aniFrames->Draw( pBits );
	}

	m_D3DSurface->UnlockRect();

	LPDIRECT3DSURFACE9 pDestSurface = NULL;

	HRESULT hr = lpTexture->GetSurfaceLevel( 0, &pDestSurface );
	if( FAILED( hr ) ) return FALSE;

	hr = D3DXLoadSurfaceFromSurface( pDestSurface, NULL, NULL, m_D3DSurface, NULL, NULL, D3DX_FILTER_NONE, m_ColorKey );
	if( FAILED( hr ) )
	{
		pDestSurface->Release();
		return FALSE;
	}

	pDestSurface->Release();
	return TRUE;
}

void c2DAnimation::Stop()
{
	for( int i = 0; i < m_AnimationList.GetSize(); i++ )
	{
		c2DAnimationFrames* aniFrames = m_AnimationList.GetAt( i );
		aniFrames->Stop();
	}
}

void c2DAnimation::Repeat( BOOL bRepeat )
{
	for( int i = 0; i < m_AnimationList.GetSize(); i++ )
	{
		c2DAnimationFrames* aniFrames = m_AnimationList.GetAt( i );
		aniFrames->Repeat( bRepeat );
	}
}

void c2DAnimation::Discontinue()
{
	for( int i = 0; i < m_AnimationList.GetSize(); i++ )
	{
		c2DAnimationFrames* aniFrames = m_AnimationList.GetAt( i );
		aniFrames->Discontinue();
	}
}

void c2DAnimation::Continue()
{
	for( int i = 0; i < m_AnimationList.GetSize(); i++ )
	{
		c2DAnimationFrames* aniFrames = m_AnimationList.GetAt( i );
		aniFrames->Continue();
	}
}

BOOL c2DAnimation::IsEndOfPlay()
{
	int  nCount = 0;

	for( int i = 0; i < m_AnimationList.GetSize(); i++ )
	{
		c2DAnimationFrames* aniFrames = m_AnimationList.GetAt( i );
		if( aniFrames->IsEndOfPlay() ) nCount++;
	}

	return ( nCount == m_AnimationList.GetSize() ) ? TRUE : FALSE;
}

