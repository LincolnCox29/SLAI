#pragma once

#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <unordered_map>
#include <string>
#include "token.h"
#include <functional>
#include "lexer.h"
#include <memory>

namespace SLAI
{

	class Interpreter
	{
	private:

		std::unordered_map<std::string, variable> _variables;
		std::vector<Token> _tokensStack;
		std::vector<int> _callStack;
		bool zFlag;
		bool sFlag;

		size_t findLabelPosition(const std::string& name);

		inline void initVariablesMap();

		inline void execArithmeticCommand(const std::string& command, variable& target, const variable& value);

		inline bool execJumpCommand(const std::string& command, const std::string& labelName, int& tokenIndex);

	public:
		Interpreter(std::vector<Token> tokensStack);

		void interpret();
	};
}

#endif // INTERPRETER_H