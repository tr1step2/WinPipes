#pragma once

#include <memory>
#include "Windows.h"
#include "atlbase.h"

#include "Pipe.hpp"

namespace winpipes
{

class CProcess
{
public:
	CProcess(HANDLE processHandle, HANDLE threadHandle);
	CProcess( const CProcess & ) = delete;

	static std::shared_ptr<CProcess> CreateChildProcess( winpipes::CPipeSPtr pipe );

private:
	CHandle m_processHandle;
	CHandle m_threadHandle;
};

using CProcessSPtr = std::shared_ptr<CProcess>;

} //ns winpipes