#pragma once

#include <string>
#include <memory>
#include "Windows.h"
#include "atlbase.h"

namespace winpipes
{

class CPipe
{
	class CPipeEnd
	{
	public:
		CPipeEnd( HANDLE handle, bool shouldClose );
		~CPipeEnd();
		CPipeEnd( CPipeEnd && pipeEnd );
		CPipeEnd( const CPipeEnd & ) = delete;
		CPipeEnd operator=(const CPipeEnd &) = delete;

		HANDLE Get() const;
	private:
		HANDLE m_handle;
		bool m_shouldClose;
	};

public:
	CPipe( CPipeEnd && writeHandle, CPipeEnd && readHandle );

	void Write( std::string data ) const;
	std::string Read() const;

	static std::shared_ptr<CPipe> CreatePipe();
	static std::shared_ptr<CPipe> CreatePipeFromStd();

	HANDLE GetReadEnd() const;
	HANDLE GetWriteEnd() const;

private:
	CPipeEnd m_readEnd;
	CPipeEnd m_writeEnd;

	bool m_shouldClose;
};

using CPipeSPtr = std::shared_ptr<CPipe>;

} // ns winpipes

