#include <string>
#include <algorithm>
#include <iostream>

#define DEBUG

namespace interpreter
{
	class Program
	{
	private:
		std::string* _lines;
		int _linesLen;
		std::string _programText;

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

		}

	public:
		Program(std::string program)
		{
			_programText = program;
			deleteEmptyLines();
#ifdef DEBUG
			std::cout << "input code:\n" << _programText << "\n\n";
#endif // DEBUG
			countLines();
#ifdef DEBUG
			std::cout << "number of lines:" << _linesLen << "\n\n";
#endif // DEBUG
			_lines = new std::string[_linesLen];
			initLines();
#ifdef DEBUG
			std::cout << "lines of code:\n";
			for (int i = 0; i < _linesLen; i++)
			{
				std::cout << _lines[i] << "\n";
			}
#endif // DEBUG

		}
	};

	static class Label
	{
		friend class LabelsTable;
	private:
		std::string _name;
		int _line;
	public:
		Label(std::string name, int line)
		{
			_name = name;
			_line = line;
		}

		Label()
		{
			_name = "";
			_line = -1;
		}

		inline std::string getName() { return _name; };

		inline int getLine() { return _line; };
	};

	class LabelsTable
	{
	private:
		Label table[64];
	public:
		LabelsTable(std::string programText)
		{
		}

		Label& operator[](const std::string name)
		{
			for (Label& label : table)
			{
				if (label.getName() == name)
					return label;
			}
			throw "Label not found";
		}
	};
}

std::string assembler_interpreter(std::string programText)
{
	interpreter::Program program = interpreter::Program(programText);

	return "qwe";
}

int main()
{
	std::string program = R"(
	; My first program
	mov  a, 5
	inc  a
	call function
	msg  '(5+1)/2 = ', a    ; output message
	end

	function:
		div  a, 2
		ret




	zxc:
		div  a, 2
		ret



	mlc:
		div  a, 2
		ret)";
	assembler_interpreter(program);
}