/*-----------------------------------------------------------------------------
 * FILENAME:
 *			DebugTracer.cpp
 *
 * DESCRIPTION:
 *			cDebugTracer 클래스
 *
 * 출처:	온라인 게임 네트워크 프로그래밍(신동훈 저) 13 장 보이지 않는 버그 잡기
 *
 * HISTORY:
 *			2004.10.18 - 디버깅 지원을 위해 작성하였다.
 *---------------------------------------------------------------------------*/
#include "StdAfx.h"
#include "DebugTracer.h"

//---------------------------------------------------------------------------//
// DEBUG TRACER 풀력 파라메타
//---------------------------------------------------------------------------//

// call fflush() to force a write of all buffered data for the given output.
#define _FLUSH_MODE_

// postpend a line feed at the end of a message.
#define _AUTO_LINEFEED_


//---------------------------------------------------------------------------//
// TRACER 클래스 정의
//---------------------------------------------------------------------------//

ConsoleTracer::ConsoleTracer()
{
	m_hConsole = INVALID_HANDLE_VALUE;
}

ConsoleTracer::~ConsoleTracer()
{
	// do nothing.
}

BOOL ConsoleTracer::open( LPCSTR out )
{
	if( ::AllocConsole() == FALSE ) return FALSE;

	m_hConsole = ::GetStdHandle( STD_OUTPUT_HANDLE );
	if( m_hConsole == INVALID_HANDLE_VALUE ) return FALSE;

	return TRUE;
}

void ConsoleTracer::close()
{
	if( m_hConsole != INVALID_HANDLE_VALUE )
	{
		::FreeConsole();
		m_hConsole = INVALID_HANDLE_VALUE;
	}
}

void ConsoleTracer::output( LPCSTR string )
{
	DWORD dwWritten;

	::WriteConsole( m_hConsole, string, (int)strlen(string), &dwWritten, NULL);
}

//---------------------------------------------------------------------------//

FileTracer::FileTracer()
{
	m_fp = NULL;
}

FileTracer::~FileTracer()
{
	// do nothing
}

BOOL FileTracer::open( LPCSTR out )
{
	if( ( m_fp = fopen( out, "w+" ) ) == NULL ) return FALSE;
	return TRUE;
}

void FileTracer::close()
{
	if( m_fp )
	{
		fclose( m_fp );
		m_fp = NULL;
	}
}

void FileTracer::output( LPCSTR string )
{
	if( !m_fp ) return;

	fprintf( m_fp, string );

#ifdef _FLUSH_MODE_
	fflush( m_fp );
#endif
}

//---------------------------------------------------------------------------//

TraceTracer::TraceTracer()
{
	// do nothing
}

TraceTracer::~TraceTracer()
{
	// do nothing
}

BOOL TraceTracer::open( LPCSTR out )
{
	// do nothing.
	return TRUE;
}

void TraceTracer::close()
{
	// do nothing.
}

void TraceTracer::output( LPCSTR string )
{
	::OutputDebugString( (LPCSTR)string );
}


//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//

cDebugTracer::cDebugTracer()
{
	m_Tracer = NULL;
}

cDebugTracer::cDebugTracer( DT_TYPE type, LPCSTR Filename )
{
	m_Tracer = NULL;

	create( type, Filename );
}

cDebugTracer::~cDebugTracer()
{
	close();
}

BOOL cDebugTracer::create( DT_TYPE type, LPCSTR Filename )
{
	switch( type )
	{
		case DTTYPE_CONSOLE:	m_Tracer = ( ITracer* ) new ConsoleTracer();	break;
		case DTTYPE_FILE:		m_Tracer = ( ITracer* ) new FileTracer();		break;
		case DTTYPE_TRACE:		m_Tracer = ( ITracer* ) new TraceTracer();		break;
		default:
			return FALSE;
	}

	return m_Tracer->open( Filename );
}

void cDebugTracer::close()
{
	if( m_Tracer )
	{
		m_Tracer->close();
		delete m_Tracer;
		m_Tracer = NULL;
	}
}

void cDebugTracer::output( LPCSTR pwszText )
{
	if( !m_Tracer ) return;

	m_Tracer->output( pwszText );

#ifdef _AUTO_LINEFEED_
	m_Tracer->output( "\n" );
#endif
}

//---------------------------------------------------------------------------//

void cDebugTracer::warning( LPCSTR fmt, ... )
{
	va_list ap;
	char wstrText[512];

	va_start( ap, fmt );
	_vsnprintf( wstrText, sizeof(wstrText), fmt, ap );
	va_end( ap );

	output( wstrText );
}

void cDebugTracer::trace( LPCSTR fmt, ... )
{
	va_list ap;
	char wstrText[512];

	va_start( ap, fmt );
	_vsnprintf( wstrText, sizeof(wstrText), fmt, ap );
	va_end( ap );

	output( wstrText );
}

void cDebugTracer::verbose( LPCSTR fmt, ... )
{
	va_list ap;
	char wstrText[512];

	va_start( ap, fmt );
	_vsnprintf( wstrText, sizeof(wstrText), fmt, ap );
	va_end( ap );

	output( wstrText );
}

//---------------------------------------------------------------------------//

