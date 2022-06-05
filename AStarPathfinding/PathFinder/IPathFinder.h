#pragma once

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

	virtual const std::vector<TCoordinates>& GetPath() const noexcept = 0;
};
