#include <iostream>
#include <thread>
#include <chrono>
#include <Windows.h>
#include <WinINet.h>
#include <fstream>
#include <ctime>
#include <string>
#include <vector>
#include <sstream>
#include <filesystem>

#include "memory/memory.hpp"
#include "classes/vector.hpp"
#include "hacks/reader.hpp"
#include "hacks/hack.hpp"
#include "classes/globals.hpp"
#include "classes/render.hpp"
#include "classes/offset.hpp"

constexpr int update_speed = 20;

std::atomic<bool> finish(false);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        g::hdcBuffer = CreateCompatibleDC(nullptr);
        g::hbmBuffer = CreateCompatibleBitmap(GetDC(hWnd), g::gameBounds.right, g::gameBounds.bottom);
        SelectObject(g::hdcBuffer, g::hbmBuffer);

        SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);

        SetLayeredWindowAttributes(hWnd, RGB(255, 255, 255), 0, LWA_COLORKEY);
        Beep(100, 100);
        break;
    }
    case WM_ERASEBKGND:
        return TRUE;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        FillRect(g::hdcBuffer, &ps.rcPaint, (HBRUSH)GetStockObject(WHITE_BRUSH));

        if (GetForegroundWindow() == g_game.process->hwnd_) {
            hack::loop();
        }

        BitBlt(hdc, 0, 0, g::gameBounds.right, g::gameBounds.bottom, g::hdcBuffer, 0, 0, SRCCOPY);

        EndPaint(hWnd, &ps);
        InvalidateRect(hWnd, nullptr, TRUE);
        break;
    }
    case WM_DESTROY:
        DeleteDC(g::hdcBuffer);
        DeleteObject(g::hbmBuffer);

        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void read_thread() {
    while (!finish) {
        if (GetForegroundWindow() == g_game.process->hwnd_) {
            g_game.loop();
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
}

int main() {
    SetConsoleTitle("easysp");

    g_game.init();

    while (GetForegroundWindow() != g_game.process->hwnd_) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        g_game.process->UpdateHWND();
        ShowWindow(g_game.process->hwnd_, TRUE);
    }

    WNDCLASSEXA wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.lpfnWndProc = WndProc;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.hbrBackground = WHITE_BRUSH;
    wc.hInstance = reinterpret_cast<HINSTANCE>(GetWindowLongA(g_game.process->hwnd_, (-6))); // GWL_HINSTANCE));
    wc.lpszMenuName = " ";
    wc.lpszClassName = " ";

    RegisterClassExA(&wc);

    GetClientRect(g_game.process->hwnd_, &g::gameBounds);

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to get console handle" << std::endl;
        return 1;
    }

    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);

    HINSTANCE hInstance = nullptr;
    HWND hWnd = CreateWindowExA(WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW, " ", "easysp", WS_POPUP,
        g::gameBounds.left, g::gameBounds.top, g::gameBounds.right - g::gameBounds.left, g::gameBounds.bottom + g::gameBounds.left, nullptr, nullptr, hInstance, nullptr);

    if (hWnd == nullptr)
        return 0;

    SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    ShowWindow(hWnd, TRUE);

    std::thread read(read_thread);

    std::cout << R"(
          F1 ESP | F2 Team

        ___            _______
       /XXX\          /       \
      |     |        | -       |
      | RIP |        |    /    |
      |     |        |   /     |
      | IFA |         \  __   /
      |_____|          \_____/

    )" << std::endl;

    /*
    const int C4 = 440;
    const int D4 = 294;
    const int E4 = 440;
    const int F4 = 294;
    const int G4 = 400;
    const int A4 = 244;
    const int B4 = 400;
    const int P4 = 244;

    const int quarter = 200;
    const int half = 2 * quarter;
    const int whole = 2 * half;

    std::vector<std::pair<int, int>> melody = {
        {C4, quarter}, {D4, quarter}, {E4, quarter}, {F4, quarter},
        {G4, quarter}, {A4, quarter}, {B4, quarter}, {C4, half},
        {P4, quarter},
    };

    for (auto note : melody) {
        Beep(note.first, note.second);
    }
    */

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0) && !finish)
    {
        if (GetAsyncKeyState(VK_F1) & 0x8000) { config::show_box_esp = !config::show_box_esp; config::save(); Beep(700, 100); };

        if (GetAsyncKeyState(VK_F2) & 0x8000) { config::team_esp = !config::team_esp; config::save(); Beep(700, 100); };

        if (GetAsyncKeyState(VK_END) & 0x8000) finish = true;

        TranslateMessage(&msg);
        DispatchMessage(&msg);

        std::this_thread::sleep_for(std::chrono::milliseconds(update_speed));
    }

    read.detach();

    DeleteDC(g::hdcBuffer);
    DeleteObject(g::hbmBuffer);

    DestroyWindow(hWnd);

    g_game.close();

    return 1;
}
