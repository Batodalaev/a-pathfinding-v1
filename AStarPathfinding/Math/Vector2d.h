#pragma once
#include <array>
#include <cmath>

namespace Math
{
	struct Vector2d final
	{
		size_t X = 0u;
		size_t Y = 0u;
	};

	constexpr bool operator==(const Vector2d& lhs, const Vector2d& rhs)
	{
		return lhs.X == rhs.X && lhs.Y == rhs.Y;
	}
	constexpr bool operator!=(const Vector2d& lhs, const Vector2d& rhs)
	{
		return !(lhs == rhs);
	}
	constexpr bool operator < (const Vector2d& lhs, const Vector2d& rhs)
	{
		//We need to overload "<" to put our struct into a set
		return lhs.X < rhs.X&& lhs.Y < rhs.Y;
	}

	//4 way
	constexpr size_t ManhattanDistance(const Vector2d& lhs, const Vector2d& rhs)
	{
		size_t dX = lhs.X > rhs.X ? lhs.X - rhs.X : rhs.X - lhs.X;
		size_t dY = lhs.Y > rhs.Y ? lhs.Y - rhs.Y : rhs.Y - lhs.Y;

		return dX + dY;
	}

	//8 way
	constexpr double DiagonalDistance(const Vector2d& lhs, const Vector2d& rhs)
	{
		constexpr size_t D = 1;
		constexpr double D2 = 1.4142135623'7309504880'1688724209;//sqrt(2);

		size_t dX = lhs.X > rhs.X ? lhs.X - rhs.X : rhs.X - lhs.X;
		size_t dY = lhs.Y > rhs.Y ? lhs.Y - rhs.Y : rhs.Y - lhs.Y;

		size_t dMin = dX > dY ? dX : dY;
		return double(D * (dX + dY)) + (D2 - 2. * D) * double(dMin);
	}

	//constexpr
	inline double EuclideanDistance(const Vector2d& lhs, const Vector2d& rhs)
	{
		size_t dX = lhs.X > rhs.X ? lhs.X - rhs.X : rhs.X - lhs.X;
		size_t dY = lhs.Y > rhs.Y ? lhs.Y - rhs.Y : rhs.Y - lhs.Y;

		double distanceSquared = double(dX) * double(dX) + double(dY) * double(dY);

		return std::sqrt(distanceSquared);
	}

	constexpr std::array<Vector2d, 4> GetNeighours4way(const Vector2d& position)
	{
		return
		{
			//top
			Vector2d{position.X - 1, position.Y},
			//bottom
			Vector2d{position.X + 1, position.Y},
			//left
			Vector2d{position.X, position.Y - 1},
			//right
			Vector2d{position.X, position.Y + 1},

		};
	}
	constexpr std::array<Vector2d, 8> GetNeighours8way(const Vector2d& position)
	{
		return
		{
			//top
			Vector2d{position.X - 1, position.Y},
			//bottom
			Vector2d{position.X + 1, position.Y},

			//top - left
			Vector2d{position.X - 1, position.Y - 1},
			//top - right
			Vector2d{position.X - 1, position.Y + 1},

			//bottom - left
			Vector2d{position.X + 1, position.Y - 1},
			//bottom - right
			Vector2d{position.X + 1, position.Y + 1},

			//left
			Vector2d{position.X, position.Y - 1},
			//right
			Vector2d{position.X, position.Y + 1},
		};
	}
}

namespace std
{

	template <>
	struct hash<Math::Vector2d>
	{
		size_t operator()(const Math::Vector2d& pos) const
		{
			size_t rowHash = std::hash<size_t>()(pos.X);
			size_t colHash = std::hash<size_t>()(pos.Y) << 1;
			return rowHash ^ colHash;
		}
	};

}
