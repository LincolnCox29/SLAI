#pragma once

#ifndef TOKEN_H
#define TOKEN_H

#include <vector>
#include <string>
#include <unordered_map>

namespace SLAI 
{
	enum type
	{
		CONST,
		VARIABLE,
		COMMAND,
		LABEL,
		STRING
	};

	class Token
	{
	private:
		std::string _name;
		type _type;
		std::vector<int> callStack;

		void spotType();

	public:
		bool isJumpToken();
		bool isArithmeticToken();
		Token(std::string name);
		std::string getName() { return _name; };
		enum type getType() { return _type; };
		void print(std::unordered_map<std::string, int>& variables);
	};
}

#endif // TOKEN_H