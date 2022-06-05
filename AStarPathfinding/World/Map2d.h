#pragma once
#include <vector>
#include <array>
#include <cassert>

#include "..\Math\Vector2d.h"
#include "FieldType.h"
#include "IMap.h"

/// <summary>
/// two dimensional map.
/// 
/// x - vertical
/// y - horizontal
///
/// (0;0) - top left corner
/// </summary>

template<bool hasDiagonalMove>
class Map2d final : public IMap<Vector2d>
{
	std::vector<FieldType> m_fields;
	size_t m_width = 0u;
	size_t m_height = 0u;

public:

	Map2d() = delete;
	Map2d(size_t width, size_t height) : m_fields(width* height, FieldType::None), m_width(width), m_height(height) {}
	~Map2d() override = default;

	size_t GetWidth() const noexcept { return m_width; }
	size_t GetHeight() const noexcept { return m_height; }

	FieldType GetField(const Vector2d& position) const noexcept override
	{
		assert(IsInside(position));
		return m_fields[position.X * m_width + position.Y];
	}

	void SetField(const Vector2d& position, FieldType  type) noexcept
	{
		assert(IsInside(position));
		m_fields[position.X * m_width + position.Y] = type;
	}

	bool IsInside(const Vector2d& position) const noexcept override
	{
		return position.X < m_height&& position.Y < m_width;
	}

	void GetNeighbours(const Vector2d& position, std::vector<Vector2d>& result) const noexcept override
	{
		assert(result.empty());

		//TODO i want c++17 :(
		if (hasDiagonalMove)
		{
			auto childs8way = GetNeighours8way(position);
			result.assign(std::begin(childs8way), std::end(childs8way));
		}
		else
		{
			auto childs4way = GetNeighours4way(position);
			result.assign(std::begin(childs4way), std::end(childs4way));
		}
	}

	double GetDistance(const Vector2d& lhs, const Vector2d& rhs) const noexcept override
	{
		//return EuclideanDistance(lhs, rhs);

		//TODO i want c++17 :(
		if (hasDiagonalMove)
			return DiagonalDistance(lhs, rhs);
		
		return double(ManhattanDistance(lhs, rhs));
	}
};
