#pragma once

#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <unordered_map>
#include <string>
#include "token.h"
#include <functional>
#include "lexer.h"
#include <memory>
#include <variant>

namespace SLAI
{

	class Interpreter
	{
	private:

		std::unordered_map<std::string, std::variant<int, double>> _variables;
		std::vector<Token> _tokensStack;
		std::vector<int> _callStack;
		bool zFlag;
		bool sFlag;

		size_t findLabelPosition(const std::string& name);

		inline void initVariablesMap();

		inline void execArithmeticCommand(const std::string& command, std::variant<int, double>& target, const std::variant<int, double>& value);

		inline bool execJumpCommand(const std::string& command, const std::string& labelName, int& tokenIndex);

	public:
		Interpreter(std::vector<Token> tokensStack);

		void interpret();
	};
}

#endif // INTERPRETER_H