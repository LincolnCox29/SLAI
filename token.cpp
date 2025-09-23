#include "token.h"
#include <unordered_set>
#include <unordered_map>
#include <iomanip>

#include <iostream>

namespace SLAI 
{
	bool Token::isArithmeticToken()
	{
		static const std::unordered_set<std::string> validCommands =
		{
			"mov", "add", "sub", "div", "mul"
		};
		return validCommands.find(_name) != validCommands.end();
	}

	bool Token::isJumpToken()
	{
		static const std::unordered_set<std::string> validCommands =
		{
			"jmp", "jne", "je", "jge", "jg", "jle", "jl", "call"
		};
		return validCommands.find(_name) != validCommands.end();
	}

	void Token::spotType()
	{
		static const std::unordered_set<std::string> commands = {
			"mov", "inc", "dec", "add", "sub", "mul", "div",
			"jmp", "cmp", "je", "jne", "jg", "jge", "jl", "jle",
			"call", "ret", "msg", "end"
		};
		try
		{
			std::stoi(_name);
			_type = CONST;
			return;
		}
		catch (const std::invalid_argument& e) {}
		if (_name[0] == '\'' && _name.back() == '\'')
		{
			_type = STRING;
			_name = _name.substr(1, _name.length() - 2);
		}
		else if (commands.find(_name) != commands.end())
		{
			_type = COMMAND;
		}
		else if (_name.back() == ':')
		{
			_type = LABEL;
		}
		else
		{
			_type = VARIABLE;
		}
	}

	Token::Token(std::string name)
	{
		_name = name;

		spotType();
	}

	void Token::print(std::unordered_map<std::string, int>& variables)
	{
		if (_type == CONST || _type == VARIABLE)
		{
			std::cout << variables[_name];
		}
		else if(_type == STRING)
		{
			std::cout << _name;
		}
	}
}