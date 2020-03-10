#pragma once
#ifndef ASTRAL_ERROR_HPP
#define ASTRAL_ERROR_HPP

#include <string>
#include <stdexcept>

namespace astral
{
	class Error
		: public std::runtime_error
	{
	public:
		explicit Error(const std::string& message)
			: std::runtime_error(message)
		{ }

		~Error() noexcept = default;
	};
}

#endif