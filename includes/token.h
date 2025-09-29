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
		Token(const std::string& name);
		const std::string& getName() const { return _name; };
		const enum type& getType() const { return _type; };
	};
}

#endif // TOKEN_H