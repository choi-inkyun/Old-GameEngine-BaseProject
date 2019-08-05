
#ifndef __UISTATIC_H__
#define __UISTATIC_H__

//-----------------------------------------------------------------------------
// Static control
//-----------------------------------------------------------------------------
class __declspec(dllexport) cUIStatic : public cUIControl
{
public:
	enum STATIC_TYPE
	{
		STATICTYPE_TEXT,					// 텍스트 출력
		STATICTYPE_IMAGE					// 이미지 출력
	};

public:
	cUIStatic( cUIDialog *pDialog = NULL );

	virtual void Render( IDirect3DDevice9* pd3dDevice );
	virtual BOOL ContainsPoint( POINT pt ) { return FALSE; }

	void SetStaticType( STATIC_TYPE type ) { m_StaticType = type; }
	HRESULT GetTextCopy( LPSTR strDest, UINT bufferCount );
	LPCSTR GetText() { return m_strText; }
	HRESULT SetText( LPCSTR strText );

protected:
	STATIC_TYPE m_StaticType;
	char m_strText[MAX_PATH];				// Window text
};


#endif

