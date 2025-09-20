#include <string>
#include <algorithm>
#include <iostream>

namespace interpreter
{
	class Program
	{
	private:
		std::string* _lines;
		int _len;
		std::string _programText;
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
			std::cout << "code: " << _programText;
		}
	public:
		Program(std::string program)
		{
			_programText = program;
			deleteEmptyLines();
			_len = std::count(program.begin(), program.end(), '\n') + 1;
			_lines = (std::string*)malloc(sizeof(std::string) * _len);
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
		LabelsTable(std::string program)
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