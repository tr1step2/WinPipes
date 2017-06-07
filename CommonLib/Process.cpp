#include <iostream>
#include <fstream>
#include <string>
#include <memory>

#include "Windows.h"

#include "Process.hpp"
#include "Pipe.hpp"

namespace
{
	const DWORD DefaultWaitProcTimeout = 250; // ms
}

winpipes::CProcess::CProcess( HANDLE processHandle, HANDLE threadHandle )
{
	m_processHandle.Attach( processHandle );
	m_threadHandle.Attach( threadHandle );
}

winpipes::CProcess::~CProcess()
{
	::WaitForSingleObject(m_processHandle, DefaultWaitProcTimeout);
	::TerminateProcess(m_processHandle, NO_ERROR);
}

winpipes::CProcessSPtr winpipes::CProcess::CreateChildProcess( winpipes::CPipeSPtr pipe )
{
	PROCESS_INFORMATION procInfo;
	STARTUPINFO startupInfo;
	BOOL res = FALSE;

	ZeroMemory( &procInfo, sizeof( PROCESS_INFORMATION ) );
	ZeroMemory( &startupInfo, sizeof( STARTUPINFO ) );

	//set startup info
	startupInfo.cb = sizeof( STARTUPINFO );
	startupInfo.hStdInput = pipe->GetReadEnd();
	startupInfo.dwFlags |= STARTF_USESTDHANDLES;

	TCHAR cmdline[] = TEXT( "consumer" );

	if ( !::CreateProcess( NULL, cmdline, NULL, NULL, TRUE, 0, NULL, NULL, &startupInfo, &procInfo ) )
		throw std::runtime_error( "Can't create child process" );

	return std::make_shared<winpipes::CProcess>( procInfo.hProcess, procInfo.hThread );
}
