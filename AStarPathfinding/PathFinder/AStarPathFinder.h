#pragma once
#include <vector>
#include <unordered_map>
#include <set>

#include "IPathFinder.h"
#include "Path2d.h"

#include "..\World\IMap.h"
#include "..\Math\Vector2d.h"

namespace PathFinder
{
	namespace details
	{
		struct AStarNode
		{
			Math::Vector2d Position;
			Math::Vector2d ParentPosition;

			double hWeight = 0.;
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
		AStarPathFinder(const World::IMap<Math::Vector2d>& map) : m_map(map) {}
		~AStarPathFinder() override = default;

		IPathFinderResult FindPath(const Math::Vector2d& begin, const Math::Vector2d& end) override;
		IPathFinderResult GetResult() const noexcept override { return m_result; }
		const IPath<Math::Vector2d>& GetPath() const noexcept override { return m_path; }

		std::vector<Math::Vector2d> GetClosedList() const;
		std::vector<Math::Vector2d> GetOpenList() const;
	private:
		const World::IMap<Math::Vector2d>& m_map;

		std::unordered_map<Math::Vector2d, AStarNode> m_closedList;
		std::set<AStarNode> m_openList;
		Path2d m_path;

		Math::Vector2d m_begin;
		Math::Vector2d m_end;

		IPathFinderResult m_result = IPathFinderResult::NotFound;

		void GetSuccessors(const AStarNode& node, std::vector<AStarNode>& result) const noexcept;
		void FillPath(const AStarNode& node);

	};
}
