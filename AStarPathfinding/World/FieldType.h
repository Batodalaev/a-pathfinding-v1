#pragma once

enum class FieldType : unsigned char
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
