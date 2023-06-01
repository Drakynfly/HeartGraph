// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Algorithms/Layout.h"

namespace Nodesoup
{
    void Circle(TArray<FVector2D>& Positions)
    {
        const double Angle = TWO_PI / Positions.Num();
        for (int32 i = 0; i < Positions.Num(); i++)
        {
            Positions[i].X = FMath::Cos(i * Angle);
            Positions[i].Y = FMath::Sin(i * Angle);
        }
    }

    void CenterAndScale(const uint32 Width, const uint32 Height, TArray<FVector2D>& Positions)
    {
        // find current dimensions
        double X_Min = TNumericLimits<double>::Max();
        double X_Max = TNumericLimits<double>::Lowest();
        double Y_Min = TNumericLimits<double>::Max();
        double Y_Max = TNumericLimits<double>::Lowest();

        for (int32 i = 0; i < Positions.Num(); i++)
        {
            if (Positions[i].X < X_Min)
            {
                X_Min = Positions[i].X;
            }
            if (Positions[i].X > X_Max)
            {
                X_Max = Positions[i].X;
            }

            if (Positions[i].Y < Y_Min)
            {
                Y_Min = Positions[i].Y;
            }
            if (Positions[i].Y > Y_Max)
            {
                Y_Max = Positions[i].Y;
            }
        }

        const double CurWidth = X_Max - X_Min;
        const double CurHeight = Y_Max - Y_Min;

        // compute scale factor (0.9: keep some margin)
        const double X_Scale = Width / CurWidth;
        const double Y_Scale = Height / CurHeight;
        const double Scale = 0.9 * (X_Scale < Y_Scale ? X_Scale : Y_Scale);

        // compute offset and apply it to every position
        const FVector2D Center = { X_Max + X_Min, Y_Max + Y_Min };
        const FVector2D Offset = Center / 2.0 * Scale;
        for (int32 i = 0; i < Positions.Num(); i++)
        {
            Positions[i] = Positions[i] * Scale - Offset;
        }
    }
}
