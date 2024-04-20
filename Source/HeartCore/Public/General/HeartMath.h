// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

namespace Heart::Math
{
	template <typename T>
	FORCEINLINE UE::Math::TVector2<T> SafeDivide(const UE::Math::TVector2<T>& A, const T& Scale)
	{
		const T RScale = static_cast<T>(Scale != 0.0 ? 1.0/Scale : 1.0);
		return UE::Math::TVector2<T>(A.X * RScale, A.Y * RScale);
	}

	template <typename T>
	FORCEINLINE UE::Math::TVector2<T> SafeDivide(const UE::Math::TVector2<T>& A, const UE::Math::TVector2<T>& B)
	{
		return { (B.X != 0.0) ? (A.X / B.X) : 0.0, (B.Y != 0.0) ? (A.Y / B.Y) : 0.0 };
	}

	template <typename T>
	FORCEINLINE UE::Math::TVector<T> operator+(const UE::Math::TVector<T>& A, const UE::Math::TVector2<T>& B)
	{
		return { A.X + B.X, A.Y + B.Y, A.Z };
	}

	template <typename T, typename U>
	FORCEINLINE UE::Math::TVector<T> operator-(const UE::Math::TVector<T>& A, const UE::Math::TVector2<U>& B)
	{
		return { A.X - B.X, A.Y - B.Y, A.Z };
	}

	template <typename T, typename U>
	FORCEINLINE UE::Math::TVector<T>& operator+=(UE::Math::TVector<T>& A, const UE::Math::TVector2<U>& B)
	{
		A.X += B.X;
		A.Y += B.Y;
		return A;
	}

	template <typename T, typename U>
	FORCEINLINE UE::Math::TVector<T>& operator-=(UE::Math::TVector<T>& A, const UE::Math::TVector2<U>& B)
	{
		A.X -= B.X;
		A.Y -= B.Y;
		return A;
	}

	template <typename T, typename U>
	FORCEINLINE UE::Math::TVector2<T>& operator+=(UE::Math::TVector2<T>& A, const UE::Math::TVector<U>& B)
	{
		A.X += static_cast<T>(B.X);
		A.Y += static_cast<T>(B.Y);
		return A;
	}

	template <typename T, typename U>
	FORCEINLINE UE::Math::TVector2<T>& operator-=(UE::Math::TVector2<T>& A, const UE::Math::TVector<U>& B)
	{
		A.X -= B.X;
		A.Y -= B.Y;
		return A;
	}

	FORCEINLINE FVector2f Vector2fInterpTo( const FVector2f& Current, const FVector2f& Target, const float DeltaTime, const float InterpSpeed )
	{
		if ( InterpSpeed <= 0.f )
		{
			return Target;
		}

		const FVector2f Dist = Target - Current;
		if ( Dist.SizeSquared() < UE_KINDA_SMALL_NUMBER )
		{
			return Target;
		}

		const FVector2f DeltaMove = Dist * FMath::Clamp<float>(DeltaTime * InterpSpeed, 0.f, 1.f);
		return Current + DeltaMove;
	}
}

// This allows anything that includes HeartMath.h to be able to use the above operator overloads implicitly.
using namespace Heart::Math;