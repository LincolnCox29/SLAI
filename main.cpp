#include <string>
#include "interpreter.h"
#include <fstream>
#include <iostream>

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cout << "ERROR: Wrong number of arguments\nExample: C:\\\\SLAI code.asm" << "\n";
	}
	SLAI::Interpreter interpreter = std::string(argv[1]);
	interpreter.interpret();
}