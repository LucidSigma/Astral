#pragma once
#ifndef ASTRAL_INIT_HPP
#define ASTRAL_INIT_HPP

#include <string>
#include <type_traits>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "AstralError.hpp"
#include "AstralSound.hpp"
#include "AstralUtil.hpp"

namespace astral
{
	enum class RequiredFileTypes
		: std::underlying_type_t<MIX_InitFlags>
	{
		FLAC = MIX_INIT_FLAC,
		MOD = MIX_INIT_MOD,
		MP3 = MIX_INIT_MP3,
		OGG = MIX_INIT_OGG,
		MIDI = MIX_INIT_MID,
		OPUS = MIX_INIT_OPUS
	};

	struct InitialiseData
	{
		std::initializer_list<RequiredFileTypes> requiredFileTypes;

		struct FormatInfo
		{
			bool isSigned = true;
			unsigned int bitCount = 16;
			Endianness endianness = Endianness::System;
		} formatInfo;

		unsigned int frequency = 44'100;
		unsigned int chunkSize = 4'096;

		unsigned int allocatedChannels = 16;
		float maxDistance = 100.0f;
		bool reverseStereo = false;
	};

	namespace
	{
		Uint16 GetFormat(const InitialiseData& initialiseData)
		{
			if (initialiseData.formatInfo.isSigned)
			{
				if (initialiseData.formatInfo.bitCount == 8)
				{
					return AUDIO_S8;
				}
				else if (initialiseData.formatInfo.bitCount == 16)
				{
					switch (initialiseData.formatInfo.endianness)
					{
					case Endianness::Little:
						return AUDIO_S16LSB;

					case Endianness::Big:
						return AUDIO_S16MSB;

					case Endianness::System:
					default:
						return AUDIO_S16SYS;
					}
				}
				else if (initialiseData.formatInfo.bitCount == 32)
				{
					switch (initialiseData.formatInfo.endianness)
					{
					case Endianness::Little:
						return AUDIO_S32LSB;

					case Endianness::Big:
						return AUDIO_S32MSB;

					case Endianness::System:
					default:
						return AUDIO_S32SYS;
					}
				}
				else
				{
					throw Error("Invalid format bit count.");
				}
			}
			else
			{
				if (initialiseData.formatInfo.bitCount == 8)
				{
					return AUDIO_U8;
				}
				else if (initialiseData.formatInfo.bitCount == 16)
				{
					switch (initialiseData.formatInfo.endianness)
					{
					case Endianness::Little:
						return AUDIO_U16LSB;

					case Endianness::Big:
						return AUDIO_U16MSB;

					case Endianness::System:
					default:
						return AUDIO_U16SYS;
					}
				}
				else
				{
					throw Error("Invalid format bit count.");
				}
			}
		}
	}
	
	void Initialise(const InitialiseData& initialiseData)
	{
		if (SDL_WasInit(SDL_INIT_AUDIO) == 0)
		{
			if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0)
			{
				using namespace std::literals::string_literals;

				throw Error("Failed to initialise SDL's audio subsystem.\nError: "s + SDL_GetError());
			}
		}

		int flags = 0;

		for (const auto requiredFileType : initialiseData.requiredFileTypes)
		{
			flags |= static_cast<std::underlying_type_t<MIX_InitFlags>>(requiredFileType);
		}

		if (Mix_Init(flags) != flags)
		{
			using namespace std::literals::string_literals;

			throw Error("Failed to initialise SDL_Mixer.\nError: "s + Mix_GetError());
		}

		if (Mix_OpenAudio(initialiseData.frequency, GetFormat(initialiseData), MIX_DEFAULT_CHANNELS, initialiseData.chunkSize) == -1)
		{
			using namespace std::literals::string_literals;

			throw Error("Failed to open audio: "s + Mix_GetError());
		}

		Mix_AllocateChannels(initialiseData.allocatedChannels);
		Sound::SetMaxDistance(initialiseData.maxDistance);

		if (initialiseData.reverseStereo)
		{
			ReverseStereo(true);
		}
	}

	inline void Initialise()
	{
		Initialise({ });
	}

	void Quit() noexcept
	{
		Mix_CloseAudio();
		Mix_Quit();
	}
}

#endif