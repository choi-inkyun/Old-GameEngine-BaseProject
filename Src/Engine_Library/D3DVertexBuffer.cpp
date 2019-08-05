
#include "StdAfx.h"
#include "D3DVertexBuffer.h"

/*-----------------------------------------------------------------------------
 * cVertexBuffer CLASS
 *---------------------------------------------------------------------------*/

cVertexBuffer::cVertexBuffer()
{
	m_Graphics = NULL;
	m_pVB = NULL;
	m_NumVertices = 0;
	m_FVF = 0;
	m_Locked = FALSE;
	m_Ptr = NULL;
}

cVertexBuffer::~cVertexBuffer()
{
	Free();
}


BOOL cVertexBuffer::Create( cGraphics *Graphics, unsigned long NumVertices, DWORD Descriptor, long VertexSize )
{
	Free();

	if( ( m_Graphics = Graphics ) == NULL ) return FALSE;
	if( m_Graphics->GetDeviceCOM() == NULL ) return FALSE;
	if( !(m_NumVertices = NumVertices ) || !(m_FVF = Descriptor) || !(m_VertexSize = VertexSize) ) return FALSE;

	if( FAILED( m_Graphics->GetDeviceCOM()->CreateVertexBuffer(
				m_NumVertices * m_VertexSize,
				0, m_FVF,
				D3DPOOL_DEFAULT, &m_pVB, NULL ) ) ) return FALSE;

	return TRUE;
}

BOOL cVertexBuffer::Free()
{
	Unlock();
	ReleaseCOM( m_pVB );
	m_Graphics = NULL;
	m_NumVertices = 0;
	m_FVF = 0;
	m_Locked = FALSE;
	m_Ptr = NULL;

	return TRUE;
}

BOOL cVertexBuffer::Set( unsigned long FirstVertex, unsigned long NumVertices, void *VertexList )
{
	if( m_Graphics == NULL || VertexList == NULL || m_pVB == NULL ) return FALSE;
	if( m_Graphics->GetDeviceCOM() == NULL ) return FALSE;

	// Lock the vertex buffer
	if( Lock( FirstVertex, NumVertices ) == FALSE ) return FALSE;

	// Copy vertices to vertex buffer
	memcpy( m_Ptr, VertexList, NumVertices * m_VertexSize );

	// Unlock vertex buffer
	if( Unlock() == FALSE ) return FALSE;

	return TRUE;
}

BOOL cVertexBuffer::Render( unsigned long FirstVertex, unsigned long NumPrimitives, DWORD Type )
{
	if( m_Graphics->GetDeviceCOM() == NULL || m_pVB == NULL ) return FALSE;

	m_Graphics->GetDeviceCOM()->SetStreamSource( 0, m_pVB, 0, m_VertexSize );
	m_Graphics->GetDeviceCOM()->SetFVF( m_FVF );
	m_Graphics->GetDeviceCOM()->DrawPrimitive( (D3DPRIMITIVETYPE)Type, FirstVertex, NumPrimitives );

	return TRUE;
}

BOOL cVertexBuffer::Lock( unsigned long FirstVertex, unsigned long NumVertices )
{
	if( m_pVB == NULL ) return FALSE;

	if( FAILED( m_pVB->Lock( FirstVertex * m_VertexSize, NumVertices * m_VertexSize,
					(VOID**)&m_Ptr, 0 /* D3DLOCK_NOSYSLOCK | D3DLOCK_DISCARD */ ) ) ) return FALSE;
	m_Locked = TRUE;

	return TRUE;
}

BOOL cVertexBuffer::Unlock()
{
	if( m_pVB == NULL ) return FALSE;
	if( FAILED( m_pVB->Unlock() ) ) return FALSE;
	m_Locked = FALSE;
	return TRUE;
}


IDirect3DVertexBuffer9 *cVertexBuffer::GetVertexBufferCOM() { return m_pVB; }
unsigned long cVertexBuffer::GetVertexSize() { return D3DXGetFVFVertexSize( m_FVF ); }
unsigned long cVertexBuffer::GetVertexFVF() { return m_FVF; }
unsigned long cVertexBuffer::GetNumVertices() { return m_NumVertices; }
BOOL cVertexBuffer::IsLoaded() { return ( m_pVB == NULL ) ? FALSE : TRUE; }
void *cVertexBuffer::GetPtr() { return (void *)m_Ptr; }

