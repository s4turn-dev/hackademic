#include <windows.h>
#include "hook.h"

HHOOK hHook = nullptr;

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT *p = (KBDLLHOOKSTRUCT *)lParam;

        // ALT+TAB, ALT+F4, CTRL+ESC, WIN
        bool altPressed = GetAsyncKeyState(VK_MENU) & 0x8000;
        bool ctrlPressed = GetAsyncKeyState(VK_CONTROL) & 0x8000;

        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            switch (p->vkCode) {
                case VK_TAB:
                    if (altPressed) return 1;
                    break;
                case VK_F4:
                    if (altPressed) return 1;
                    break;
                case VK_ESCAPE:
                    if (ctrlPressed) return 1;
                    break;
                case VK_LWIN:
                case VK_RWIN:
                    return 1;
            }
        }
    }

    return CallNextHookEx(hHook, nCode, wParam, lParam);
}

void installKeyboardHook() {
    hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, nullptr, 0);
}

void removeKeyboardHook() {
    if (hHook) UnhookWindowsHookEx(hHook);
}
