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


constexpr char GetFieldTypeView(World::FieldType field) noexcept
{
	switch (field)
	{
	case World::FieldType::Obstacle:
		return 'X';
	case World::FieldType::None:
		return ' ';
	default:
		assert(false);
		break;
	}
	return ' ';
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

int main()
{
	//constexpr size_t Width = 8;
	//constexpr size_t Height = 8;
	//constexpr int RandomSeed = 0;
	//constexpr int ObstacleCount = 16;
	//constexpr bool HasDiagonalMove = true;

	constexpr size_t Width = 79;
	constexpr size_t Height = 65;
	constexpr int RandomSeed = 2;
	constexpr int ObstacleCount = 1000;// Width* Height / 10;
	constexpr bool HasDiagonalMove = true;

	bool Debug = false;
	constexpr bool Async = true;
	double FPS = 1.;
	size_t FrameTime = size_t(1000. / FPS);

	World::Map2d<HasDiagonalMove> map(Width, Height);

	//random set obstacles
	srand(RandomSeed);
	for (int i = 0; i < ObstacleCount; ++i)
	{
		size_t obstacle = size_t(rand()) % (map.GetHeight() * map.GetWidth());
		Math::Vector2d obstaclePosition { obstacle / map.GetWidth(), obstacle % map.GetWidth() };

		map.SetField(obstaclePosition, World::FieldType::Obstacle);
	}

	//random set begin and end
	Math::Vector2d beginPosition;
	Math::Vector2d endPosition;
	do
	{
		size_t begin = size_t(rand()) % (map.GetHeight() * map.GetWidth());
		beginPosition = Math::Vector2d{ begin / map.GetWidth(), begin % map.GetWidth() };

		size_t end = size_t(rand()) % (map.GetHeight() * map.GetWidth());
		endPosition = Math::Vector2d{ end / map.GetWidth(), end % map.GetWidth() };

	} while (beginPosition == endPosition ||
		map.GetField(beginPosition) == World::FieldType::Obstacle || map.GetField(endPosition) == World::FieldType::Obstacle);

	if (Async)
	{
		auto mapAsync = map;
		PathFinder::AStarPathFinder pathFinder(mapAsync);

		std::future<PathFinder::IPathFinderResult> future = std::async(std::launch::async, [&]()
			{
			return pathFinder.FindPath(beginPosition, endPosition);
			});

		Math::Matrix2d<char> view(Width, Height);

		renderMap(view, map.GetFields());

		bool pathFinderStopped = false;
		while (true)
		{
			if (future.valid())
			{
				std::future_status status = future.wait_for(std::chrono::milliseconds(FrameTime));
				if (status == std::future_status::ready)
					pathFinderStopped = true;					
			}
			
			if(pathFinderStopped)
			{
				const bool found = pathFinder.GetResult() == PathFinder::IPathFinderResult::Found;
				
				const auto& path = pathFinder.GetPath();

				if (Debug)
				{
					const auto& openList = pathFinder.GetOpenList();
					const auto& closedList = pathFinder.GetClosedList();

					renderArray(view, openList, 'o');
					renderArray(view, closedList, 'c');
				}

				if (found)
				{
					renderPath(view, path, 'i');
				}
				view.SetField(beginPosition, 'b');
				view.SetField(endPosition, 'e');

				std::this_thread::sleep_for(std::chrono::milliseconds(FrameTime));
			}
			
			system("cls");

			if (pathFinderStopped)
			{
				const bool found = pathFinder.GetResult() == PathFinder::IPathFinderResult::Found;
				std::cout << (found ? "Path found\n" : "Path not found\n");
			}
			else
			{
				std::cout << "Finding a path\n";
			}

			draw(view);
		}
	}
	else
	{
		PathFinder::AStarPathFinder pathFinder(map);
		const bool found = pathFinder.FindPath(beginPosition, endPosition) == PathFinder::IPathFinderResult::Found;

		//show map
		const auto& path = pathFinder.GetPath();
		
		Math::Matrix2d<char> view(Width, Height);

		renderMap(view, map.GetFields());

		if (Debug)
		{
			const auto& openList = pathFinder.GetOpenList();
			const auto& closedList = pathFinder.GetClosedList();
			renderArray(view, openList, 'o');
			renderArray(view, closedList, 'c');
		}
		if (found)
			renderPath(view, path, 'i');

		view.SetField(beginPosition, 'b');
		view.SetField(endPosition, 'e');
		draw(view);

		std::cout << (found ? "Path found\n" : "Path not found\n");
		if (found)
			std::cout << "Path length: " << pathFinder.GetPath().GetLength() << '\n';

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
