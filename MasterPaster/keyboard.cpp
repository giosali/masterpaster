#include "keyboard.h"

Keyboard::Keyboard()
    : m_hookId(NULL)
    , m_isPasting(false)
{
}

Keyboard::Keyboard(const std::function<bool(unsigned)>& func)
    : m_hookId(NULL)
    , m_isPasting(false)
{
    s_func = func;
}

void Keyboard::sendKeyboardInput(int vks[], int len)
{
    std::vector<INPUT> inputs(len * 2);
    for (int i = 0; i < len; i++) {
        INPUT input {};
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = vks[i];
        inputs[i] = input;

        input.ki.dwFlags = KEYEVENTF_KEYUP;
        int equivlent = i + len;
        inputs[equivlent] = input;
    }

    SendInput(len * 2, inputs.data(), sizeof(INPUT));
}

void Keyboard::removeHook() const
{
    UnhookWindowsHookEx(m_hookId);
}

void Keyboard::addHook(HINSTANCE& hInstance)
{
    m_hookId = SetWindowsHookEx(WH_KEYBOARD_LL, hookCallBack, hInstance, 0);
}

bool Keyboard::getIsPasting() const
{
    return m_isPasting;
}

void Keyboard::setIsPasting(bool value)
{
    m_isPasting = value;
}

bool Keyboard::getIsEnabled() const
{
    return s_isEnabled;
}

void Keyboard::setIsEnabled(bool value)
{
    s_isEnabled = value;
}

LRESULT CALLBACK Keyboard::hookCallBack(_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
    if (nCode >= 0 && s_isEnabled) {
        switch (wParam) {
        case WM_KEYDOWN:
            KBDLLHOOKSTRUCT* kbStruct = (KBDLLHOOKSTRUCT*)lParam;
            int vkCode = kbStruct->vkCode;
            if (s_isControlDown && s_func(vkCode)) {
                s_isControlDown = false;
                return 1;
            }

            switch (vkCode) {
            case VK_LCONTROL:
            case VK_RCONTROL:
                if (!s_isControlDown) {
                    s_isControlDown = true;
                }

                break;
            default:
                if (s_isControlDown) {
                    s_isControlDown = false;
                }

                break;
            }

            break;
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

std::function<bool(unsigned)> Keyboard::s_func = {};

bool Keyboard::s_isControlDown = false;

bool Keyboard::s_isEnabled = true;