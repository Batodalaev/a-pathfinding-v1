// AStarPathfinding.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <cassert>
#include <vector>
#include <array>
#include <map>
#include <set>
#include <functional>
#include <chrono>
#include <thread>
#include <future>
#include <thread>

#include "World/FieldType.h"
#include "Math/Vector2d.h"
#include "World/Map2d.h"
#include "PathFinder/AStarPathFinder.h"

namespace
{
	constexpr char Obstacle = 'X';
	constexpr char EmptyField = ' ';
	constexpr char BeginPath = 'b';
	constexpr char IntermediatePath = 'i';
	constexpr char EndPath = 'e';
	constexpr char OpenListFieldPath = 'o';
	constexpr char ClosedListFieldPath = 'c';

	constexpr size_t MaxViewWidth = 79;
	constexpr size_t MaxViewHeight = 65;
}

constexpr char GetFieldTypeView(World::FieldType field) noexcept
{
	switch (field)
	{
	case World::FieldType::Obstacle:
		return Obstacle;
	case World::FieldType::None:
		return EmptyField;
	default:
		assert(false);
		break;
	}
	return EmptyField;
}

void renderPath(Math::Matrix2d<char>& view, const PathFinder::IPath<Math::Vector2d>& path, char type)
{
	path.SetToBegin();

	while (!path.IsEnd())
	{
		view.SetField(path.GetCoordinates(), type);
		path.Next();
	}
	path.SetToBegin();
}
void renderArray(Math::Matrix2d<char>& view, const std::vector<Math::Vector2d>& positions, char type)
{
	for (auto&& position : positions)
	{
		view.SetField(position, type);
	}
}
void renderMap(Math::Matrix2d<char>& view, const Math::Matrix2d<World::FieldType>& map)
{
	for (size_t x = 0; x < view.GetHeight(); ++x)
	{
		for (size_t y = 0; y < view.GetWidth(); ++y)
		{
			Math::Vector2d position{ x, y };

			auto&& field = map.GetField(position);
			view.SetField(position, GetFieldTypeView(field));
		}
	}
}

void draw(const Math::Matrix2d<char>& view)
{
	for (size_t x = 0; x < view.GetHeight(); ++x)
	{
		for (size_t y = 0; y < view.GetWidth(); ++y)
		{
			Math::Vector2d position{ x, y };
			auto&& field = view.GetField(position);
			std::cout.put(field);
		}
		std::cout.put('\n');
	}
	std::cout.flush();
}
void renderPathFinder(Math::Matrix2d<char>& view, const PathFinder::AStarPathFinder& pathFinder, bool hasDebug = false)
{
	const bool found = pathFinder.GetResult() == PathFinder::IPathFinderResult::Found;

	if (hasDebug)
	{
		const auto& openList = pathFinder.GetOpenList();
		const auto& closedList = pathFinder.GetClosedList();

		renderArray(view, openList, OpenListFieldPath);
		renderArray(view, closedList, ClosedListFieldPath);
	}

	if (found)
	{
		const auto& path = pathFinder.GetPath();

		renderPath(view, path, IntermediatePath);
	}
}

World::Map2d generateMap()
{
	//constexpr size_t Width = 8;
	//constexpr size_t Height = 8;
	//constexpr int RandomSeed = 0;
	//constexpr int ObstacleCount = 16;

	//constexpr size_t Width = 79;
	//constexpr size_t Height = 65;
	//constexpr int RandomSeed = 2;
	//constexpr int ObstacleCount = 1000;// Width* Height / 10;

	constexpr size_t Width = 5000;
	constexpr size_t Height = 5000;
	constexpr int RandomSeed = 0;
	constexpr int ObstacleCount = Width* Height / 4000;

	World::Map2d map(Width, Height);

	//random set obstacles
	srand(RandomSeed);
	for (int i = 0; i < ObstacleCount; ++i)
	{
		size_t obstacle = size_t(rand()) % (map.GetHeight() * map.GetWidth());
		Math::Vector2d obstaclePosition{ obstacle / map.GetWidth(), obstacle % map.GetWidth() };

		map.SetField(obstaclePosition, World::FieldType::Obstacle);
	}

	return map;
}

std::array<Math::Vector2d, 2> generatePositions(const World::Map2d& map)
{
	//random set begin and end
	Math::Vector2d beginPosition;
	Math::Vector2d endPosition;
	do
	{
		size_t begin = size_t(rand()) % (map.GetHeight() * map.GetWidth());
		beginPosition = Math::Vector2d{ begin / map.GetWidth(), begin % map.GetWidth() };

		size_t end = size_t(rand()) % (map.GetHeight() * map.GetWidth());
		endPosition = Math::Vector2d{ end / map.GetWidth(), end % map.GetWidth() };

	} while (beginPosition == endPosition);

	return { beginPosition, endPosition };
}
int main()
{
	auto map = generateMap();
	auto positions = generatePositions(map);
	auto beginPosition = positions[0];
	auto endPosition = positions[1];

	//force empty field
	map.SetField(beginPosition, World::FieldType::None);
	map.SetField(endPosition, World::FieldType::None);


	bool Debug = true;
	constexpr bool Async = true;
	double FPS = 1.;
	size_t FrameTime = size_t(1000. / FPS);

	//copy map for async path finding
	auto mapAsync = map;
	PathFinder::AStarPathFinder pathFinder(mapAsync);

	std::future<PathFinder::IPathFinderResult> future =
		std::async(Async ? std::launch::async : std::launch::deferred, [&]()
			{
				return pathFinder.FindPath(beginPosition, endPosition);
			});

	Math::Matrix2d<char> view(map.GetWidth(), map.GetHeight());
	bool pathFinderStopped = false;

	while (true)
	{
		//update
		if (future.valid())
		{
			std::future_status status = future.wait_for(std::chrono::milliseconds(0));
			if (status == std::future_status::ready)
				pathFinderStopped = true;

			if (status == std::future_status::deferred)
			{
				future.get();
				pathFinderStopped = true;
			}
		}


		//draw to view
		renderMap(view, map.GetFields());
		if (pathFinderStopped)
		{
			renderPathFinder(view, pathFinder, Debug);
		}
		view.SetField(beginPosition, BeginPath);
		view.SetField(endPosition, EndPath);

		//draw to console
		system("cls");
		if (pathFinderStopped)
		{
			const bool found = pathFinder.GetResult() == PathFinder::IPathFinderResult::Found;
			std::cout << (found ? "Path found\n" : "Path not found\nn");
			if (found)
				std::cout << "Path length: " << pathFinder.GetPath().GetLength() << "\n";
		}
		else
		{
			std::cout << "Finding a path\n\n";
		}

		if(view.GetWidth() <= MaxViewWidth && view.GetHeight() <= MaxViewHeight)
			draw(view);

		std::this_thread::sleep_for(std::chrono::milliseconds(FrameTime));
	}
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
