/*
 * FILENAME:
 *			DataPackage.cpp
 *
 * DESCRIPTION:
 *			데이터를 저장하고 로드를 지원하는 클래스이다.
 *
*/

#include "StdAfx.h"
#include "DataPackage.h"

/*-----------------------------------------------------------------------------
 * cDataPackage CLASS
 *---------------------------------------------------------------------------*/

cDataPackage::cDataPackage()
{
	m_Buffer = NULL;
	m_Size = 0;
}

cDataPackage::~cDataPackage()
{
	Free();
}

void *cDataPackage::Create( unsigned long Size )
{
	// free a previously created buffer
	Free();

	// allocate some memory and return a pointer
	return ( m_Buffer = (void *)new char[ (m_Size = Size) ] );
}

void cDataPackage::Free()
{
	delete m_Buffer;
	m_Buffer = NULL;
	m_Size = 0;
}

BOOL cDataPackage::Save( char *Filename )
{
	FILE *fp;
	
	// make sure there's something to write.
	if( m_Buffer != NULL && m_Size )
	{
		// open file, write size and data
		if( (fp = fopen( Filename, "wb" )) != NULL )
		{
			fwrite( &m_Size, 1, sizeof(m_Size), fp );
			fwrite( m_Buffer, 1, m_Size, fp );
			fclose( fp );
			return TRUE;
		}
	}

	return FALSE;
}

void *cDataPackage::Load( char *Filename, unsigned long *Size )
{
	FILE *fp;

	// free a prior buffer if any
	Free();

	if( (fp = fopen( Filename, "rb" )) != NULL )
	{
		// Read in size and data
		fread( &m_Size, 1, sizeof(m_Size), fp );
		if( (m_Buffer = (void*)new char[m_Size]) != NULL ) fread( m_Buffer, 1, m_Size, fp );
		fclose( fp );

		if( Size != (unsigned long *)NULL ) *Size = m_Size;
		return m_Buffer;
	}

	return NULL;
}

void *cDataPackage::GetPtr()
{
	return m_Buffer;
}

unsigned long cDataPackage::GetSize()
{
	return m_Size;
}

