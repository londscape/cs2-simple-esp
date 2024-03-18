#pragma once
#include <Windows.h>

namespace render
{
    void Box(HDC hdc, int x, int y, int w, int h, COLORREF borderColor, int borderWidth)
    {
        HPEN hBorderPen = CreatePen(PS_SOLID, borderWidth, borderColor);
        if (hBorderPen == nullptr)
        {
            return;
        }

        HGDIOBJ hOldPen = SelectObject(hdc, hBorderPen);
        if (hOldPen == nullptr)
        {
            DeleteObject(hBorderPen); 
            return;
        }

        Rectangle(hdc, x, y, x + w, y + h);

        if (SelectObject(hdc, hOldPen) == nullptr)
        {
            DeleteObject(hBorderPen);
            return;
        }

        if (!DeleteObject(hBorderPen))
        {
            std::cout << "Error [1]" << std::endl;
        }
    }
}
