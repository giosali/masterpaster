#include "main.h"

#include <chrono>
#include <deque>
#include <stdexcept>
#include <tchar.h>
#include <thread>
#include <vector>
#include <Windows.h>
#include "clipboard.h"
#include "ini.h"
#include "io.h"
#include "keyboard.h"
#include "utils.h"

// Global Variables:
HINSTANCE g_hInst = NULL;
Clipboard g_clipboard;
ini::IniFile g_settingsIni;
Keyboard g_keyboard;
std::deque<const wchar_t*> g_clipboardItems;
const uint32_t g_notifyIconId = 1;
const UINT WMAPP_NOTIFYCALLBACK = WM_APP + 1;
static TCHAR szWindowClass[] = _T("masterPaster");
static TCHAR szTitle[] = _T("MasterPaster");

// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL addNotificationIcon(HWND&);
BOOL deleteNotificationIcon(HWND&);
void showContextMenu(HWND&, POINT&);
void editContextMenuItem(HMENU&, int, UINT, bool, const wchar_t* = L"");
bool tryInterpretVirtualKeyCode(unsigned);
void registerRunValue();
void deleteRunValue();

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    // Prevents multiple instances of this application from running.
    HANDLE hMutex = CreateMutexEx(NULL, szWindowClass, CREATE_MUTEX_INITIAL_OWNER, MUTEX_ALL_ACCESS);
    if (!hMutex) {
        return 1;
    }

    g_hInst = hInstance;

    WNDCLASSEX wcex = { sizeof(wcex) };
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);
    RegisterClassEx(&wcex);

    // Causes the application to look crisp on displays with high DPIs.
    // This must be called before creating any HWNDs.
    SetProcessDPIAware();

    // HWND_MESSAGE causes the window to be a message-only window.
    HWND hWnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 450, HWND_MESSAGE, NULL, hInstance, NULL);
    if (!hWnd) {
        return 1;
    }

    ShowWindow(hWnd, nCmdShow);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    ReleaseMutex(hMutex);
    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_CLIPBOARDUPDATE: {
        if (g_keyboard.getIsPasting()) {
            g_keyboard.setIsPasting(false);
            break;
        }

        if (g_clipboard.containsText()) {
            // This is necessary because we don't want to be the first to process
            // the WM_CLIPBOARDUPDATE message since it may bug out for certain
            // applications (such as Firefox and its Network Monitor). It's a very weird
            // bug and I'm not sure if I might be improperly retrieving the clipboard
            // data (I personally don't believe so, however).
            std::this_thread::sleep_for(std::chrono::milliseconds(50));

            const wchar_t* text = g_clipboard.getText();

            // Prevents duplicate copies by checking if the item to add is already at the
            // front of collection.
            if (g_clipboardItems.size() > 0 && wcscmp(g_clipboardItems[0], text) == 0) {
                break;
            }

            g_clipboardItems.push_front(text);

            // Ensures the collection contains at most 10 items at any given time.
            if (g_clipboardItems.size() > 10) {
                g_clipboardItems.pop_back();
            }
        }

        return 1;
        //break;
    }
    case WM_COMMAND: {
        int const wmId = LOWORD(wParam);
        switch (wmId) {
        case IDM_ENABLE:
            // Toggles the state of the keyboard hook.
            g_keyboard.setIsEnabled(!g_keyboard.getIsEnabled());
            break;
        case IDM_RUNATSTARTUP: {
            bool runAtStartup = !g_settingsIni.get<bool>(io::settings::runAtStartup).value().boolean;
            g_settingsIni.set(io::settings::runAtStartup, runAtStartup);
            g_settingsIni.save();
            if (runAtStartup) {
                registerRunValue();
            } else {
                deleteRunValue();
            }

            break;
        }
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }

        break;
    }
    case WM_CREATE:
        if (!addNotificationIcon(hWnd) || !AddClipboardFormatListener(hWnd)) {
            return -1;
        }

        g_settingsIni = ini::IniFile(io::getSettingsIniPath());
        if (!g_settingsIni.exists()) {
            std::string text = io::getIniResource(IDR_SETTINGSINI);
            g_settingsIni.load(text);
            if (g_settingsIni.get<bool>(io::settings::runAtStartup).value().boolean) {
                registerRunValue();
            }
        }

        g_keyboard = Keyboard(tryInterpretVirtualKeyCode);
        g_keyboard.addHook(g_hInst);

        g_clipboard = Clipboard(hWnd);
        break;
    case WM_DESTROY:
        deleteNotificationIcon(hWnd);
        RemoveClipboardFormatListener(hWnd);
        g_keyboard.removeHook();
        PostQuitMessage(0);
        break;
    case WMAPP_NOTIFYCALLBACK:
        switch (LOWORD(lParam)) {
        case WM_CONTEXTMENU:
            POINT pt;

            if (GetCursorPos(&pt)) {
                showContextMenu(hWnd, pt);
            }

            break;
        }

        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

BOOL addNotificationIcon(HWND& hWnd)
{
    NOTIFYICONDATA nid = { sizeof(nid) };
    nid.hWnd = hWnd;
    nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE | NIF_SHOWTIP;
    nid.uID = g_notifyIconId;
    nid.uCallbackMessage = WMAPP_NOTIFYCALLBACK;
    nid.hIcon = (HICON)LoadImage(g_hInst, MAKEINTRESOURCE(IDI_NOTIFICATIONICON), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR | LR_DEFAULTSIZE);
    wcscpy_s(nid.szTip, L"MasterPaster v1.0.2");
    Shell_NotifyIcon(NIM_ADD, &nid);

    // NOTIFYICON_VERSION_4 is preferred
    nid.uVersion = NOTIFYICON_VERSION_4;
    return Shell_NotifyIcon(NIM_SETVERSION, &nid);
}

BOOL deleteNotificationIcon(HWND& hWnd)
{
    NOTIFYICONDATA nid = { sizeof(nid) };
    nid.hWnd = hWnd;
    nid.uID = g_notifyIconId;
    return Shell_NotifyIcon(NIM_DELETE, &nid);
}

void showContextMenu(HWND& hWnd, POINT& pt)
{
    HMENU hMenu = LoadMenu(g_hInst, MAKEINTRESOURCE(IDR_NOTIFICATIONICONMENU));
    if (hMenu) {
        HMENU hSubMenu = GetSubMenu(hMenu, 0);
        if (hSubMenu) {
            std::wstring ws = utils::stringToWString(g_keyboard.getIsEnabled() ? "Disable" : "Enable");
            editContextMenuItem(hMenu, IDM_ENABLE, MIIM_STRING | MIIM_DATA, false, ws.c_str());
            editContextMenuItem(hMenu, IDM_RUNATSTARTUP, MIIM_STATE, g_settingsIni.get<bool>(io::settings::runAtStartup).value().boolean);

            // The window must be the foreground window before calling TrackPopupMenu
            // or the menu will not disappear when the user clicks away.
            SetForegroundWindow(hWnd);

            // Respects menu drop alignment and returns menu item id.
            UINT uFlags = TPM_RIGHTBUTTON;
            if (GetSystemMetrics(SM_MENUDROPALIGNMENT) != 0) {
                uFlags |= TPM_RIGHTALIGN;
            } else {
                uFlags |= TPM_LEFTALIGN;
            }

            TrackPopupMenuEx(hSubMenu, uFlags, pt.x, pt.y, hWnd, NULL);
        }

        DestroyMenu(hMenu);
    }
}

void editContextMenuItem(HMENU& hMenu, int idm, UINT fMask, bool check, const wchar_t* caption)
{
    MENUITEMINFO mii = { sizeof(MENUITEMINFO) };
    mii.fMask = fMask;
    GetMenuItemInfo(hMenu, idm, FALSE, &mii);

    if ((fMask & MIIM_STATE) == MIIM_STATE) {
        mii.fState = check ? MFS_CHECKED : MFS_UNCHECKED;
    }

    if ((fMask & MIIM_STRING) == MIIM_STRING) {
        mii.dwTypeData = const_cast<LPWSTR>(caption);
    }

    SetMenuItemInfo(hMenu, idm, FALSE, &mii);
}

bool tryInterpretVirtualKeyCode(unsigned vkCode)
{
    // Continues if the virtual-key code matches the 0-9 keys.
    if (vkCode >= 0x30 && vkCode <= 0x39) {
        const int base = 0x30;
        int diff;

        // Assigns 10 to the index if the '0' key was pressed.
        int index = (diff = vkCode - base) ? diff : 10;

        if (g_clipboardItems.size() < index) {
            return false;
        }

        g_keyboard.setIsPasting(true);
        const wchar_t* text = g_clipboardItems.at(--index);
        g_clipboard.setText(text);
        int vks[] = { 'V' };
        Keyboard::sendKeyboardInput(vks, sizeof(vks) / sizeof(*vks));
        return true;
    }

    return false;
}

void registerRunValue()
{
    wchar_t buffer[MAX_PATH];
    GetModuleFileName(NULL, buffer, MAX_PATH);
    std::wstring fp(buffer);
    HKEY hKey;
    LONG retval = RegOpenKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_WRITE, &hKey);
    if (retval == ERROR_SUCCESS) {
        RegSetValueEx(hKey, szTitle, 0, REG_SZ, (BYTE*)fp.c_str(), (unsigned)((fp.size() + 1) * sizeof(wchar_t)));
    }

}

void deleteRunValue()
{
    HKEY hKey;
    RegOpenKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_ALL_ACCESS, &hKey);
    RegDeleteValue(hKey, szTitle);
    RegCloseKey(hKey);
}