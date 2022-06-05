#pragma once
#include "IPath.h"

namespace PathFinder
{
	enum class IPathFinderResult : unsigned char
	{
		NotFound,
		Found
	};

	template<typename TCoordinates>
	class IPathFinder
	{
	public:
		IPathFinder() = default;
		virtual ~IPathFinder() = default;

		virtual IPathFinderResult FindPath(const TCoordinates& begin, const TCoordinates& end) = 0;
		virtual IPathFinderResult GetResult() const noexcept = 0;

		virtual const IPath<TCoordinates>& GetPath() const noexcept = 0;
	};
}
