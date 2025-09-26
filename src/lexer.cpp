#include "../includes/lexer.h"
#include <algorithm>

namespace SLAI
{
	void Lexer::preprocessCode()
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
			if (_programText[i] == ' ' && ((i == 0) || _programText[i + 1] == ' '))
			{
				_programText.erase(i, 1);
				i--;
			}
		}
		_programText.erase(std::remove(_programText.begin(), _programText.end(), '\t'), _programText.end());
		std::replace(_programText.begin(), _programText.end(), '\n', ' ');
	}

	std::vector<Token> Lexer::tokenization()
	{
		std::vector<Token> tokensStack;
		int tokenStart = 0;
		int si = 0;
		bool isStr = 0;
		while (si < _programText.length())
		{
			if (_programText[si] == '\'')
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
						tokensStack.push_back(Token(_programText.substr(tokenStart, si - tokenStart + 1)));
					}
					tokenStart = si + 1;
					isStr = false;
				}

			}
			else if (_programText[si] == ' ' && !isStr)
			{
				if (si > tokenStart)
				{
					tokensStack.push_back(Token(_programText.substr(tokenStart, si - tokenStart)));
				}
				tokenStart = si + 1;
			}
			si++;
		}
		if (tokenStart < _programText.length())
		{
			tokensStack.push_back(Token(_programText.substr(tokenStart)));
		}
		return tokensStack;
	}
}