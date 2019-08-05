
#ifndef __ENGINE_DATAPACKAGE_H__
#define __ENGINE_DATAPACKAGE_H__

/*-----------------------------------------------------------------------------
 * cDataPackage CLASS
 *---------------------------------------------------------------------------*/

class cDataPackage
{
protected:
	void			*m_Buffer;
	unsigned long	 m_Size;

public:
	cDataPackage();
	~cDataPackage();

	void *Create( unsigned long Size );
	void Free();

	BOOL Save( char *Filename );
	void *Load( char *Filename, unsigned long *Size = NULL );

	void			*GetPtr();
	unsigned long	 GetSize();
};

#endif

