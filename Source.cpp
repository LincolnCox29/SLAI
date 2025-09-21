#include <string>
#include <algorithm>
#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <iomanip>
#include <optional>

#define DEBUG

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

		void spotType()
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

	public:
		Token(std::string name)
		{
			_name = name;
			
			spotType();
		}
		std::string getName() { return _name; };
		enum type getType() { return _type; };
	};

	class Interpreter
	{
	private:
		std::unordered_map<std::string, int> _variables;
		std::string* _lines;
		int _linesLen;
		std::string _programText;
		std::vector<Token> _tokensStack;

		inline void initLines()
		{
			int currentLineIndex = 0;
			int lineIndex = 0;
			for(int i = 0; i < _programText.length(); i++)
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

		inline void countLines()
		{
			_linesLen = std::count(_programText.begin(), _programText.end(), '\n') + 1;
		}

		inline void deleteEmptyLines()
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

		inline void deleteWasteSpaces()
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

		inline void deleteComments()
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

		void tokenization()
		{
			for (int li = 0; li < _linesLen; li++)
			{
				std::string& line = _lines[li];
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

		inline void initVariablesMap()
		{
			for (Token token : _tokensStack)
			{
				if (token.getType() == VARIABLE)
				{
					_variables[token.getName()] = 0;
				}
			}
		}

	public:
		Interpreter(std::string program)
		{
			_programText = program;
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
#ifdef DEBUG
			std::cout << "tokens:\n";
			for (Token token : _tokensStack)
			{
				std::cout << token.getName()<< "   " << token.getType() << "\n";
			}
#endif // DEBUG
			initVariablesMap();
		}

		void interpret()
		{
			std::string cmd;
			int tokenIndex = 0;
			while (cmd != "end")
			{
				Token token = _tokensStack[tokenIndex];
				if (token.getType() != COMMAND)
				{
					throw "The first token in the line must be of type COMMAND";
					break;
				}
				Token subtoken1 = _tokensStack[tokenIndex + 1];
				Token subtoken2 = _tokensStack[tokenIndex + 2];
				if (token.getName() == "mov")
				{
					if (subtoken1.getType() != VARIABLE)
					{
						break;
					}
					if (subtoken2.getType() == VARIABLE)
					{
						_variables[subtoken1.getName()] += _variables[subtoken2.getName()];
					}
					else if (subtoken2.getType() == CONST)
					{
						_variables[subtoken1.getName()] += std::stoi(subtoken2.getName());
					}
					tokenIndex += 3;
				}
				else if (token.getName() == "inc")
				{
					if (subtoken1.getType() != VARIABLE)
					{
						break;
					}
					_variables[subtoken1.getName()]++;
					tokenIndex += 2;
				}
				else if (token.getName() == "msg")
				{
					tokenIndex++;
					int argsIndex = tokenIndex;
					while (_tokensStack[argsIndex].getType() != COMMAND)
					{
						if (_tokensStack[argsIndex].getType() == CONST)
						{
							std::cout << std::stoi(_tokensStack[argsIndex].getName());
						}
						else if (_tokensStack[argsIndex].getType() == VARIABLE)
						{
							std::cout << _variables[_tokensStack[argsIndex].getName()];
						}
						else if (_tokensStack[argsIndex].getType() == STRING)
						{
							std::cout << _tokensStack[argsIndex].getName();
						}
						else
						{
							break;
						}
						argsIndex++;
					}
					tokenIndex = argsIndex;
				}
				else
				{
					break;
				}
			}
		}
	};
}

std::string assembler_interpreter(std::string programText)
{
	SLAI::Interpreter program = SLAI::Interpreter(programText);
	program.interpret();

	return "output";
}

int main()
{
	std::string program = R"(
	; My first program
	mov  a 5
	inc a
	;call function
	msg  '5+1 = ' a    ; output message
	end

	function:
		div  a 2
		ret)";
	assembler_interpreter(program);
}