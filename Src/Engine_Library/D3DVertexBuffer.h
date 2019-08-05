
#ifndef __ENGINE_D3DVERTEXBUFFER_H__
#define __ENGINE_D3DVERTEXBUFFER_H__

#include "D3DGraphics.h"

/*-----------------------------------------------------------------------------
 * cVertexBuffer CLASS
 *---------------------------------------------------------------------------*/

class cVertexBuffer
{
private:
	cGraphics				*m_Graphics;
	IDirect3DVertexBuffer9	*m_pVB;

	DWORD					 m_NumVertices;
	DWORD					 m_VertexSize;
	DWORD					 m_FVF;

	BOOL					 m_Locked;
	char					*m_Ptr;

public:
	cVertexBuffer();
	~cVertexBuffer();

	IDirect3DVertexBuffer9	*GetVertexBufferCOM();
	unsigned long			 GetVertexSize();
	unsigned long			 GetVertexFVF();
	unsigned long			 GetNumVertices();
	BOOL					 IsLoaded();
	void					*GetPtr();

	BOOL Create( cGraphics *Graphics, unsigned long NumVertices, DWORD Descriptor, long VertexSize );
	BOOL Free();

	BOOL Set( unsigned long FirstVertex, unsigned long NumVertices, void *VertexList );
	BOOL Render( unsigned long FirstVertex, unsigned long NumPrimitives, DWORD Type );

	BOOL Lock( unsigned long FirstVertex = 0, unsigned long NumVertices = 0 );
	BOOL Unlock();
};

#endif

