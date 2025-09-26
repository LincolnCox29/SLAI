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
		std::string* _lines;
		int _linesLen;
		std::string _programText;

		void initLines();

		void countLines();

		void deleteEmptyLines();

		void deleteWasteSpaces();

		void deleteComments();

	public:

		Lexer(std::string programText)
		{
			_programText = programText;
			deleteComments();
			deleteWasteSpaces();
			deleteEmptyLines();
			countLines();
			_lines = new std::string[_linesLen];
			initLines();
		}

		std::vector<Token> tokenization();
	};
}

#endif // LEXER_H

