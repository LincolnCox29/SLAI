#pragma once

#include <variant>

#ifndef VARIABLE_H
#define VARIABLE_H
#include <optional>
#include <unordered_map>
#include <string>
#include <ostream>

namespace SLAI
{
	using untypedVar = std::variant<int, double, int*, double*>;

	class Variable
	{
	private:
		untypedVar _value;

	public:
		Variable(int val) : _value(val) {}
		Variable(int* val) : _value(val) {}
		Variable(double val) : _value(val) {}
		Variable(double* val) : _value(val) {}
		Variable(untypedVar val) : _value(val) {}

		template<typename T>
		bool is() const { return std::holds_alternative<T>(_value); }
		template<typename T>
		T& getValue() { return std::get<T>(_value); }
		template<typename T>
		std::remove_pointer_t<T>& getDerefValue() 
		{
			return *std::get<T>(_value);
		}

		void incdec(std::string cmd, type tokenType)
		{
			if (tokenType == VARIABLE)
			{
				if (is<double>())       getValue<double>() += cmd == "inc" ? 1.0 : -1.0;
				else if (is<int>())     getValue<int>() += cmd == "inc" ? 1 : -1;
				else if (is<int*>())    getValue<int*>() += cmd == "inc" ? 1 : -1;
				else if (is<double*>()) getValue<double*>() += cmd == "inc" ? 1 : -1;
			}
			else
			{
				if (is<int*>())         getDerefValue<int*>() += cmd == "inc" ? 1 : -1;
				else if (is<double*>()) getDerefValue<double*>() += cmd == "inc" ? 1 : -1;
			}
		}

		void setPtrValue(Variable derefVar)
		{
			if (is<int*>())
			{
				*getValue<int*>() = derefVar.getValue<int>();
			}
			else if (is<double*>())
			{
				*getValue<double*>() = derefVar.getValue<double>();
			}
		}

		untypedVar& getValue() { return _value; }

		bool sameTypeAs(const Variable& other) const 
		{
			return _value.index() == other._value.index();
		}

		bool operator==(const Variable& var) const
		{
			return _value == var._value;
		}

		bool operator<(const Variable& var) const
		{
			return _value < var._value;
		}

		void operator=(const Variable& var)
		{
			_value = var._value;
		}

		inline void printDeref()
		{
			if (is<int*>())
			{
				std::cout << getDerefValue<int*>();
			}
			else if (is<double*>())
			{
				std::cout << getDerefValue<double*>();
			}
		}
	};

	using varTable = std::unordered_map <std::string, Variable>;

	inline std::ostream& operator<<(std::ostream& os, Variable var)
	{
		if (var.is<int>())
		{
			return os << var.getValue<int>();
		}
		else if (var.is<double>())
		{
			return os << var.getValue<double>();
		}
		else if (var.is<int*>())
		{
			return os << "0x" << std::hex << reinterpret_cast<uintptr_t>(var.getValue<int*>());
		}
		else if (var.is<double*>())
		{
			return os << "0x" << std::hex << reinterpret_cast<uintptr_t>(var.getValue<double*>());
		}
	}
}

#endif //VARIABLE_H