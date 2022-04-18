#include "clipboard.h"

Clipboard::Clipboard()
    : m_hWnd(NULL)
{
}

Clipboard::Clipboard(const HWND& hWnd)
    : m_hWnd(hWnd)
{
}

bool Clipboard::containsText() const
{
    return IsClipboardFormatAvailable(CF_UNICODETEXT);
}

wchar_t* Clipboard::getText() const
{
    wchar_t* text {};
    while (true) {
        if (OpenClipboard(m_hWnd)) {
            HGLOBAL hglb = GetClipboardData(CF_UNICODETEXT);
            if (hglb) {
                const wchar_t* lockedText = (const wchar_t*)GlobalLock(hglb);
                if (lockedText) {
                    const size_t size = GlobalSize(hglb);
                    text = (wchar_t*)malloc(size);
                    if (text) {
                        wcscpy_s(text, size / sizeof(wchar_t), lockedText);
                    }
                }


                GlobalUnlock(hglb);
            }

            CloseClipboard();
            break;
        } else if (GetLastError() != ERROR_ACCESS_DENIED) {
            break;
        }
    }

    return text;
}

void Clipboard::setText(const wchar_t* text) const
{
    while (true) {
        if (OpenClipboard(m_hWnd)) {
            const size_t textLen = wcslen(text);
            HGLOBAL hMem = GlobalAlloc(GHND, (textLen + 1) * sizeof(wchar_t));
            if (hMem) {
                wchar_t* lockedText = (wchar_t*)GlobalLock(hMem);
                if (lockedText) {
                    wcscpy_s(lockedText, textLen + 1, text);
                }

                GlobalUnlock(hMem);
                EmptyClipboard();
                SetClipboardData(CF_UNICODETEXT, hMem);
            }

            CloseClipboard();
            break;
        } else {
            DWORD dw = GetLastError();
            if (dw != ERROR_ACCESS_DENIED) {
                break;
            }
        }
    }
}
