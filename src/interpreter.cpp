#include "../includes/interpreter.h"
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <fstream>
#include "../includes/lexer.h"
#include "../includes/variable.h"

namespace SLAI
{
	inline void Interpreter::initConstVariables()
	{
		for (const Token& token : _tokensStack)
		{
			if (token.getType() == CONST)
			{
				const std::string& value = token.getName();
				if (value.find('.') != std::string::npos ||
					value.find('e') != std::string::npos)
				{
					_variables.emplace(value, Variable(std::stod(value)));
				}
				else
				{
					_variables.emplace(value, Variable(std::stoi(value)));
				}
			}
		}
	}

	inline void Interpreter::execArithmeticCommand(
		const std::string& command,
		Variable& target,
		Variable& value)
	{
		auto visitor = [&](auto&& t, auto&& v)
		{
			using TargetType = std::decay_t<decltype(t)>;
			using ValueType = std::decay_t<decltype(v)>;

			if constexpr (std::is_arithmetic_v<TargetType> && std::is_arithmetic_v<ValueType>) 
			{
				if (command == "add")
				{
					t = t + v;
				}
				else if (command == "sub")
				{
					t = t - v;
				}
				else if (command == "mul")
				{
					t = t * v;
				}
				else if (command == "div")
				{
					if (v == 0)
					{
						throw std::runtime_error("Division by zero");
					}
					t = t / v;
				}
			}
		};
		std::visit(visitor, target.getValue(), value.getValue());
	}

	inline bool Interpreter::execJumpCommand(const std::string& command, const std::string& labelName, int& tokenIndex)
	{
		static std::unordered_map<std::string, std::function<bool(const bool&, const bool&)>> commands =
		{
			{"je",   [](const bool& z, const bool& s) { return  z; }},
			{"jne",  [](const bool& z, const bool& s) { return !z; }},
			{"jg",   [](const bool& z, const bool& s) { return !z && !s; }},
			{"jl",   [](const bool& z, const bool& s) { return s; }},
			{"jle",  [](const bool& z, const bool& s) { return z || s; }},
			{"jge",  [](const bool& z, const bool& s) { return z || !s; }},
			{"jmp",  [](const bool& z, const bool& s) { return true; }},
			{"call", [](const bool& z, const bool& s) { return true; }}
		};

		if ((commands.find(command) != commands.end()) && commands[command](zFlag, sFlag))
		{
			if (command == "call")
			{
				_callStack.push_back(tokenIndex + 2);
			}
			tokenIndex = findLabelPosition(labelName) + 1;
			return true;
		}
		return false;
	}

	static inline void throwFirstOperandIstVARIABLE(const Token& token)
	{
		if (token.getType() != VARIABLE)
		{
			throw std::runtime_error("The first operand must be VARIABLE.");
		}
	}

	inline void Interpreter::throwUndefinedVariable(const std::string& varName)
	{
		if (!_variables.count(varName))
		{
			throw std::runtime_error("Undefined variable");
		}
	}

	size_t Interpreter::findLabelPosition(const std::string& name)
	{
		if (auto it = std::find_if(_tokensStack.begin(), _tokensStack.end(),
			[&name](Token& t) { return t.getName() == name + ':'; }); it != _tokensStack.end())
		{
			return it != _tokensStack.end() ? std::distance(_tokensStack.begin(), it) : -1;
		}
		throw std::runtime_error("Label: \"" + name + "\" not found");
	}

	void Interpreter::interpret()
	{
		initConstVariables();
		std::string cmd;
		int tokenIndex = 0;
		while (cmd != "end")
		{
			if (tokenIndex + 1 > _tokensStack.size())
			{
				throw std::runtime_error("The command \"end\" was not found.");
			}
			Token token = _tokensStack[tokenIndex];
			cmd = token.getName();
			if (cmd == "ret")
			{
				if (_callStack.empty())
				{
					throw std::runtime_error("RET called without corresponding CALL - Stack underflow!");
				}
				tokenIndex = _callStack.back();
				_callStack.pop_back();
				continue;
			}
			if (token.getType() != COMMAND)
			{
				throw std::runtime_error("Invalid number of operands.");
			}
			if (cmd == "end")
			{
				return;
			}
			Token subtoken1 = _tokensStack[tokenIndex + 1];
			if (cmd == "inc" || cmd == "dec")
			{
				throwFirstOperandIstVARIABLE(subtoken1);
				throwUndefinedVariable(subtoken1.getName());
				Variable& var = _variables.at(subtoken1.getName());

				if (var.is<double>())
				{
					var.getValue<double>() += cmd == "inc" ? 1.0 : -1.0;
				}
				else if (var.is<int>())
				{
					var.getValue<int>() += cmd == "inc" ? 1 : -1;
				}
				else if (var.is<uintptr_t>())
				{
					var.getValue<uintptr_t>() += cmd == "inc" ? 1 : -1;
				}
				tokenIndex += 2;
				continue;
			}
			if (token.isJumpToken())
			{
				throwFirstOperandIstVARIABLE(subtoken1);
				if (!execJumpCommand(cmd, subtoken1.getName(), tokenIndex))
				{
					tokenIndex += 2;
				}
				continue;
			}
			if (cmd == "msg")
			{
				tokenIndex++;
				int argsIndex = tokenIndex;
				while (_tokensStack[argsIndex].getType() != COMMAND &&
					_tokensStack[argsIndex].getType() != LABEL)
				{
					const Token& arg = _tokensStack[argsIndex];
					if (arg.getType() == VARIABLE)
					{
						throwUndefinedVariable(arg.getName());
						std::cout << _variables.at(arg.getName());
					}
					else if (arg.getType() == STRING)
					{
						std::cout << arg.getName();
					}
					argsIndex++;
					if (argsIndex == _tokensStack.size())
					{
						std::cout << "\n";
						throw std::runtime_error("The command \"end\" was not found.");
					}
				}
				std::cout << "\n";
				tokenIndex = argsIndex;
				continue;
			}
			Token subtoken2 = _tokensStack[tokenIndex + 2];
			if (token.isArithmeticToken())
			{
				throwFirstOperandIstVARIABLE(subtoken1);
				throwUndefinedVariable(subtoken1.getName());
				if (subtoken2.getType() == VARIABLE || subtoken2.getType() == CONST)
				{
					throwUndefinedVariable(subtoken2.getName());
					execArithmeticCommand(cmd, _variables.at(subtoken1.getName()), _variables.at(subtoken2.getName()));
				}
				else
				{
					throw std::runtime_error("Invalid operand type.");
				}
				tokenIndex += 3;
			}
			else if (cmd == "mov")
			{
				if (subtoken2.getType() == VARIABLE || subtoken2.getType() == CONST)
				{
					const std::string& var1 = subtoken1.getName();
					const std::string& var2 = subtoken2.getName();

					auto it1 = _variables.find(var1);
					auto it2 = _variables.find(var2);

					if (it2 == _variables.end())
					{
						throw std::runtime_error("Undefined variable");
					}

					if (it1 != _variables.end())
					{
						it1->second.getValue() = it2->second.getValue();
					}
					else
					{
						_variables.emplace(var1, it2->second.getValue());
					}
				}
				else
				{
					throw std::runtime_error("Invalid operand type.");
				}
				tokenIndex += 3;
			}
			else if (cmd == "cmp")
			{
				if (subtoken1.getType() != CONST && subtoken1.getType() != VARIABLE ||
					subtoken2.getType() != CONST && subtoken2.getType() != VARIABLE)
				{
					throw std::runtime_error("CMP operands must be VARIABLE or CONST");
				}
				if (_variables.count(subtoken1.getName()) &&
					_variables.count(subtoken2.getName()))
				{
					zFlag = _variables.at(subtoken1.getName()).getValue() == _variables.at(subtoken2.getName()).getValue();
					sFlag = _variables.at(subtoken1.getName()).getValue() < _variables.at(subtoken2.getName()).getValue();
				}
				else
				{
					throw std::runtime_error("Undefined variable");
				}
				tokenIndex += 3;
			}
		}
	}
}