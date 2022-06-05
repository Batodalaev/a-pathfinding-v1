#pragma once
#include <vector>
#include <array>
#include <cassert>

#include "..\Math\Vector2d.h"
#include "..\Math\Matrix2d.h"
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

namespace World
{
	class Map2d final : public IMap<Math::Vector2d>
	{
		Math::Matrix2d<FieldType> m_fields;

	public:

		Map2d() = delete;
		Map2d(size_t width, size_t height) : m_fields(width, height){}
		Map2d(const Map2d& map) : m_fields(map.m_fields) {}
		~Map2d() override = default;

		const Math::Matrix2d<FieldType>& GetFields() const noexcept { return m_fields; }

		size_t GetWidth() const noexcept { return m_fields.GetWidth(); }
		size_t GetHeight() const noexcept { return m_fields.GetHeight(); }

		FieldType GetField(const Math::Vector2d& position) const noexcept override
		{
			return m_fields.GetField(position);
		}

		void SetField(const Math::Vector2d& position, FieldType  type) noexcept
		{
			m_fields.SetField(position, type);
		}

		bool IsInside(const Math::Vector2d& position) const noexcept override
		{
			return m_fields.IsInside(position);
		}
	};
}
