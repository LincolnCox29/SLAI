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
	using untypedVar = std::variant<int, double, uintptr_t>;

	class Variable
	{
	private:
		untypedVar _value;

	public:
		Variable(int val) : _value(val) {}
		Variable(double val) : _value(val) {}
		Variable(uintptr_t val) : _value(val) {}
		Variable(untypedVar val) : _value(val) {}

		template<typename T>
		bool is() const { return std::holds_alternative<T>(_value); }
		template<typename T>
		T& getValue() { return std::get<T>(_value); }

		untypedVar& getValue() { return _value; }

		bool sameTypeAs(const Variable& other) const 
		{
			return _value.index() == other._value.index();
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
		else if (var.is<uintptr_t>())
		{
			return os << "0x" << std::hex << var.getValue<uintptr_t>();
		}
	}
}

#endif //VARIABLE_H