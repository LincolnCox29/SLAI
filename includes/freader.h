#pragma once

#ifndef FREADER_H
#define FREADER_H

#include <string>
#include <iostream>
#include <fstream>

namespace SLAI 
{
	class FReader
	{
	private:
		std::string fileName;
		std::string text;
	public:
		std::string read(const std::string& filename)
		{
			std::ifstream file(filename);
			if (!file.is_open())
			{
				throw std::runtime_error("Cannot open assembly file: " + filename);
			}

			std::string text((std::istreambuf_iterator<char>(file)),
				std::istreambuf_iterator<char>());

			if (file.fail() && !file.eof())
			{
				throw std::runtime_error("Cannot reading assembly file: " + filename);
			}
			return text;
		}
	};
}

#endif // FREADER_H