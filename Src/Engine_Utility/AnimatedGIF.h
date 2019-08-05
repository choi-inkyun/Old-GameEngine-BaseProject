/*-----------------------------------------------------------------------------
 * FILENAME:
 *			cAnimatedGIF.h
 *
 *---------------------------------------------------------------------------*/

#ifndef __ANIMATEDGIF_H__
#define __ANIMATEDGIF_H__

#include <deque>		// std::deque<>

//---------------------------------------------------------------------------//

//#define GIF_TRACE		// for debugging.
//#define TRACE			// make it empty.

//---------------------------------------------------------------------------//

#pragma pack(1)			// turn byte alignment on

enum LSDPackedValues	// logical screen descriptor packed field values
{
	LSD_PACKED_GLOBALCT,
	LSD_PACKED_CRESOLUTION,
	LSD_PACKED_SORT,
	LSD_PACKED_GLOBALCTSIZE
};

enum GIFBlockTypes
{
	BLOCK_UNKNOWN,
	BLOCK_APPEXT,
	BLOCK_COMMEXT,
	BLOCK_CONTROLEXT,
	BLOCK_PLAINTEXT,
	BLOCK_IMAGE,
	BLOCK_TRAILER
};

enum ControlExtValues // graphic control extension packed field values
{
	GCX_PACKED_DISPOSAL,  // disposal method
	GCX_PACKED_USERINPUT,
	GCX_PACKED_TRANSPCOLOR
};

enum IDPackedValues   // image descriptor packed field values
{
	ID_PACKED_LOCALCT,
	ID_PACKED_INTERLACE,
	ID_PACKED_SORT,
	ID_PACKED_LOCALCTSIZE
};


struct TGIFHeader       // GIF header  
{
	char m_cSignature[3]; // Signature - Identifies the GIF Data Stream
						  // This field contains the fixed value 'GIF'
	char m_cVersion[3];	// Version number. May be one of the following:
						// "87a" or "89a"
};

struct TGIFLSDescriptor // Logical Screen Descriptor
{
	WORD m_wWidth;	// 2 bytes. Logical screen width
	WORD m_wHeight; // 2 bytes. Logical screen height

	unsigned char m_cPacked;      // packed field	

	unsigned char m_cBkIndex;     // 1 byte. Background color index
	unsigned char m_cPixelAspect; // 1 byte. Pixel aspect ratio
	inline int GetPackedValue(enum LSDPackedValues Value);
};

struct TGIFAppExtension // application extension block
{
	unsigned char m_cExtIntroducer; // extension introducer (0x21)
	unsigned char m_cExtLabel; // app. extension label (0xFF)
	unsigned char m_cBlockSize; // fixed value of 11
	char m_cAppIdentifier[8];   // application identifier
	char m_cAppAuth[3];  // application authentication code
};

struct TGIFCommentExt  // comment extension block
{
	unsigned char m_cExtIntroducer; // extension introducer (0x21)
	unsigned char m_cCommentLabel;  // comment extension label (0xFE)
};

struct TGIFControlExt // graphic control extension block
{
	unsigned char m_cExtIntroducer; // extension introducer (0x21)
	unsigned char m_cControlLabel;  // control extension label (0xF9)
	unsigned char m_cBlockSize; // fixed value of 4
	unsigned char m_cPacked;    // packed field
	WORD m_wDelayTime;	// delay time
	unsigned char m_cTColorIndex; // transparent color index
	unsigned char m_cBlockTerm;   // block terminator (0x00)
public:
	inline int GetPackedValue(enum ControlExtValues Value);
};

struct TGIFPlainTextExt // plain text extension block
{
	unsigned char m_cExtIntroducer;  // extension introducer (0x21)
	unsigned char m_cPlainTextLabel; // text extension label (0x01)
	unsigned char m_cBlockSize; // fixed value of 12
	WORD m_wLeftPos;    // text grid left position
	WORD m_wTopPos;     // text grid top position
	WORD m_wGridWidth;  // text grid width
	WORD m_wGridHeight; // text grid height
	unsigned char m_cCellWidth;  // character cell width
	unsigned char m_cCellHeight; // character cell height
	unsigned char m_cFgColor; // text foreground color index
	unsigned char m_cBkColor; // text background color index
};

struct TGIFImageDescriptor // image descriptor block
{
	unsigned char m_cImageSeparator; // image separator byte (0x2C)
	WORD m_wLeftPos; // image left position
	WORD m_wTopPos;  // image top position
	WORD m_wWidth;   // image width
	WORD m_wHeight;  // image height
	unsigned char m_cPacked; // packed field
	inline int GetPackedValue(enum IDPackedValues Value);
};

typedef struct TGIFCOLORTABLE	// color table
{
	BYTE r;
	BYTE g;
	BYTE b;
} GIFCOLORTABLE, *LPGIFCOLORTABLE;

#pragma pack() // turn byte alignment off

struct TFrame    // structure that keeps a single frame info
{
	char*		m_frameData;				// ������ ������
	UINT		m_frameDataSize;			// ������ ������ ũ��
	SIZE		m_frameSize;				// ������ ������
	SIZE		m_frameOffset;				// ������ ��ġ ����
	UINT		m_nDelay;					// delay (in 1/100s of a second)
	UINT		m_nDisposal;				// disposal method
};


class cAnimatedGIF
{
private:
	unsigned char*	m_pRawData;				// ���� ������
	UINT			m_nDataSize;			// ���� ũ��
	UINT			m_nCurrOffset;			// ���Ͽ��� ���� �׸� ������ ��ġ

	TGIFHeader*		m_pGIFHeader;			// GIF ���� ���
	TGIFLSDescriptor* m_pGIFLSDescriptor;	// GIF LS ��ũ����

	BOOL			m_bIsLoaded;			// ���� �б� �÷���
	BOOL			m_bIsGIF;				// GIF ���� �÷���
	UINT			m_nGlobalCTSize;		// global color table size
	GIFCOLORTABLE	m_GlobalColorTable[256]; // global color table
	int				m_nBackgroundIndex;		// ���� �ε��� ��ȣ
	COLORREF		m_clrBackground;		// ����
	SIZE			m_PictureSize;			// �׸� ũ��

	std::deque<TFrame> m_arrFrames;			// �����Ӻ� �׸� ������

public:
	cAnimatedGIF();
	~cAnimatedGIF();

	void ResetDataPointer();
	BOOL SkipNextGraphicBlock();
	enum GIFBlockTypes GetNextBlock() const;
	BOOL SkipNextBlock();
	int  GetNextBlockLen() const;
	UINT GetSubBlocksLen( UINT nStartingOffset ) const;
	UINT GetSubBlockData( UINT nStartingOffset, unsigned char *lpData );
	HGLOBAL GetNextGraphicBlock( UINT *pBlockLen, UINT *pDelay, SIZE *pBlockSize, SIZE *pBlockOffset, UINT *pDisposal );

#ifdef GIF_TRACE
	void EnumGIFBlocks();
	void WriteDataOnDisk( LPCSTR szFileName, HGLOBAL hData, DWORD dwSize );
#endif

	BOOL Load( LPVOID lpBuffer, DWORD nBufferSize );
	BOOL Load( LPCSTR szFileName );

	// releases all resources used by loading.
	void Unload();

	BOOL IsLoaded() const;
	BOOL IsGIF() const;
	SIZE GetSize() const;
	BOOL IsAnimatedGIF() const;
	int  GetFrameCount() const;
	TFrame *GetTFrame( int frame ) const;

	unsigned char* GetRawData() const { return m_pRawData; }
	UINT GetCurrOffset() const { return m_nCurrOffset; }
	unsigned char* GetCurrRawData() const { return &m_pRawData[m_nCurrOffset]; }
	LPGIFCOLORTABLE GetGlobalColorTable() const { return (LPGIFCOLORTABLE)m_GlobalColorTable; }
	int  GetBackgroundColorIndex() const { return m_nBackgroundIndex; }
	COLORREF GetBackgroundColor() const { return m_clrBackground; }
};

inline int power(int a, int b)
{
	int count;//�� ���� sum�� a�� ������ ����
	int sum=a;//����� ������ ���� �ִ´�
	//sum�� a�� b��ŭ ���ؼ� �ִ´�(sum *= a�� sum = sum*a�� �����ϴ�)
	for(count=1; count<b; ++count)
	{
		sum *= a; 
	}
	return sum;//��� �Ϸ��� sum�� �����Ѵ�.
}

#endif // end of #ifndef __ANIMATEDGIF_H__

