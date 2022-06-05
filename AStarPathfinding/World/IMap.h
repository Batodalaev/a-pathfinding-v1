#pragma once
#include "FieldType.h"

namespace World
{
	template<typename TCoordinates>
	class IMap
	{
	public:
		IMap() = default;
		virtual ~IMap() = default;

		virtual FieldType GetField(const TCoordinates& position) const noexcept = 0;

		virtual bool IsInside(const TCoordinates& position) const noexcept = 0;

		virtual void ObtainNeighbours(const TCoordinates& position, std::vector<TCoordinates>& result) const noexcept = 0;
	};
}
