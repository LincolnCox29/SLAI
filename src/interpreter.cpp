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
					t += v;
				}
				else if (command == "sub")
				{
					t -= v;
				}
				else if (command == "mul")
				{
					t *= v;
				}
				else if (command == "div")
				{
					if (v == 0)
					{
						throw std::runtime_error("Division by zero");
					}
					t /= v;
				}
			}
			else if constexpr (std::is_pointer_v<TargetType> && std::is_integral_v<ValueType>)
			{
				if (command == "add")
				{
					t += v;
				}
				else if (command == "sub")
				{
					t -= v;
				}
				else
				{
					throw std::runtime_error("Only \"add\" and \"sub\" supported for pointer arithmetic");
				}
			}
			else
			{
				throw std::runtime_error("Unsupported operand types for arithmetic operation");
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
		if (token.getType() != VARIABLE && token.getType() != DEREF)
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

	inline void Interpreter::execAlloc(std::string ptrName, int size, std::string type)
	{
		void* memory;
		if (type == "int")
		{
			memory = malloc(size * sizeof(int));
			_variables.emplace(ptrName, reinterpret_cast<int*>(memory));
		}
		else if (type == "double")
		{
			memory = malloc(size * sizeof(double));
			_variables.emplace(ptrName, reinterpret_cast<double*>(memory));
		}
		else
		{
			throw std::runtime_error("Unsupported type: " + type);
		}
	}

	inline void Interpreter::execFree(Variable& ptrVar)
	{
		if (ptrVar.is<double*>())
		{
			free(ptrVar.getValue<double*>());
			ptrVar.getValue<double*>() = nullptr;
		}
		else if (ptrVar.is<int*>())
		{
			free(ptrVar.getValue<int*>());
			ptrVar.getValue<int*>() = nullptr;
		}
		else
		{
			throw std::runtime_error("Invalid free");
		}
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

				_variables.at(subtoken1.getName()).incdec(cmd, subtoken1.getType());

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
					else if (arg.getType() == DEREF)
					{
						throwUndefinedVariable(arg.getName());
						_variables.at(arg.getName()).printDeref();
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
			if (cmd == "free")
			{
				throwFirstOperandIstVARIABLE(subtoken1);
				throwUndefinedVariable(subtoken1.getName());
				
				execFree(_variables.at(subtoken1.getName()));

				tokenIndex += 2;
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
				continue;
			}
			else if (cmd == "mov")
			{
				if (subtoken1.getType() == VARIABLE &&
					(subtoken2.getType() == VARIABLE || subtoken2.getType() == CONST))
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
						it1->second = it2->second;
					}
					else
					{
						_variables.emplace(var1, it2->second.getValue());
					}
				}
				else if (subtoken1.getType() == DEREF && (subtoken2.getType() == VARIABLE || subtoken2.getType() == CONST))
				{
					const std::string& ptrName = subtoken1.getName();
					const std::string& valueName = subtoken2.getName();

					if (!_variables.count(ptrName))
					{
						throw std::runtime_error("Undefined pointer: " + ptrName);
					}

					Variable& ptrVar = _variables.at(ptrName);
					Variable& valueVar = _variables.at(valueName);

					if (ptrVar.is<int*>())
					{
						if (valueVar.is<int>())
						{
							*ptrVar.getValue<int*>() = valueVar.getValue<int>();
						}
						else
						{
							throw std::runtime_error("Type mismatch: cannot assign non-int to int*");
						}
					}
					else if (ptrVar.is<double*>())
					{
						if (valueVar.is<double>())
						{
							*ptrVar.getValue<double*>() = valueVar.getValue<double>();
						}
						else
						{
							throw std::runtime_error("Type mismatch: cannot assign non-double to double*");
						}
					}
					else
					{
						throw std::runtime_error("Cannot dereference non-pointer variable");
					}
				}
				else if (subtoken1.getType() == VARIABLE && subtoken2.getType() == DEREF)
				{
					const std::string& varName = subtoken1.getName();
					const std::string& ptrName = subtoken2.getName();

					if (!_variables.count(ptrName))
					{
						throw std::runtime_error("Undefined pointer: " + ptrName);
					}

					Variable& ptrVar = _variables.at(ptrName);
					auto it1 = _variables.find(varName);

					if (ptrVar.is<int*>())
					{
						int value = *ptrVar.getValue<int*>();
						if (it1 != _variables.end())
						{
							it1->second = Variable(value);
						}
						else
						{
							_variables.emplace(varName, Variable(value));
						}
					}
					else if (ptrVar.is<double*>())
					{
						double value = *ptrVar.getValue<double*>();
						if (it1 != _variables.end())
						{
							it1->second = Variable(value);
						}
						else
						{
							_variables.emplace(varName, Variable(value));
						}
					}
					else
					{
						throw std::runtime_error("Cannot dereference non-pointer variable");
					}
				}
				else
				{
					throw std::runtime_error("Invalid operand type for mov");
				}
				tokenIndex += 3;
				continue;
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
					zFlag = _variables.at(subtoken1.getName()) == _variables.at(subtoken2.getName());
					sFlag = _variables.at(subtoken1.getName()) < _variables.at(subtoken2.getName());
				}
				else
				{
					throw std::runtime_error("Undefined variable");
				}
				tokenIndex += 3;
				continue;
			}
			Token subtoken3 = _tokensStack[tokenIndex + 3];
			if (cmd == "alloc")
			{
				if (subtoken1.getType() != VARIABLE ||
					subtoken2.getType() != VARIABLE && subtoken2.getType() != CONST ||
					subtoken3.getType() != VARIABLE)
				{
					throw std::runtime_error("Invalid operand type.");
				}
				if (_variables.count(subtoken1.getName()))
				{
					throw std::runtime_error("Allocate memory for already allocated");
				}
				void* memory = nullptr;
				Variable& sizeVar = _variables.at(subtoken2.getName());
				int& size = sizeVar.getValue<int>();
				if (!sizeVar.is<int>() || sizeVar.getValue<int>() < 0)
				{
					throw std::runtime_error("Memory size must be positive INT");
				}
				execAlloc(subtoken1.getName(), size, subtoken3.getName());
				tokenIndex += 4;
			}
		}
	}
}