#pragma once

#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <vector>
#include "token.h"

namespace SLAI
{
	class Lexer
	{
	private:
		int _linesLen;
		std::string _programText;

		void preprocessCode();

	public:

		Lexer(std::string programText)
		{
			_programText = programText;
			preprocessCode();
		}

		std::vector<Token> tokenization();
	};
}

#endif // LEXER_H

