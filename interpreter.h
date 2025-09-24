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
		std::vector<int> _callStack;
		bool zFlag;
		bool sFlag;

		inline void initLines();

		inline void countLines();

		inline void deleteEmptyLines();

		inline void deleteWasteSpaces();

		inline void deleteComments();

		void tokenization();

		size_t findLabelPosition(const std::string& name);

		inline void initVariablesMap();

		inline void execArithmeticCommand(const std::string& command, int& target, const int value);

		inline bool execJumpCommand(const std::string& command, const std::string& labelName, int& tokenIndex);

	public:
		Interpreter(const std::string& program);

		void interpret();
	};
}

#endif // INTERPRETER_H