// Copyright 2021 Philippe Quesnel  
//
// This file is part of pqLayouts.
//
// pqLayouts is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// pqLayouts is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with pqLayouts.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include "layer.h"

class KbdDisplayWnd
{
public:
    KbdDisplayWnd(HINSTANCE hInstance);
    // window destruction vs object destruction is NOT handled !!
    // for now we expect the window to exist as long as the app is running
    ~KbdDisplayWnd();

    // sequence: GdiplusStartup, ShowImage, ClearImage, GdiplusShutdown
    bool SetImageFile(const WCHAR*);
    void ClearImage();

    void SetImageView(Layer::ImageView);


private:

    bool CreateMyWindow(HINSTANCE hInstance);
    void OnPaint(HDC, PAINTSTRUCT&) const;
    void OnMouseMove();
    void PlaceWindow() const;
    void CalculateClientPadding();
    void ResizeWindow();
    LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    static bool RegisterMyClass(HINSTANCE hInstance);
    static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
    HWND hWnd = nullptr;
    Gdiplus::Bitmap* image = nullptr;
    Layer::ImageView imageView;
    bool displayAtTop = false;

    bool clientPadIsCalculated = false;
    int clientPadWidth = 0;
    int clientPadHeight = 0;

    static ATOM classAtom;
};

