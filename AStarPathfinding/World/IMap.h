#pragma once
#include "FieldType.h"

template<typename TCoordinates>
class IMap
{
public:
	IMap() = default;
	virtual ~IMap() = default;

	virtual FieldType GetField(const TCoordinates& position) const noexcept = 0;

	virtual bool IsInside(const TCoordinates& position) const noexcept = 0;

	virtual void GetNeighbours(const TCoordinates& position, std::vector<TCoordinates>& result) const noexcept = 0;

	virtual double GetDistance(const TCoordinates& lhs, const TCoordinates& rhs) const noexcept = 0;
};
