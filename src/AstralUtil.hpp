#pragma once
#ifndef ASTRAL_UTIL_HPP
#define ASTRAL_UTIL_HPP

#include <SDL2/SDL_mixer.h>

namespace astral
{
	enum class Endianness
	{
		Little,
		Big,
		System
	};

	struct VersionData
	{
		unsigned int major;
		unsigned int minor;
		unsigned int patch;
	};

	constexpr VersionData Version{ 1, 0, 0 };

	struct LoopForeverType { };
	constexpr LoopForeverType LoopForever{ };

	void ReverseStereo(const bool reverseStereo)
	{
		Mix_SetReverseStereo(MIX_CHANNEL_POST, reverseStereo ? SDL_TRUE : SDL_FALSE);
	}
}

#endif