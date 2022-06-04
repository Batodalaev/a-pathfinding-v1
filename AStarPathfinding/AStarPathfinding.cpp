// AStarPathfinding.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <cassert>
#include <vector>
#include <array>
#include <map>
#include <set>
#include <functional>


enum class FieldType : uint8_t
{
	/// <summary>
	/// Пустая клетка, можно ходить
	/// </summary>
	None,

	/// <summary>
	/// Непустая клетка, ходить нельзя
	/// </summary>
	Obstacle,
};

constexpr char GetFieldTypeView(FieldType field) noexcept
{
	switch (field)
	{
	case FieldType::Obstacle:
		return 'X';
	case FieldType::None:
		return ' ';
	default:
		assert(false);
		break;
	}
	return ' ';
}

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
	return lhs.X < rhs.X && lhs.Y < rhs.Y;
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

constexpr std::array<Vector2d, 4> GetNeighours4way(const Vector2d position)
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
constexpr std::array<Vector2d, 8> GetNeighours8way(const Vector2d position)
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


/// <summary>
/// two dimensional map.
/// 
/// x - vertical
/// y - horizontal
///
/// (0;0) - top left corner
/// </summary>
class Map2d final
{
	std::vector<FieldType> m_fields;
	size_t m_width = 0u;
	size_t m_height = 0u;

public:

	Map2d() = delete;
	Map2d(size_t width, size_t height) : m_fields(width* height, FieldType::None), m_width(width), m_height(height) {}
	~Map2d() = default;

	size_t GetWidth() const noexcept { return m_width; }
	size_t GetHeight() const noexcept { return m_height; }

	FieldType GetField(Vector2d position) const noexcept
	{
		assert(IsInside(position));
		return m_fields[position.X * m_width + position.Y];
	}
	void SetField(Vector2d position, FieldType  type) noexcept
	{
		assert(IsInside(position));
		m_fields[position.X * m_width + position.Y] = type;
	}

	bool IsInside(Vector2d position) const noexcept
	{
		return position.X < m_height&& position.Y < m_width;
	}
};



struct AStarNode
{
	Vector2d Position;
	Vector2d ParentPosition;

	double hWeight;
	double gWeight;
	double fWeight;
};
constexpr bool operator < (const AStarNode& lhs, const AStarNode& rhs)
{
	//We need to overload "<" to put our struct into a set
	return lhs.fWeight < rhs.fWeight;
}

template<bool hasDiagonalMove>
bool FindPath(Map2d map, Vector2d begin, Vector2d end, std::vector<Vector2d>& result)
{
	result.clear();

	std::vector<AStarNode> closedList;

	auto h = [&end](const Vector2d& position)
	{
		//TODO i want c++17 :(
		if(hasDiagonalMove)
			return double(DiagonalDistance(end, position));

		return double(ManhattanDistance(end, position));
	};

	auto successors = [&map, &h](const AStarNode& node)
	{
		std::vector<AStarNode> childNodes;

		std::vector<Vector2d> childs;

		//TODO i want c++17 :(
		if (hasDiagonalMove)
		{
			auto childs8way = GetNeighours8way(node.Position);
			childs.assign(std::begin(childs8way), std::end(childs8way));
		}
		else
		{
			auto childs4way = GetNeighours4way(node.Position);
			childs.assign(std::begin(childs4way), std::end(childs4way));
		}

		for (auto&& childNodePosition : childs)
		{
			if (map.IsInside(childNodePosition) && map.GetField(childNodePosition) != FieldType::Obstacle)
			{
				double hWeight = h(childNodePosition);
				double gWeight = node.gWeight + 1;
				
				childNodes.emplace_back(AStarNode{ childNodePosition, node.Position, hWeight , gWeight, hWeight + gWeight });
			}
		}

		return childNodes;
	};

	std::set<AStarNode, std::less<>> openList;

	double hBegin = h(begin);
	AStarNode beginNode{ begin, begin, hBegin, 0, hBegin + 0 };
	openList.insert(beginNode);

	while (!openList.empty())
	{
		//find with smallest f
		auto it = openList.begin();
		auto node = *it;
		openList.erase(it);

		auto childNodes = successors(node);

		for (auto&& childNode : childNodes)
		{
			if (childNode.Position == end)
			{
				//todo add to result
				auto currentNode = node;

				while (currentNode.Position != begin)
				{
					result.emplace_back(currentNode.Position);

					currentNode = *std::find_if(std::begin(closedList), std::end(closedList), [&currentNode](const AStarNode& value)
						{
							return value.Position == currentNode.ParentPosition;
						});
				}

				return true;
			}

			auto it2 = openList.find(childNode);
			if (it2 != std::end(openList) && it2->fWeight < childNode.fWeight)
				continue;
			
			auto it3 = std::find_if(std::begin(closedList), std::end(closedList), [&childNode](const AStarNode& value)
				{
					return value.Position == childNode.Position;
				});

			if (it3 != std::end(closedList) && it3->fWeight < childNode.fWeight)
				continue;
			
			//todo replace
			if (it2 != std::end(openList))
				openList.erase(it2);

			openList.emplace(childNode);
		}

		//node checked
		closedList.emplace_back(node);
	}

	//result.clear();
	assert(result.empty());
	return false;
}

int main()
{
	constexpr size_t Width = 70;
	constexpr size_t Height = 50;
	constexpr int RandomSeed = 0;
	constexpr int ObstacleCount = 2000;
	constexpr bool HasDiagonalMove = true;

	Map2d map(Width, Height);

	//random set obstacles
	srand(RandomSeed);
	for (int i = 0; i < ObstacleCount; ++i)
	{
		size_t obstacle = size_t(rand()) % (map.GetHeight() * map.GetWidth());
		Vector2d obstaclePosition { obstacle / map.GetWidth(), obstacle % map.GetWidth() };

		map.SetField(obstaclePosition, FieldType::Obstacle);
	}

	//random set begin and end
	Vector2d beginPosition;
	Vector2d endPosition;
	do
	{
		size_t begin = size_t(rand()) % (map.GetHeight() * map.GetWidth());
		beginPosition = Vector2d{ begin / map.GetWidth(), begin % map.GetWidth() };

		size_t end = size_t(rand()) % (map.GetHeight() * map.GetWidth());
		endPosition = Vector2d{ end / map.GetWidth(), end % map.GetWidth() };

	} while (beginPosition == endPosition ||
		map.GetField(beginPosition) == FieldType::Obstacle || map.GetField(endPosition) == FieldType::Obstacle);


	std::vector<Vector2d> path;
	const bool found = FindPath<HasDiagonalMove>(map, beginPosition, endPosition, path);

	std::cout << (found ? "Path found\n" : "Path not found\n");

	//show map
	for (size_t x = 0; x < map.GetHeight(); ++x)
	{
		for (size_t y = 0; y < map.GetWidth(); ++y)
		{
			Vector2d position{ x, y };

			if (position == beginPosition)
			{
				std::cout.put('b');
				continue;
			}
			if (position == endPosition)
			{
				std::cout.put('e');
				continue;
			}
			if (std::find(std::begin(path), std::end(path), position) != std::end(path))
			{
				std::cout.put('i');
				continue;
			}

			auto&& field = map.GetField(position);
			std::cout.put(GetFieldTypeView(field));
		}
		std::cout.put('\n');
	}
	std::cout.flush();


}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
