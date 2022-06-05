// AStarPathfinding.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <cassert>
#include <vector>
#include <array>
#include <map>
#include <set>
#include <functional>

#include "World/FieldType.h"
#include "Math/Vector2d.h"
#include "World/Map2d.h"
#include "PathFinder/AStarPathFinder.h"


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



int main()
{
	constexpr size_t Width = 1000;
	constexpr size_t Height = 1000;
	constexpr int RandomSeed = 0;
	constexpr int ObstacleCount = Width*Height / 100;
	constexpr bool HasDiagonalMove = true;

	Map2d<HasDiagonalMove> map(Width, Height);

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

	
	AStarPathFinder pathFinder(map);
	const auto found = pathFinder.FindPath(beginPosition, endPosition);

	std::cout << (found == IPathFinderResult::Found ? "Path found\n" : "Path not found\n");

	return 0;

	//show map
	const auto& path = pathFinder.GetPath();
	const auto& openList = pathFinder.GetOpenList();
	const auto& closedList = pathFinder.GetClosedList();

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
			if (std::find(std::begin(closedList), std::end(closedList), position) != std::end(closedList))
			{
				std::cout.put('.');
				continue;
			}
			if (std::find(std::begin(openList), std::end(openList), position) != std::end(openList))
			{
				std::cout.put('+');
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
