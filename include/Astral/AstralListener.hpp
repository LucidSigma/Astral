#pragma once
#ifndef ASTRAL_LISTENER_HPP
#define ASTRAL_LISTENER_HPP

#include "AstralMath.hpp"

namespace astral
{
	class Listener
	{
	private:
		inline static math::Vec3 m_position{ 0.0f, 0.0f, 0.0f };
		inline static math::Vec3 m_lookAtVector{ 0.0f, 0.0f, 1.0f };
		inline static math::Vec3 m_upVector{ 0.0f, 1.0f, 0.0f };

	public:
		inline static const math::Vec3& GetPosition() noexcept { return m_position; }
		inline static void SetPosition(const math::Vec3& position) noexcept { m_position = position; }

		inline static const math::Vec3& GetLookAtVector() noexcept { return m_lookAtVector; }
		inline static void SetLookAtVector(const math::Vec3& lookAtVector) noexcept { m_lookAtVector = lookAtVector; }

		inline static const math::Vec3& GetUpVector() noexcept { return m_upVector; }
		inline static void SetUpVector(const math::Vec3& upVector) noexcept { m_upVector = upVector; }

		Listener() = delete;

		Listener(const Listener&) = delete;
		Listener& operator =(const Listener&) = delete;

		Listener(Listener&&) noexcept = delete;
		Listener& operator =(Listener&&) noexcept = delete;

		~Listener() noexcept = delete;
	};
}

#endif