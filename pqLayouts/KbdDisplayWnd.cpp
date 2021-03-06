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

#include "stdafx.h"
#include "KbdDisplayWnd.h"

#include "../StaticLib1/util.h"


namespace
{
    const WCHAR CLASS_NAME[] = _T("KeyboardHelpDisplay");
    const int IMG_BORDER = 0;
}


ATOM KbdDisplayWnd::classAtom = NULL;


KbdDisplayWnd::KbdDisplayWnd(const HINSTANCE hInstance)
{
    RegisterMyClass(hInstance);
    CreateMyWindow(hInstance);
}

KbdDisplayWnd::~KbdDisplayWnd()
{
    ClearImage();
}

bool KbdDisplayWnd::RegisterMyClass(HINSTANCE hInstance)
{
    WNDCLASSEX wc;

    if (classAtom != NULL)
        return true;

    //Register the Window Class
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = StaticWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(KbdDisplayWnd*);
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(hInstance, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(hInstance, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>((COLOR_WINDOW + 1));
    wc.lpszMenuName  = nullptr;
    wc.lpszClassName = CLASS_NAME;
    wc.hIconSm       = LoadIcon(hInstance, IDI_APPLICATION);

    classAtom = RegisterClassEx(&wc);
    return classAtom != NULL ? true : false;
}

bool KbdDisplayWnd::CreateMyWindow(HINSTANCE hInstance)
{
    // Create the window.

    hWnd = CreateWindowEx(
        WS_EX_TOOLWINDOW,
        reinterpret_cast<LPCWSTR>(classAtom),
        L"pqLayouts kbd image", 
        WS_POPUP | WS_DLGFRAME,
        200,200, 
        300,150,
        nullptr,       // Parent window    
        nullptr,       // Menu
        hInstance,
        nullptr        // Additional application data
        );

    if (hWnd == nullptr)
    {
        return false;
    }

    // save 'this' in the window's user data (used by static wndProc to dispatch to KbdDisplayWnd)
    SetWindowLongPtrA(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    // 'show' window, hidden at 1st
    ShowWindow(hWnd, SW_HIDE);
    return true;
}


/// <summary>
/// forward window messages to the KbdDisplayWnd instance wndProc
/// </summary>
/// <param name="hWnd"></param>
/// <param name="msg"></param>
/// <param name="wParam"></param>
/// <param name="lParam"></param>
/// <returns></returns>
LRESULT CALLBACK KbdDisplayWnd::StaticWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // Get saved pointer to the KbdDisplayWnd of this window
    const auto windowData = GetWindowLongA(hWnd, GWL_USERDATA);
    if (windowData != 0)
    {
        auto *instance = reinterpret_cast<KbdDisplayWnd*>(windowData);
        return instance->WndProc(hWnd, msg, wParam, lParam);
    }

    //? should not happen
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK KbdDisplayWnd::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        auto* const hDc = BeginPaint(hWnd, &ps);
        OnPaint(hDc, ps);
        EndPaint(hWnd, &ps);
        break;
    }

    case WM_MOUSEMOVE:
        OnMouseMove();
        break;

    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}

/// <summary>
/// draw the image in the window
/// </summary>
/// <param name="hDc"></param>
/// <param name="ps"></param>
void KbdDisplayWnd::OnPaint(HDC hDc, PAINTSTRUCT& ps) const
{
    if (image != nullptr && imageView.Height() > 0)
    {
        Gdiplus::Graphics graphics(hDc);

        const int width = image->GetWidth();
        const int height = imageView.Height(); // image->GetHeight();

        graphics.DrawImage(
            image, IMG_BORDER,IMG_BORDER, 
            0, imageView.TopY, 
            width, height, 
            Gdiplus::UnitPixel);
    }
}

void KbdDisplayWnd::DisplayWindow(bool visible)
{
    if (image != nullptr && image->GetWidth() > 0 && image->GetHeight() > 0)
    {
        ShowWindow(hWnd, visible ? SW_SHOWNORMAL : SW_HIDE); 
    }
}

/// <summary>
/// Calculates the difference between the client area and the window size
/// </summary>
void KbdDisplayWnd::CalculateClientPadding()
{
    if (!clientPadIsCalculated)
    {
        clientPadIsCalculated = true;

        RECT clientRect, windowRect;
        GetWindowRect(hWnd, &windowRect);
        GetClientRect(hWnd, &clientRect);
        const int clientWidth  = clientRect.right - clientRect.left;
        const int clientHeight = clientRect.bottom - clientRect.top;
        const int windowWidth  = windowRect.right - windowRect.left;
        const int windowHeight = windowRect.bottom - windowRect.top;

        clientPadWidth = windowWidth - clientWidth;
        clientPadHeight = windowHeight - clientHeight;
    }
}

/// <summary>
/// Place window centered in monitor in X
/// and near top / bottom in Y according to displayAtTop
/// </summary>
void KbdDisplayWnd::PlaceWindow() const
{
    //PQ-TODO support multiple monitors (center window in monitor of current active window)
    //const auto screenCx = GetSystemMetrics(SM_CXSCREEN); 
    //const auto screenCy = GetSystemMetrics(SM_CYSCREEN);

    if (image != nullptr && image->GetWidth() > 0 && image->GetHeight() > 0)
    {
        RECT desktopRect;
        SystemParametersInfoA(SPI_GETWORKAREA, 0, &desktopRect, FALSE);
        const int screenCx = desktopRect.right - desktopRect.left;
        const int screenCy = desktopRect.bottom - desktopRect.top;

        RECT windowRect;
        GetWindowRect(hWnd, &windowRect);
        const int windowWidth  = windowRect.right - windowRect.left;
        const int windowHeight = windowRect.bottom - windowRect.top;

        // center window in X
        const int x = screenCx / 2 - windowWidth / 2;

        // place window near top or bottom in Y
        const int y = displayAtTop ? 5 : (screenCy - windowHeight - 5);

        // move window, setting it TOPMOST, then show it (is created hidden)
        SetWindowPos(hWnd, HWND_TOPMOST, x, y, 0, 0, SWP_NOSIZE);
        ShowWindow(hWnd, SW_SHOWNORMAL); 
    }
}

void KbdDisplayWnd::ResizeWindow()
{
    // calculate the size of the window to fit exactly around the image size (ie client size == img size)
    CalculateClientPadding();

    if (image != nullptr && image->GetWidth() > 0 && image->GetHeight() > 0)
    {
        const int imgWidth = static_cast<int>(image->GetWidth());
        const int imgHeight = imageView.Height(); // static_cast<int>(image->GetHeight());

        // img size + client pad = wnd size
        const int newWindowWidth = imgWidth + clientPadWidth + (2*IMG_BORDER);
        const int newWindowHeight = imgHeight + clientPadHeight + (2*IMG_BORDER);

        // resize the window
        SetWindowPos(hWnd, nullptr, 0, 0, newWindowWidth, newWindowHeight, SWP_NOMOVE | SWP_NOZORDER);
    }
}

bool KbdDisplayWnd::SetImageFile(const WCHAR* imageFilename)
{
    delete image;

    image = Gdiplus::Bitmap::FromFile(imageFilename);

    // update window size / position
    if (image != nullptr && image->GetWidth() > 0 && image->GetHeight() > 0)
    {
        ResizeWindow();
        PlaceWindow();
    }

    // redraw the window
    InvalidateRect(hWnd, nullptr, TRUE);

    return true;
}

void KbdDisplayWnd::ClearImage()
{
    delete image;
    image = nullptr;

    InvalidateRect(hWnd, nullptr, TRUE);
}

void KbdDisplayWnd::SetImageView(Layer::ImageView imgView)
{
    const int currentViewHeight = imageView.Height();

    // update image view
    imageView = imgView;
    Printf("KbdDisplayWnd::SetImageView top/bott %d-%d\n", imageView.TopY, imageView.BottomY);

    // resize window if height changed
    if (imgView.Height() != currentViewHeight)
    {
        ResizeWindow();
        PlaceWindow();
    }

    InvalidateRect(hWnd, nullptr, TRUE);
}

/// <summary>
/// Move the window out of the way when we get the mouse over it
/// </summary>
void KbdDisplayWnd::OnMouseMove()
{
    displayAtTop = !displayAtTop;
    PlaceWindow();
}

