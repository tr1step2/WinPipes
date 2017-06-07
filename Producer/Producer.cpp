#include <iostream>
#include <fstream>
#include <string>
#include "Windows.h"

#include "Pipe.hpp"
#include "Process.hpp"

namespace
{
	const size_t DefaultBufferLength = 255;
}

void main_impl( int argc, char ** argv )
{
	if ( argc < 2 )
		throw std::runtime_error( "Not enough arguments" );

	std::string filename = argv[1];

	//open file for read
	std::ifstream ifs( filename, std::ofstream::in | std::ofstream::binary );
	if ( !ifs )
		throw std::runtime_error( "Can't open input file for reading" );

	//create pipe and child process with pipe end
	auto pipe = winpipes::CPipe::CreatePipe();
	auto proc = winpipes::CProcess::CreateChildProcess( pipe );

	do
	{
		char buf[DefaultBufferLength] = {};
		ifs.read( buf, DefaultBufferLength );

		std::string data( buf, ifs.gcount() );

		//write data to pipe
		pipe->Write( data );

	} while ( !ifs.eof() );

	pipe->Write( "close_pipe" );
}

int main( int argc, char ** argv )
{
	try
	{
		main_impl(argc, argv);
	}
	catch (const std::exception & e)
	{
		std::cout << "ERROR: " << e.what();
	}
}