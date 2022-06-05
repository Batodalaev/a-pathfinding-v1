#pragma once
#include <vector>
#include <unordered_map>
#include <set>

#include "IPathFinder.h"
#include "..\World\IMap.h"

#include "..\Math\Vector2d.h"

namespace details
{
	struct AStarNode
	{
		Vector2d Position;
		Vector2d ParentPosition;

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
class AStarPathFinder final : public IPathFinder<Vector2d>
{
	using AStarNode = details::AStarNode;

public:
	AStarPathFinder() = delete;
	AStarPathFinder(const IMap<Vector2d>& map) : m_map(map) {}
	virtual ~AStarPathFinder() override = default;

	IPathFinderResult FindPath(const Vector2d& begin, const Vector2d& end) override;

	const std::vector<Vector2d>& GetPath() const noexcept override {	return m_path; }

	std::vector<Vector2d> GetClosedList() const;
	std::vector<Vector2d> GetOpenList() const;
private:
	const IMap<Vector2d>& m_map;

	std::vector<Vector2d> m_path;
	std::unordered_map<Vector2d, AStarNode> m_closedList;
	std::set<AStarNode> m_openList;

	Vector2d m_begin;
	Vector2d m_end;

	void GetSuccessors(const AStarNode& node, std::vector<AStarNode>& result) const noexcept;
	void FillPath(const AStarNode& node);

};
