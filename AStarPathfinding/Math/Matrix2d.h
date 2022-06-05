#pragma once
#include <vector>
#include <cassert>

#include "Vector2d.h"

/// <summary>
/// two dimensional matrix.
/// 
/// x - vertical
/// y - horizontal
///
/// (0;0) - top left corner
/// </summary>

namespace Math
{
	template<typename T>
	class Matrix2d final
	{
		std::vector<T> m_fields;
		size_t m_width = 0u;
		size_t m_height = 0u;

	public:
		Matrix2d() = default;
		Matrix2d(size_t width, size_t height) : m_fields(width* height), m_width(width), m_height(height) {}
		Matrix2d(const Matrix2d& matrix) : m_fields(matrix.m_fields), m_width(matrix.m_width), m_height(matrix.m_height) {}
		~Matrix2d() = default;

		size_t GetWidth() const noexcept { return m_width; }
		size_t GetHeight() const noexcept { return m_height; }

		T GetField(const Math::Vector2d& position) const noexcept
		{
			assert(IsInside(position));
			return m_fields[position.X * m_width + position.Y];
		}

		void SetField(const Math::Vector2d& position, T  type) noexcept
		{
			assert(IsInside(position));
			m_fields[position.X * m_width + position.Y] = type;
		}

		bool IsInside(const Math::Vector2d& position) const noexcept
		{
			return position.X < m_height && position.Y < m_width;
		}
	};
}
