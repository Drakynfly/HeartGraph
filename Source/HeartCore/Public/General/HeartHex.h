// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Misc/EnumRange.h"

namespace Heart::Hex
{
	namespace Flat
	{
		enum class EDirections : uint8
		{
			North,		// 0
			Northeast,	// 60
			Southeast,	// 120
			South,		// 180
			Southwest,	// 240
			Northwest,	// 300
		};
	}

	namespace Pointy
	{
		enum class EDirections : uint8
		{
			Northwest,	// 330
			Northeast,	// 30
			West,		// 90
			Southeast,	// 150
			Southwest,	// 210
			East		// 270
		};
	}
}

ENUM_RANGE_BY_FIRST_AND_LAST(Heart::Hex::Flat::EDirections, Heart::Hex::Flat::EDirections::North, Heart::Hex::Flat::EDirections::Northwest)
ENUM_RANGE_BY_FIRST_AND_LAST(Heart::Hex::Pointy::EDirections, Heart::Hex::Pointy::EDirections::Northwest, Heart::Hex::Pointy::EDirections::East)

namespace Heart::Hex
{
	/*------------------------------------------------------------------------------------------------
	 This namespace was based on the functions found here: https://www.redblobgames.com/grids/hexagons
	------------------------------------------------------------------------------------------------*/

	template <typename T>
	struct Statics
	{
		static constexpr T OneThird	 = 1.0 / 3.0;
		static constexpr T TwoThirds = 2.0 / 3.0;
		static constexpr T Sqrt3Over2 = UE_SQRT_3 / 2.0;
		static constexpr T Sqrt3Over3 = UE_SQRT_3 / 3.0;
	};

	struct FHex
	{
		FHex() { }

		constexpr FHex(const double Q, const double R)
		  : Q(Q), R(R) { }

		const double Q = 0.0;
		const double R = 0.0;

		constexpr FHex operator+(const FHex& Other) const
		{
			return FHex(Q + Other.Q, R + Other.R);
		}

		constexpr FHex operator-(const FHex& Other) const
		{
			return FHex(Q - Other.Q, R - Other.R);
		}
	};

	struct FCube
	{
		FCube() { }

		constexpr FCube(const double Q, const double R, const double S)
		  : Q(Q), R(R), S(S) { }

		const double Q = 0.0;
		const double R = 0.0;
		const double S = 0.0;

		constexpr FCube operator+(const FCube& Other) const
		{
			return FCube(Q + Other.Q, R + Other.R, S + Other.S);
		}

		constexpr FCube operator-(const FCube& Other) const
		{
			return FCube(Q - Other.Q, R - Other.R, S - Other.S);
		}

		constexpr FCube Abs() const
		{
			return FCube(FMath::Abs(Q), FMath::Abs(R), FMath::Abs(S));
		}
	};

	struct FHexNeighbors
	{
		const TStaticArray<FHex, 6> Out;
	};

	struct FCubeNeighbors
	{
		const TStaticArray<FCube, 6> Out;
	};

	constexpr FHex ToAxial(const FCube& Cube)
	{
		return FHex(Cube.Q, Cube.R);
	}

	constexpr FCube ToCube(const FHex Hex)
	{
		return FCube(Hex.Q, Hex.R, -Hex.Q-Hex.R);
	}

	// vectors pointing to each neighbor, with degrees in "flat" or "pointy" and cardinals for each
	static constexpr FCube DirectionVectors_Cube[6] = {
		FCube( 0.0, -1.0,  1.0),	// 0 or 330 degrees		/	NORTH or NORTHWEST
		FCube( 1.0, -1.0,  0.0),	// 60 or 30 degrees		/	NORTHEAST
		FCube( 1.0,  0.0, -1.0),	// 120 or 90 degrees	/	SOUTHEAST or WEST
		FCube( 0.0,  1.0, -1.0),	// 180 or 150 degrees	/	SOUTH or SOUTHWEST
		FCube(-1.0,  1.0,  0.0),	// 240 or 210 degrees	/	SOUTHWEST
		FCube(-1.0,  0.0,  1.0)		// 300 or 270 degrees	/	NORTHWEST or EAST
	};

	// vectors pointing to each neighbor, with degrees in "flat" or "pointy" and cardinals for each
	static constexpr FHex DirectionVectors_Axial[6] = {
		FHex( 0.0, -1.0),	// 0 or 330 degrees		/	NORTH or NORTHWEST
		FHex( 1.0, -1.0),	// 60 or 30 degrees		/	NORTHEAST
		FHex( 1.0,  0.0),	// 120 or 90 degrees	/	SOUTHEAST or WEST
		FHex( 0.0,  1.0),	// 180 or 150 degrees	/	SOUTH or SOUTHWEST
		FHex(-1.0,  1.0),	// 240 or 210 degrees	/	SOUTHWEST
		FHex(-1.0,  0.0)	// 300 or 270 degrees	/	NORTHWEST or EAST
	};

	template <typename DirectionEnum> constexpr FCube Direction_Cube(const DirectionEnum Direction)
	{
		return DirectionVectors_Cube[static_cast<uint8>(Direction)];
	}

	template <typename DirectionEnum> constexpr FHex Direction_Axial(const DirectionEnum Direction)
	{
		return DirectionVectors_Axial[static_cast<uint8>(Direction)];
	}

	template <typename DirectionEnum> constexpr FCube Neighbor_Cube(const FCube& Cube, const DirectionEnum Direction)
	{
		return Cube + Direction_Cube(Direction);
	}

	template <typename DirectionEnum> constexpr FHex Neighbor_Axial(const FHex& Hex, const DirectionEnum Direction)
	{
		return Hex + Direction_Axial(Direction);
	}

	template <typename DirectionEnum> FCubeNeighbors Neighbors_Cube(const FCube& Cube)
	{
		TStaticArray<FCube, 6> Out;
		for (DirectionEnum Direction : TEnumRange<DirectionEnum>())
		{
			Out[static_cast<uint8>(Direction)] = Neighbor_Cube(Cube, Direction);
		}
		return FCubeNeighbors{Out};
	}

	template <typename DirectionEnum> FHexNeighbors Neighbors_Axial(const FHex& Hex)
	{
		TStaticArray<FHex, 6> Out;
		for (DirectionEnum Direction : TEnumRange<DirectionEnum>())
		{
			Out[static_cast<uint8>(Direction)] = Neighbor_Axial(Hex, Direction);
		}
		return FHexNeighbors{Out};
	}

	FORCEINLINE_DEBUGGABLE FCube RoundHalfToEven(const FCube& Cube)
	{
		double RoundQ = FMath::RoundHalfToEven(Cube.Q);
		double RoundR = FMath::RoundHalfToEven(Cube.R);
		double RoundS = FMath::RoundHalfToEven(Cube.S);

		const FCube Diff = (FCube(RoundQ, RoundR, RoundS) - Cube).Abs();

		if (Diff.Q > Diff.R && Diff.Q > Diff.S)
		{
			RoundQ = -RoundR-RoundS;
		}
		else if (Diff.R > Diff.S)
		{
			RoundR = -RoundQ-RoundS;
		}
		else
		{
			RoundS = -RoundQ-RoundR;
		}

		return FCube(RoundQ, RoundR, RoundS);
	}

	FORCEINLINE_DEBUGGABLE FHex RoundHalfToEven(const FHex& Hex)
	{
		return ToAxial(RoundHalfToEven(ToCube(Hex)));
	}

	FORCEINLINE_DEBUGGABLE FVector2D HexToPixel_Flat(const FHex& Hex, const double Size)
	{
		FVector2D Pixel;
		Pixel.X = Size * (1.5 * Hex.Q);
		Pixel.Y = Size * (Statics<double>::Sqrt3Over2 * Hex.Q + UE_SQRT_3 * Hex.R);
		return Pixel;
	}

	FORCEINLINE_DEBUGGABLE FVector2D HexToPixel_Pointy(const FHex& Hex, const double Size)
	{
		FVector2D Pixel;
		Pixel.X = Size * (UE_SQRT_3 * Hex.Q + Statics<double>::Sqrt3Over2 * Hex.R);
		Pixel.Y = Size * (1.5 * Hex.R);
		return Pixel;
	}

	FORCEINLINE_DEBUGGABLE FHex PixelToHex_Flat(const FVector2D Point, const double Size)
	{
		auto&& HexQ = (Statics<double>::TwoThirds * Point.X) / Size;
		auto&& HexR = ((-Statics<double>::OneThird * Point.X) + (Statics<double>::Sqrt3Over3 * Point.Y)) / Size;
		return RoundHalfToEven(FHex(HexQ, HexR));
	}

	FORCEINLINE_DEBUGGABLE FHex PixelToHex_Pointy(const FVector2D Point, const double Size)
	{
        auto&& HexQ = (Statics<double>::Sqrt3Over3 * Point.X - 1.0 / 3.0 * Point.Y) / Size;
        auto&& HexR = (Statics<double>::TwoThirds * Point.Y) / Size;
        return RoundHalfToEven(FHex(HexQ, HexR));
	}

	FORCEINLINE_DEBUGGABLE FVector2D SnapToNearestHex_Flat(const FVector2D& Location, const double Size)
	{
		auto&& HexCoord = PixelToHex_Flat(Location, Size);
		return HexToPixel_Flat(HexCoord, Size);
	}

	FORCEINLINE_DEBUGGABLE FVector2D SnapToNearestHex_Pointy(const FVector2D& Location, const double Size)
	{
		auto&& HexCoord = PixelToHex_Pointy(Location, Size);
		return HexToPixel_Pointy(HexCoord, Size);
	}
}