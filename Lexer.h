#pragma once

#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <vector>
#include "../includes"

namespace SLAI
{
	class Lexer
	{
	private:
		std::string* _lines;
		int _linesLen;
		std::string _programText;
		std::vector<Token> _tokensStack;
	public:
		Lexer()
		{
			
		}
	};
}

#endif // LEXER_H

