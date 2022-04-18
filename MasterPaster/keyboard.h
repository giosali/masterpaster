#pragma once

#include <functional>
#include <Windows.h>

class Keyboard {
public:
    Keyboard();
    explicit Keyboard(const std::function<bool(unsigned)>&);

    static void sendKeyboardInput(int[], int);
    void removeHook() const;
    void addHook(HINSTANCE&);
    bool getIsPasting() const;
    void setIsPasting(bool);
    bool getIsEnabled() const;
    void setIsEnabled(bool);

private:
    static LRESULT CALLBACK hookCallBack(_In_ int, _In_ WPARAM, _In_ LPARAM);

    static std::function<bool(unsigned)> s_func;
    static bool s_isControlDown;
    static bool s_isEnabled;

    HHOOK m_hookId;
    bool m_isPasting;
};
