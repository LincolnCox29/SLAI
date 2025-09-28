#pragma once

#ifndef VARIABLE_H
#define VARIABLE_H

#include <variant>
#include <optional>
#include <unordered_map>
#include <string>

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

		template<typename T>
		bool is() const { return std::holds_alternative<T>(_value); }
		template<typename T>
		T& getValue() { return std::get<T>(_value); }

		untypedVar& getValue() { return _value; }
	};

	using varTable = std::unordered_map <std::string, Variable>;
}

#endif //VARIABLE_H