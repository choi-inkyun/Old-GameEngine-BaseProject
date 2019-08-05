
#ifndef __DEBUGTRACER_H__
#define __DEBUGTRACER_H__

class ITracer
{
public:
	ITracer() {}
	virtual ~ITracer() {}

	virtual BOOL open( LPCSTR out ) = 0;
	virtual void close() = 0;
	virtual void output( LPCSTR string ) = 0;
};

class ConsoleTracer : public ITracer
{
public:
	ConsoleTracer();
	virtual ~ConsoleTracer();

	virtual BOOL open( LPCSTR out );
	virtual void close();
	virtual void output( LPCSTR string );

private:
	HANDLE	m_hConsole;
};

class FileTracer : public ITracer
{
public:
	FileTracer();
	virtual ~FileTracer();

	virtual BOOL open( LPCSTR out );
	virtual void close();
	virtual void output( LPCSTR string );

private:
	FILE* m_fp;
};

class TraceTracer : public ITracer
{
public:
	TraceTracer();
	virtual ~TraceTracer();

	virtual BOOL open( LPCSTR out );
	virtual void close();
	virtual void output( LPCSTR string );
};


class cDebugTracer
{
private:
	ITracer*	m_Tracer;

public:
	enum DT_TYPE
	{
		DTTYPE_CONSOLE,						// 콘솔 출력 타입
		DTTYPE_FILE,						// 파일 출력 타입
		DTTYPE_TRACE						// MS Visual Studio Debug 출력 타입
	};

public:
	cDebugTracer();
	cDebugTracer( DT_TYPE type, LPCSTR Filename = NULL );
	virtual ~cDebugTracer();

	BOOL create( DT_TYPE type, LPCSTR Filename = NULL );
	void close();

	void output( LPCSTR pwszText );

	void warning( LPCSTR fmt, ... );
	void trace( LPCSTR fmt, ... );
	void verbose( LPCSTR fmt, ... );
};

#endif

