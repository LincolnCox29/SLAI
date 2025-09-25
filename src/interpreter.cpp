#include "../includes/interpreter.h"
#include <iostream>
#include <unordered_set>
#include <algorithm>
#include <fstream>

namespace SLAI 
{
	void Interpreter::loadProgramFromFile(const std::string& filename)
	{
		std::ifstream file(filename);
		if (!file.is_open()) 
		{
			throw std::runtime_error("Cannot open assembly file: " + filename);
		}

		std::string programText((std::istreambuf_iterator<char>(file)),
			std::istreambuf_iterator<char>());

		if (file.fail() && !file.eof()) 
		{
			throw std::runtime_error("Cannot reading assembly file: " + filename);
		}
		_programText = programText;
	}

	inline void Interpreter::initLines()
	{
		int currentLineIndex = 0;
		int lineIndex = 0;
		for (int i = 0; i < _programText.length(); i++)
		{
			if (_programText[i] == '\n')
			{
				_lines[lineIndex] = _programText.substr(currentLineIndex, i - currentLineIndex);
				lineIndex++;
				currentLineIndex = i + 1;
			}
		}
		_lines[lineIndex] = _programText.substr(currentLineIndex, _programText.length() - 1);
	}

	inline void Interpreter::countLines()
	{
		_linesLen = std::count(_programText.begin(), _programText.end(), '\n') + 1;
	}

	inline void Interpreter::deleteEmptyLines()
	{
		for (int i = 0; i < _programText.length(); i++)
		{
			if (_programText[i] == '\n' && ((i == 0) || _programText[i - 1] == '\n'))
			{
				_programText.erase(i, 1);
				i--;
			}
		}
	}

	inline void Interpreter::deleteWasteSpaces()
	{
		_programText.erase(std::remove(_programText.begin(), _programText.end(), '\t'), _programText.end());
		for (int i = 0; i < _programText.length(); i++)
		{
			if (_programText[i] == ' ' && ((i == 0) || _programText[i + 1] == '\n' || _programText[i + 1] == ' '))
			{
				_programText.erase(i, 1);
				i--;
			}
		}
	}

	inline void Interpreter::deleteComments()
	{
		for (int i = 0; i < _programText.length(); i++)
		{
			if (_programText[i] == ';')
			{
				const auto start_it = _programText.begin() + i;
				const auto newline_it = std::find(start_it, _programText.end(), '\n');

				if (newline_it != _programText.end())
				{
					_programText.erase(start_it, newline_it);
				}
				else
				{
					_programText.erase(start_it, _programText.end());
				}
			}
		}
	}

	void Interpreter::tokenization()
	{
		for (int li = 0; li < _linesLen; li++)
		{
			const std::string& line = _lines[li];
			int tokenStart = 0;
			int si = 0;
			bool isStr = 0;
			while (si < line.length())
			{
				if (line[si] == '\'')
				{
					if (!isStr)
					{
						isStr = true;
						si++;
						continue;
					}
					else
					{
						if (si > tokenStart)
						{
							_tokensStack.push_back(Token(line.substr(tokenStart, si - tokenStart + 1)));
						}
						tokenStart = si + 1;
						isStr = false;
					}

				}
				else if (line[si] == ' ' && !isStr)
				{
					if (si > tokenStart)
					{
						_tokensStack.push_back(Token(line.substr(tokenStart, si - tokenStart)));
					}
					tokenStart = si + 1;
				}
				si++;
			}
			if (tokenStart < line.length())
			{
				_tokensStack.push_back(Token(line.substr(tokenStart)));
			}
		}
	}

	inline void Interpreter::initVariablesMap()
	{
		for (const Token& token : _tokensStack)
		{
			if (token.getType() == VARIABLE || token.getType() == CONST)
			{
				_variables[token.getName()] = token.getType() == CONST ? std::stoi(token.getName()) : 0;
			}
		}
	}

	inline void Interpreter::execArithmeticCommand(const std::string& command, int& target, const int value)
	{
		static std::unordered_map<std::string, std::function<void(int&, int)>> commands =
		{
			{"mov", [](int& t, int v) { t = v;  }},
			{"add", [](int& t, int v) { t += v; }},
			{"sub", [](int& t, int v) { t -= v; }},
			{"mul", [](int& t, int v) { t *= v; }},
			{"div", [](int& t, int v) { if (v != 0) t /= v; }}
		};

		if (commands.find(command) != commands.end())
		{
			commands[command](target, value);
		}
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

	Interpreter::Interpreter(const std::string& filename)
	{
		try
		{
			loadProgramFromFile(filename);
		}
		catch (const std::runtime_error& e)
		{
			std::cerr << "ERROR: " << e.what() << std::endl;
		}
		deleteComments();
		deleteWasteSpaces();
		deleteEmptyLines();
#ifdef DEBUG
		std::cout << "input code:\n" << _programText << "\n\n";
#endif // DEBUG
		countLines();
#ifdef DEBUG
		std::cout << "number of lines: " << _linesLen << "\n\n";
#endif // DEBUG
		_lines = new std::string[_linesLen];
		initLines();
#ifdef DEBUG
		std::cout << "lines of code:\n";
		for (int i = 0; i < _linesLen; i++)
		{
			std::string lineWithVisibleSpaces = std::string(_lines[i]);
			std::replace(lineWithVisibleSpaces.begin(), lineWithVisibleSpaces.end(), ' ', '_');
			std::cout << lineWithVisibleSpaces << "\n";
		}
		std::cout << "\n";
#endif // DEBUG
		tokenization();
		delete[] _lines;
#ifdef DEBUG
		std::cout << "tokens:\n";
		for (Token token : _tokensStack)
		{
			std::cout << token.getName() << "   " << token.getType() << "\n";
		}
#endif // DEBUG
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
					_variables[subtoken1.getName()] += cmd == "inc" ? 1 : -1;
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