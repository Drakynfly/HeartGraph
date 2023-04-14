// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

namespace Heart::Math
{
	FORCEINLINE FVector2D SaveDivide(const FVector2D& A, const double& Scale)
	{
		const double RScale = Scale != 0.0f ? 1.f/Scale : 1.f;
		return FVector2D(A.X * RScale, A.Y * RScale);
	}

	FORCEINLINE FVector2D SaveDivide(const FVector2D& A, const FVector2D& B)
	{
		return { (B.X != 0.0f) ? (A.X / B.X) : 0.0f, (B.Y != 0.0f) ? (A.Y / B.Y) : 0.0f };
	}

	FORCEINLINE FVector operator+(const FVector& A, const FVector2D& B)
	{
		return { A.X + B.X, A.Y + B.Y, A.Z };
	}

	FORCEINLINE FVector operator-(const FVector& A, const FVector2D& B)
	{
		return { A.X - B.X, A.Y - B.Y, A.Z };
	}

	FORCEINLINE FVector& operator+=(FVector& A, const FVector2D& B)
	{
		A.X += B.X;
		A.Y += B.Y;
		return A;
	}

	FORCEINLINE FVector& operator-=(FVector& A, const FVector2D& B)
	{
		A.X -= B.X;
		A.Y -= B.Y;
		return A;
	}
}

// This allows anything that includes HeartMath.h to be able to use the above operator overload implicitly.
using namespace Heart::Math;