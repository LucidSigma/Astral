#pragma once
#ifndef ASTRAL_SOUND_HPP
#define ASTRAL_SOUND_HPP

#include <algorithm>
#include <string>
#include <string_view>
#include <utility>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "AstralError.hpp"
#include "AstralListener.hpp"
#include "AstralMath.hpp"
#include "AstralUtil.hpp"

namespace astral
{
	class Sound
	{
	private:
		static constexpr int s_InvalidChannel = -1;

		inline static float s_maxDistance = 100.0f;

		Mix_Chunk* m_chunkHandle = nullptr;
		mutable int m_channel = -1;

		math::Vec3 m_position{ 0.0f, 0.0f, 0.0f };
		float m_volume = 1.0f;

	public:
		inline static float GetMaxDistance() noexcept { return s_maxDistance; }
		inline static void SetMaxDistance(const float maxDistance) noexcept { s_maxDistance = maxDistance; }

		explicit Sound(const std::string_view& filepath)
			: m_chunkHandle(Mix_LoadWAV(filepath.data()))
		{
			if (m_chunkHandle == nullptr)
			{
				using namespace std::literals::string_literals;

				throw Error("Failed to load sound file: "s + filepath.data() + "\nError: " + Mix_GetError());
			}
		}

		Sound(const Sound&) = delete;
		Sound& operator =(const Sound&) = delete;

		Sound(Sound&& other) noexcept
			: m_channel(other.m_channel), m_position(other.m_position), m_volume(other.m_volume)
		{
			if (m_chunkHandle != nullptr)
			{
				Destroy();
			}

			this->m_chunkHandle = std::exchange(other.m_chunkHandle, nullptr);
		}

		Sound& operator =(Sound&& other) noexcept
		{
			if (m_chunkHandle != nullptr)
			{
				Destroy();
			}

			this->m_chunkHandle = std::exchange(other.m_chunkHandle, nullptr);
			this->m_channel = std::exchange(other.m_channel, s_InvalidChannel);

			this->m_position = std::exchange(other.m_position, math::Vec3{ 0.0f, 0.0f, 0.0f });
			this->m_volume = std::exchange(other.m_volume, 1.0f);

			return *this;
		}

		~Sound() noexcept
		{
			Destroy();
		}

		void Play(const unsigned int times = 1, const unsigned int fadeInMilliseconds = 0, const unsigned int fadeOutMilliseconds = 0) const
		{
			m_channel = fadeInMilliseconds == 0 ? Mix_PlayChannel(-1, m_chunkHandle, times - 1) : Mix_FadeInChannel(-1, m_chunkHandle, times - 1, fadeInMilliseconds);
			InitiateEffects(times - 1, fadeOutMilliseconds);
		}

		void Play(const LoopForeverType, const unsigned int fadeInMilliseconds = 0, const unsigned int fadeOutMilliseconds = 0) const
		{
			Play(-1, fadeInMilliseconds, fadeOutMilliseconds);
		}

		inline void Pause() const { Mix_Pause(m_channel); }
		inline void Resume() const { Mix_Resume(m_channel); }

		inline bool IsPlaying() const { return Mix_Playing(m_channel) != 0; }
		inline bool IsPaused() const { return Mix_Paused(m_channel) != 0; }

		void Stop() const
		{
			if (m_channel != s_InvalidChannel)
			{
				Mix_HaltChannel(m_channel);
				m_channel = s_InvalidChannel;
			}
		}

		inline const math::Vec3& GetPosition() const noexcept { return m_position; }
		inline void SetPosition(const math::Vec3& position) noexcept { m_position = position; }

		inline float GetVolume() const noexcept { return m_volume; }
		inline void SetVolume(const float volume) noexcept { m_volume = std::clamp(volume, 0.0f, 1.0f); }

	private:
		void Destroy() noexcept
		{
			if (m_chunkHandle != nullptr)
			{
				Mix_FreeChunk(m_chunkHandle);
				m_chunkHandle = nullptr;
			}
		}

		void InitiateEffects(const int loopCount, const unsigned int fadeOutMilliseconds) const
		{
			if (fadeOutMilliseconds == 0)
			{
				Mix_Volume(m_channel, static_cast<int>(m_volume * MIX_MAX_VOLUME));
			}
			else
			{
				Mix_FadeOutChannel(m_channel, fadeOutMilliseconds);
			}
			

			const math::Vec3 direction = m_position - Listener::GetPosition();
			const math::Vec3 lookAtVector = math::Normalise(Listener::GetLookAtVector());

			float distance = math::Magnitude(direction);

			if (distance > s_maxDistance)
			{
				distance = s_maxDistance;
			}

			float angleBetween = math::AngleBetween(lookAtVector, direction);

			if (const float directionSign = math::DotProduct(math::CrossProduct(direction, lookAtVector), Listener::GetUpVector());
				directionSign < 0.0f)
			{
				angleBetween += 180.0f;
			}

			constexpr Uint8 SDLMaxDistance = 255u;
			const float mappedDistance = math::MapRange({ 0.0f, s_maxDistance }, { 0.0f, SDLMaxDistance }, distance);

			Mix_SetPosition(m_channel, static_cast<Sint16>(angleBetween), static_cast<Uint8>(mappedDistance));
		}
	};
}

#endif