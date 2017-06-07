#include <iostream>
#include <fstream>
#include <string>
#include <memory>

#include "Windows.h"

#include "Pipe.hpp"

winpipes::CPipe::CPipeEnd::CPipeEnd( HANDLE handle, bool shouldClose )
	: m_handle( handle )
	, m_shouldClose( shouldClose )
{
}

winpipes::CPipe::CPipeEnd::CPipeEnd( winpipes::CPipe::CPipeEnd && pipeEnd )
{
	m_handle = std::move( pipeEnd.m_handle );
	m_shouldClose = pipeEnd.m_shouldClose;

	pipeEnd.m_shouldClose = false;
	pipeEnd.m_handle = INVALID_HANDLE_VALUE;
}

winpipes::CPipe::CPipeEnd::~CPipeEnd()
{
	if ( m_shouldClose && INVALID_HANDLE_VALUE != m_handle )
		::CloseHandle( m_handle );
}

HANDLE winpipes::CPipe::CPipeEnd::Get() const
{
	if ( INVALID_HANDLE_VALUE == m_handle )
		throw std::runtime_error("Try to access bad pipe end");
	return m_handle;
}



winpipes::CPipe::CPipe( winpipes::CPipe::CPipeEnd && readEnd, winpipes::CPipe::CPipeEnd && writeEnd )
	: m_readEnd(std::move(readEnd))
	, m_writeEnd(std::move(writeEnd))
{
}

void winpipes::CPipe::Write( std::string data ) const
{
	DWORD written = 0;
	BOOL res = ::WriteFile( m_writeEnd.Get(), data.data(), data.length(), &written, NULL );

	if ( !res || written != data.length() )
		throw std::runtime_error("Failed while write data to pipe");

	::FlushFileBuffers( m_writeEnd.Get() );
}

std::string winpipes::CPipe::Read() const
{
	char buf[255] = {};
	DWORD readed = 0;
	BOOL res = ::ReadFile( m_readEnd.Get(), buf, 255, &readed, NULL );

	if ( !res )
		throw std::runtime_error( "Failed while read data from pipe" );

	return std::string( buf, readed );
}

winpipes::CPipeSPtr winpipes::CPipe::CreatePipe()
{
	SECURITY_ATTRIBUTES attr;

	// Set the bInheritHandle flag so pipe handles are inherited. 
	attr.nLength = sizeof( SECURITY_ATTRIBUTES );
	attr.bInheritHandle = TRUE;
	attr.lpSecurityDescriptor = NULL;

	HANDLE readEnd;
	HANDLE writeEnd;

	if ( !::CreatePipe( &readEnd, &writeEnd, &attr, 0 ) )
		throw std::runtime_error( "Can't create pipe" );

	return std::shared_ptr<winpipes::CPipe>( new winpipes::CPipe({ readEnd, true }, { writeEnd, true } ));
}

winpipes::CPipeSPtr winpipes::CPipe::CreatePipeFromStd()
{
	HANDLE readEnd;
	HANDLE writeEnd;

	readEnd = ::GetStdHandle( STD_INPUT_HANDLE );
	writeEnd = ::GetStdHandle( STD_OUTPUT_HANDLE );

	if ( readEnd == INVALID_HANDLE_VALUE || writeEnd == INVALID_HANDLE_VALUE )
		throw std::runtime_error( "Can't create pipe from STD" );

	return std::shared_ptr<winpipes::CPipe>( new winpipes::CPipe( { readEnd, false }, { writeEnd, false } ) );
}

HANDLE winpipes::CPipe::GetReadEnd() const
{
	return m_readEnd.Get();
}

HANDLE winpipes::CPipe::GetWriteEnd() const
{
	return m_writeEnd.Get();
}
