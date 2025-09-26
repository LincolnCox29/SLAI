#include "../includes/lexer.h"

namespace SLAI 
{
	void Lexer::initLines()
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

	void Lexer::countLines()
	{
		_linesLen = std::count(_programText.begin(), _programText.end(), '\n') + 1;
	}

	void Lexer::deleteEmptyLines()
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

	void Lexer::deleteWasteSpaces()
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

	void Lexer::deleteComments()
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

	std::vector<Token> Lexer::tokenization()
	{
		std::vector<Token> tokensStack;
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
							tokensStack.push_back(Token(line.substr(tokenStart, si - tokenStart + 1)));
						}
						tokenStart = si + 1;
						isStr = false;
					}

				}
				else if (line[si] == ' ' && !isStr)
				{
					if (si > tokenStart)
					{
						tokensStack.push_back(Token(line.substr(tokenStart, si - tokenStart)));
					}
					tokenStart = si + 1;
				}
				si++;
			}
			if (tokenStart < line.length())
			{
				tokensStack.push_back(Token(line.substr(tokenStart)));
			}
		}
		delete[] _lines;
		return tokensStack;
	}
}