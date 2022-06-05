#include <cassert>

#include "AStarPathfinder.h"

namespace PathFinder
{
	IPathFinderResult AStarPathFinder::FindPath(const Math::Vector2d& begin, const Math::Vector2d& end)
	{
		if (m_searchDataDirty)
		{
			m_searchData.Clear();
		}

		m_result = IPathFinderResult::NotFound;
		m_begin = begin;
		m_end = end;

		m_openList.clear();
		m_path = Path2d();

		//temp
		std::vector<AStarNode> childNodes;

		double hBegin = m_hasDiagonalMove ? Math::EuclideanDistance(begin, end) : Math::ManhattanDistance(begin, end);
		AStarNode beginNode{ begin, 0, hBegin + 0 };
		NodeData beginNodeData{ Math::Vector2d(), beginNode.fWeight, true, false };

		m_openList.insert(beginNode);
		m_searchData.SetField(begin, beginNodeData);

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

					m_result = IPathFinderResult::Found;
					return m_result;
				}

				auto&& childNodeData = m_searchData.GetField(childNode.Position);

				if (childNodeData.InOpenList && childNodeData.FWeight <= childNode.fWeight)
					continue;
				if (childNodeData.InClosedList && childNodeData.FWeight <= childNode.fWeight)
					continue;

				childNodeData.ParentPosition = node.Position;
				childNodeData.FWeight = childNode.fWeight;
				childNodeData.InOpenList = true;
				//nodeData.InClosedList;

				m_searchData.SetField(childNode.Position, childNodeData);
				m_openList.emplace(childNode);
			}

			auto&& nodeData = m_searchData.GetField(node.Position);
			nodeData.InOpenList = false;
			nodeData.InClosedList = true;
			m_searchData.SetField(node.Position, nodeData);
		}

		return m_result;
	}

	std::vector<Math::Vector2d> AStarPathFinder::GetClosedList() const
	{
		std::vector<Math::Vector2d> result;

		for (size_t x = 0; x < m_searchData.GetHeight(); ++x)
		{
			for (size_t y = 0; y < m_searchData.GetWidth(); ++y)
			{
				Math::Vector2d position{ x, y };
				if (m_searchData.GetField(position).InClosedList)
					result.emplace_back(position);
			}
		}

		return result;
	}

	std::vector<Math::Vector2d> AStarPathFinder::GetOpenList() const
	{
		std::vector<Math::Vector2d> result;

		for (size_t x = 0; x < m_searchData.GetHeight(); ++x)
		{
			for (size_t y = 0; y < m_searchData.GetWidth(); ++y)
			{
				Math::Vector2d position{ x, y };
				if (m_searchData.GetField(position).InOpenList)
					result.emplace_back(position);
			}
		}

		return result;
	}

	void AStarPathFinder::GetSuccessors(const AStarNode& node, std::vector<AStarNode>& result) const noexcept
	{
		if (m_hasDiagonalMove)
		{
			const auto positions = Math::GetNeighours8way(node.Position);

			for (auto&& position : positions)
			{
				if (m_map.IsInside(position) && m_map.GetField(position) != World::FieldType::Obstacle)
				{
					double hWeight = Math::EuclideanDistance(position, m_end);
					double gWeight = node.gWeight + 1.;

					result.emplace_back(AStarNode{ position, gWeight, hWeight + gWeight });
				}
			}
		}
		else
		{
			const auto positions = Math::GetNeighours4way(node.Position);

			for (auto&& position : positions)
			{
				if (m_map.IsInside(position) && m_map.GetField(position) != World::FieldType::Obstacle)
				{
					double hWeight = double(Math::ManhattanDistance(position, m_end));
					double gWeight = node.gWeight + 1.;

					result.emplace_back(AStarNode{ position, gWeight, hWeight + gWeight });
				}
			}
		}
	}

	void AStarPathFinder::FillPath(const AStarNode& node)
	{
		const size_t length = size_t(node.gWeight + 1.);
		std::vector<Math::Vector2d> path;
		path.reserve(length);

		path.emplace_back(m_end);

		auto currentPosition = node.Position;

		while (currentPosition != m_begin)
		{
			path.emplace_back(currentPosition);

			currentPosition = m_searchData.GetField(currentPosition).ParentPosition; 
		}

		m_path = Path2d(std::move(path));
	}
}
