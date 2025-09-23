#include "token.h"
#include <unordered_set>
#include <unordered_map>
#include <iomanip>

#include <iostream>

namespace SLAI 
{
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
		switch (_type)
		{
		case CONST:
			std::cout << std::stoi(_name);
			break;
		case VARIABLE:
			std::cout << variables[_name];
			break;
		case STRING:
			std::cout << _name;
			break;
		default:
			break;
		}
	}
}