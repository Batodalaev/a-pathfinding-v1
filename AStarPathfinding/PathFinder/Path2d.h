#pragma once
#include "IPath.h"
#include "..\Math\Vector2d.h"

namespace PathFinder
{
	//Поиск пути алгоритмом А*
	class Path2d final : public IPath<Math::Vector2d>
	{
		std::vector<Math::Vector2d> m_path;
		mutable size_t m_index = 0u;

	public:
		Path2d() = default;
		Path2d(std::vector<Math::Vector2d>&& path) : m_path(path) {}
		~Path2d() override = default;

		Math::Vector2d GetCoordinates() const noexcept override { assert(IsEnd()); return m_path[m_path.size() - m_index - 1]; }

		double GetLength() const noexcept override { return double(m_path.size()); }

		void SetToBegin() const noexcept override { m_index = 0u; }
		bool Next() const noexcept override
		{
			if (IsEnd())
				return false;

			++m_index;
			return true;
		}
		bool IsEnd() const noexcept override { return m_index == m_path.size(); }

		bool Contains(const Math::Vector2d& coordinates) const noexcept override
		{
			return std::find(std::begin(m_path), std::end(m_path), coordinates) != std::end(m_path);
		}
	};
}
