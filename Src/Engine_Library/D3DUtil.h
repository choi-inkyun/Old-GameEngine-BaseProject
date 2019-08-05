
#ifndef __D3DUTIL_H__
#define __D3DUTIL_H__

#define SAFE_DELETE( x )	if( x ) { delete    x; x = NULL; }
#define SAFE_DELETEn( x )	if( x ) { delete [] x; x = NULL; }

#define SAFE_RELEASE( x )	if( x ) { x->Release(); x = NULL; }

#ifndef ASSERT
#define ASSERT(exp)			assert(exp)
#endif


#endif

