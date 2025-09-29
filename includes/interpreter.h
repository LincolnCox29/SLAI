#pragma once

#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <unordered_map>
#include <string>
#include "token.h"
#include <functional>
#include "lexer.h"
#include <memory>
#include "../includes/variable.h"

namespace SLAI
{

	class Interpreter
	{
	private:

		varTable _variables;
		std::vector<Token> _tokensStack;
		std::vector<int> _callStack;
		bool zFlag;
		bool sFlag;

		size_t findLabelPosition(const std::string& name);

		inline void execArithmeticCommand(const std::string& command, Variable& target, Variable& value);

		inline bool execJumpCommand(const std::string& command, const std::string& labelName, int& tokenIndex);

		inline void throwUndefinedVariable(const std::string& varName);

		inline void initConstVariables();

	public:
		Interpreter(std::vector<Token> tokensStack) 
		{
			_tokensStack = tokensStack;
			zFlag = false;
			sFlag = false;
		}

		void interpret();
	};
}

#endif // INTERPRETER_H