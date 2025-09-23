#pragma once

#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <unordered_map>
#include <string>
#include "token.h"
#include <functional>

namespace SLAI
{

	class Interpreter
	{
	private:
		std::unordered_map<std::string, int> _variables;
		std::string* _lines;
		int _linesLen;
		std::string _programText;
		std::vector<Token> _tokensStack;

		inline void initLines();

		inline void countLines();

		inline void deleteEmptyLines();

		inline void deleteWasteSpaces();

		inline void deleteComments();

		void tokenization();

		inline void initVariablesMap();

		inline void execArithmeticCommand(const std::string& command, int& target, int value);

		inline bool isArithmeticCommand(std::string command);

	public:
		Interpreter(std::string program);

		void interpret();
	};
}

#endif // INTERPRETER_H