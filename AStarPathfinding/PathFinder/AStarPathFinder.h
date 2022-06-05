#pragma once
#include <vector>
#include <unordered_map>
#include <set>

#include "IPathFinder.h"
#include "Path2d.h"

#include "..\World\Map2d.h"
#include "..\Math\Vector2d.h"
#include "..\Math\Matrix2d.h"

namespace PathFinder
{
	namespace details
	{
		struct AStarNode final
		{
			Math::Vector2d Position;

			//double hWeight = 0.;
			double gWeight = 0.;
			double fWeight = 0.;
		};

		constexpr bool operator < (const AStarNode& lhs, const AStarNode& rhs)
		{
			//We need to overload "<" to put our struct into a set
			return lhs.fWeight < rhs.fWeight;
		}

	}

	//Поиск пути алгоритмом А*
	class AStarPathFinder final : public IPathFinder<Math::Vector2d>
	{
		using AStarNode = details::AStarNode;

	public:
		AStarPathFinder() = delete;
		AStarPathFinder(const World::Map2d& map) : m_map(map), m_searchData(map.GetWidth(), map.GetHeight()){}
		~AStarPathFinder() override = default;

		IPathFinderResult FindPath(const Math::Vector2d& begin, const Math::Vector2d& end) override;
		IPathFinderResult GetResult() const noexcept override { return m_result; }
		const IPath<Math::Vector2d>& GetPath() const noexcept override { return m_path; }

		std::vector<Math::Vector2d> GetClosedList() const;
		std::vector<Math::Vector2d> GetOpenList() const;

		//todo IMapWalker
		void SetHasDiagonalMove(bool has) noexcept { m_hasDiagonalMove = has; }
	private:
		const World::IMap<Math::Vector2d>& m_map;

		struct NodeData final
		{
			Math::Vector2d ParentPosition;
			double FWeight = 0.f;

			//TODO compress to 1 byte (maybe not need?)
			bool InOpenList = false;
			bool InClosedList = false;
		};
		Math::Matrix2d<NodeData> m_searchData;
		bool m_searchDataDirty = false;

		std::multiset<AStarNode> m_openList;
		Path2d m_path;

		Math::Vector2d m_begin;
		Math::Vector2d m_end;

		IPathFinderResult m_result = IPathFinderResult::NotFound;
		bool m_hasDiagonalMove = true;

		void GetSuccessors(const AStarNode& node, std::vector<AStarNode>& result) const noexcept;
		void FillPath(const AStarNode& node);
		double GetDistance(const Math::Vector2d& lhs, const Math::Vector2d& rhs) const noexcept;

	};
}
