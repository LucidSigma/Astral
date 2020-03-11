#pragma once
#ifndef ASTRAL_MATH_HPP
#define ASTRAL_MATH_HPP

#include <cmath>
#include <type_traits>
#include <utility>

#include <SDL2/SDL.h>

namespace astral::math
{
	struct Vec3
	{
		float x;
		float y;
		float z;
	};

	constexpr Vec3 operator -(const Vec3& lhs, const Vec3& rhs) noexcept { return Vec3{ lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z }; }
	constexpr Vec3 operator *(const Vec3& vector, const float scalar) noexcept { return Vec3{ vector.x * scalar, vector.y * scalar, vector.z * scalar }; }

	float Magnitude(const Vec3& vector)
	{
		return std::sqrtf(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
	}

	Vec3 Normalise(const Vec3& vector)
	{
		return vector * (1.0f / Magnitude(vector));
	}

	constexpr float DotProduct(const Vec3& lhs, const Vec3& rhs)
	{
		return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
	}

	constexpr Vec3 CrossProduct(const Vec3& lhs, const Vec3& rhs)
	{
		return Vec3{
			lhs.y * rhs.z - lhs.z * rhs.y,
			lhs.z * rhs.x - lhs.x * rhs.z,
			lhs.x * rhs.y - lhs.y * rhs.x
		};
	}

	float AngleBetween(const Vec3& lhs, const Vec3& rhs)
	{
		const Vec3 normalisedLHS = Normalise(lhs);
		const Vec3 normalisedRHS = Normalise(rhs);

		float angleCosine = DotProduct(normalisedLHS, normalisedRHS);

		return std::acosf(angleCosine) * 180.0f / static_cast<float>(M_PI);
	}

	template <typename T = float, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
	constexpr float MapRange(const std::pair<float, float>& rangeA, const std::pair<float, float>& rangeB, const float value)
	{
		return (value - rangeA.first) * ((rangeB.second - rangeB.first) / (rangeA.second - rangeA.first)) + rangeB.first;
	}
}

#endif