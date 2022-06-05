#include <cassert>

#include "AStarPathfinder.h"

namespace PathFinder
{
	IPathFinderResult AStarPathFinder::FindPath(const Math::Vector2d& begin, const Math::Vector2d& end)
	{
		m_begin = begin;
		m_end = end;

		m_openList.clear();
		m_closedList.clear();
		m_path = Path2d();

		//temp
		std::vector<AStarNode> childNodes;

		double hBegin = m_map.GetDistance(begin, end);
		AStarNode beginNode{ begin, begin, hBegin, 0, hBegin + 0 };
		m_openList.insert(beginNode);

		while (!m_openList.empty())
		{
			//find with smallest f
			auto it = m_openList.begin();
			auto node = *it;
			m_openList.erase(it);

			childNodes.clear();
			GetSuccessors(node, childNodes);

			for (auto&& childNode : childNodes)
			{
				if (childNode.Position == end)
				{
					FillPath(node);

					return IPathFinderResult::Found;
				}

				auto it2 = m_openList.find(childNode);
				if (it2 != std::end(m_openList) && it2->fWeight < childNode.fWeight)
					continue;

				auto it3 = m_closedList.find(childNode.Position);

				if (it3 != std::end(m_closedList) && it3->second.fWeight <= childNode.fWeight)
					continue;

				//todo replace
				if (it2 != std::end(m_openList))
					m_openList.erase(it2);

				m_openList.emplace(childNode);
			}

			//node checked 
			auto pair = m_closedList.emplace(node.Position, node);

			//(replace if exists and lower f
			if (!pair.second && pair.first->second.fWeight > node.fWeight)
			{
				pair.first->second = node;
			}
		}

		return IPathFinderResult::NotFound;
	}

	std::vector<Math::Vector2d> AStarPathFinder::GetClosedList() const
	{
		std::vector<Math::Vector2d> result;
		result.reserve(m_closedList.size());

		for (auto&& node : m_closedList)
		{
			result.emplace_back(node.second.Position);
		}

		return result;
	}

	std::vector<Math::Vector2d> AStarPathFinder::GetOpenList() const
	{
		std::vector<Math::Vector2d> result;
		result.reserve(m_openList.size());

		for (auto&& node : m_openList)
		{
			result.emplace_back(node.Position);
		}

		return result;
	}

	void AStarPathFinder::GetSuccessors(const AStarNode& node, std::vector<AStarNode>& result) const noexcept
	{
		std::vector<Math::Vector2d> positions;
		m_map.GetNeighbours(node.Position, positions);

		for (auto&& position : positions)
		{
			if (m_map.IsInside(position) && m_map.GetField(position) != World::FieldType::Obstacle)
			{
				double hWeight = m_map.GetDistance(position, m_end);
				double gWeight = node.gWeight + 1.;

				result.emplace_back(AStarNode{ position, node.Position, hWeight , gWeight, hWeight + gWeight });
			}
		}
	}

	void AStarPathFinder::FillPath(const AStarNode& node)
	{
		const size_t length = size_t(node.gWeight + 1.);
		std::vector<Math::Vector2d> path;
		path.reserve(length);

		path.emplace_back(m_end);

		auto currentNode = node;

		while (currentNode.Position != m_begin)
		{
			path.emplace_back(currentNode.Position);

			currentNode = m_closedList.find(currentNode.ParentPosition)->second;
		}

		m_path = Path2d(std::move(path));
	}
}
