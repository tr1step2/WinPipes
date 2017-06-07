#include <iostream>
#include <fstream>
#include <string>
#include "Windows.h"

#include "Pipe.hpp"
#include "Process.hpp"

namespace 
{
	const std::string DefaultOutputFilename = "__pipe_out.txt";
}

void main_impl( int argc, char ** argv )
{
	std::string data;

	std::ofstream ofs( DefaultOutputFilename, std::ofstream::app | std::ofstream::binary );
	if ( !ofs )
		throw std::runtime_error("Can't open output file for write");

	do
	{
		std::cin >> data;
		ofs.write( data.data(), data.length() );

	} while ( data != "close_pipe" );
}

int main( int argc, char ** argv)
{
	try
	{
		main_impl( argc, argv );
	}
	catch ( const std::exception & e )
	{
		std::cout << "ERROR: " << e.what() << std::endl;
	}
}