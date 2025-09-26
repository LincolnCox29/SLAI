#include <string>
#include "../includes/interpreter.h"
#include "../includes/freader.h"
#include "../includes/token.h"
#include "../includes/lexer.h"
#include <vector>
#include <fstream>
#include <iostream>

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cout << "ERROR: Wrong number of arguments\nExample: C:\\SLAI code.asm" << "\n";
	}
	std::string programText;
	try
	{
		programText = SLAI::FReader().read(argv[1]);
	}
	catch (const std::runtime_error& e)
	{
		std::cerr << "ERROR: " << e.what() << std::endl;
	}
	std::vector<SLAI::Token> tokensStack = SLAI::Lexer(programText).tokenization();
	SLAI::Interpreter(tokensStack).interpret();
}