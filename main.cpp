#include <string>
#include "interpreter.h"

int main()
{
	std::string programText = R"(
	mov a 4 
	inc a
    msg  '4 + 1 = ' a
	mov b 9
    dec b
    msg  '9 - 1 = ' b
    mul a 3
    msg  '5 * 3 = ' a
	add a b
	msg  '15 + 8 = ' a
	sub a 2
    msg  '23 - 2 = ' a
	div b 2
    msg  '8 / 2 = ' b)";
	SLAI::Interpreter program = SLAI::Interpreter(programText);
	program.interpret();
}