#pragma once

namespace PathFinder
{
	template<typename TCoordinates>
	class IPath
	{
	public:
		IPath() = default;
		virtual ~IPath() = default;

		virtual TCoordinates GetCoordinates() const noexcept = 0;

		virtual double GetLength() const noexcept = 0;

		virtual void SetToBegin() noexcept = 0;
		virtual bool Next() noexcept = 0;
		virtual bool IsEnd() const noexcept = 0;

		virtual bool Contains(const TCoordinates& coordinates) const noexcept = 0;
	};
}
