#include <string>
#include "interpreter.h"

int main()
{
	std::string program1 = R"(
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
    msg  '8 / 2 = ' b
	end)";
	SLAI::Interpreter i1 = SLAI::Interpreter(program1);
	i1.interpret();

	std::string program2 = R"(
	mov a 5
	jmp lbl
	inc a
	inc a
	inc a
	end

	lbl:
		mul a 100
		msg a
		end)";
	SLAI::Interpreter i2= SLAI::Interpreter(program2);
	i2.interpret();
}