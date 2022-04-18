#pragma once

#include <Windows.h>

class Clipboard {
public:
    Clipboard();
    explicit Clipboard(const HWND& hWnd);

    bool containsText() const;
    wchar_t* getText() const;
    void setText(const wchar_t*) const;

private:
    HWND m_hWnd;
};
