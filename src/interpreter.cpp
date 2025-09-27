#include "../includes/interpreter.h"
#include <iostream>
#include <unordered_set>
#include <memory>
#include <algorithm>
#include <fstream>
#include "../includes/lexer.h"

namespace SLAI 
{
	inline void Interpreter::initVariablesMap()
	{
		for (const Token& token : _tokensStack)
		{
			if (token.getType() == CONST)
			{
				const std::string& value = token.getName();
				if (value.find('.') != std::string::npos ||
					value.find('e') != std::string::npos)
				{
					_variables[value] = std::stod(value);
				}
				else
				{
					_variables[value] = std::stoi(value);
				}
			}
			else 
			{
				_variables[token.getName()] = 0;
			}
		}
	}

	inline void Interpreter::execArithmeticCommand(
		const std::string& command,
		std::variant<int, double>& target,
		const std::variant<int, double>& value)
	{
		auto visitor = [&](auto&& target_val, auto&& value_val) 
		{
			using TargetType = std::decay_t<decltype(target_val)>;
			using ValueType = std::decay_t<decltype(value_val)>;

			if constexpr (std::is_arithmetic_v<TargetType> && std::is_arithmetic_v<ValueType>) {
				if (command == "mov")
				{
					target = value_val;
				}
				else if (command == "add")
				{
					target = target_val + value_val;
				}
				else if (command == "sub")
				{
					target = target_val - value_val;
				}
				else if (command == "mul")
				{
					target = target_val * value_val;
				}
				else if (command == "div")
				{
					if (value_val == 0)
					{
						throw std::runtime_error("Division by zero");
					}
					target = target_val / value_val;
				}
			}
			else
			{
				throw std::runtime_error("Invalid types for arithmetic operation");
			}
		};
		std::visit(visitor, target, value);
	}

	inline bool Interpreter::execJumpCommand(const std::string& command, const std::string& labelName, int& tokenIndex)
	{
		static std::unordered_map<std::string, std::function<bool(const bool& , const bool&)>> commands =
		{
			{"je",   [](const bool& z, const bool& s) { return  z; }},
			{"jne",  [](const bool& z, const bool& s) { return !z; }},
			{"jg",   [](const bool& z, const bool& s) { return !z && !s; }},
			{"jl",   [](const bool& z, const bool& s) { return s; }},
			{"jle",  [](const bool& z, const bool& s) { return z ||  s; }},
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

	Interpreter::Interpreter(std::vector<Token> tokensStack)
	{
		_tokensStack = tokensStack;
		initVariablesMap();
	}

	static inline void throwFirstOperandIstVARIABLE(const Token& token)
	{

		if (token.getType() != VARIABLE)
		{
			throw std::runtime_error("The first operand must be VARIABLE.");
		}
	}

	size_t Interpreter::findLabelPosition(const std::string& name)
	{
		if (auto it = std::find_if(_tokensStack.begin(), _tokensStack.end(),
			[&name](Token& t) { return t.getName() == name + ':'; }); it != _tokensStack.end())
		{
			return it != _tokensStack.end() ? std::distance(_tokensStack.begin(), it) : -1;
		}
	}

	void Interpreter::interpret()
	{
		std::string cmd;
		int tokenIndex = 0;
		try
		{
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
					if (std::holds_alternative<double>(_variables[subtoken1.getName()]))
					{
						std::get<double>(_variables[subtoken1.getName()]) += cmd == "inc" ? 1.0 : -1.0;
					}
					else
					{
						std::get<int>(_variables[subtoken1.getName()]) += cmd == "inc" ? 1 : -1;
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
					while (_tokensStack[argsIndex].getType() != COMMAND)
					{
						_tokensStack[argsIndex].print(_variables);
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
					if (subtoken2.getType() == VARIABLE || subtoken2.getType() == CONST)
					{
						execArithmeticCommand(cmd, _variables[subtoken1.getName()], _variables[subtoken2.getName()]);
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
					zFlag = _variables[subtoken1.getName()] == _variables[subtoken2.getName()];
					sFlag = _variables[subtoken1.getName()] < _variables[subtoken2.getName()];
					tokenIndex += 3;
				}
			}
		}
		catch (const std::runtime_error& e)
		{
			std::cerr << "ERROR: " << e.what() << std::endl;
		}
	}
}