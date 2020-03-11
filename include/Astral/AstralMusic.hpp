#pragma once
#ifndef ASTRAL_MUSIC_HPP
#define ASTRAL_MUSIC_HPP

#include <algorithm>
#include <string>
#include <string_view>
#include <utility>

#include <SDL2/SDL_mixer.h>

#include "AstralError.hpp"
#include "AstralUtil.hpp"

namespace astral
{
	class Music
	{
	private:
		inline static Mix_Music* s_currentlyPlayingMusic = nullptr;

		Mix_Music* m_musicHandle = nullptr;
		float m_volume = 1.0f;

	public:
		inline static bool IsMusicPlaying() { return Mix_PlayingMusic() == 1; }
		inline static bool IsMusicPaused() { return Mix_PausedMusic() == 1; }

		static void StopAllMusic(const unsigned int fadeOutMilliseconds = 0)
		{
			if (fadeOutMilliseconds == 0)
			{
				Mix_HaltMusic();
			}
			else
			{
				Mix_FadeOutMusic(fadeOutMilliseconds);
			}

			s_currentlyPlayingMusic = nullptr;
		}

		explicit Music(const std::string_view& filepath)
			: m_musicHandle(Mix_LoadMUS(filepath.data()))
		{
			if (m_musicHandle == nullptr)
			{
				using namespace std::literals::string_literals;

				throw Error("Failed to load music file: "s + filepath.data() + "\nError: " + Mix_GetError());
			}
		}

		Music(const Music&) = delete;
		Music& operator =(const Music&) = delete;

		Music(Music&& other) noexcept
			: m_volume(other.m_volume)
		{
			if (m_musicHandle != nullptr)
			{
				Destroy();
			}

			this->m_musicHandle = std::exchange(other.m_musicHandle, nullptr);
		}

		Music& operator =(Music&& other) noexcept
		{
			if (m_musicHandle != nullptr)
			{
				Destroy();
			}

			this->m_musicHandle = std::exchange(other.m_musicHandle, nullptr);
			this->m_volume = std::exchange(other.m_volume, 1.0f);

			return *this;
		}

		~Music() noexcept
		{
			Destroy();
		}

		void Play(const unsigned int playCount = 1, const unsigned int fadeInMilliseconds = 0)
		{
			if (fadeInMilliseconds == 0)
			{
				Mix_VolumeMusic(static_cast<int>(m_volume * MIX_MAX_VOLUME));
				Mix_PlayMusic(m_musicHandle, playCount - 1);
			}
			else
			{
				Mix_FadeInMusic(m_musicHandle, playCount - 1, fadeInMilliseconds);
			}

			s_currentlyPlayingMusic = m_musicHandle;
		}

		void Play(const LoopForeverType, const unsigned int fadeInMilliseconds = 0)
		{
			Play(-1, fadeInMilliseconds);
		}

		void Pause()
		{
			if (IsPlaying())
			{
				Mix_PauseMusic();
			}
		}

		void Resume()
		{
			if (IsPaused())
			{
				Mix_ResumeMusic();
			}
		}

		inline bool IsPlaying() noexcept { return IsMusicPlaying() && s_currentlyPlayingMusic == m_musicHandle; }
		inline bool IsPaused() noexcept { return IsMusicPaused() && s_currentlyPlayingMusic == m_musicHandle; }

		void Stop(const unsigned int fadeOutMilliseconds = 0)
		{
			if (IsPlaying() || IsPaused())
			{
				StopAllMusic(fadeOutMilliseconds);
			}
		}

		void Rewind()
		{
			if (IsPlaying() || IsPaused())
			{
				Mix_RewindMusic();
			}
		}

		inline float GetVolume() const noexcept { return m_volume; }
		inline void SetVolume(const float volume) noexcept { m_volume = std::clamp(volume, 0.0f, 1.0f); }

	private:
		void Destroy() noexcept
		{
			Stop();

			if (m_musicHandle != nullptr)
			{
				Mix_FreeMusic(m_musicHandle);
				m_musicHandle = nullptr;
			}
		}
	};
}

#endif