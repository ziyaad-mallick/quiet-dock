// ==WindhawkMod==
// @id              quiet-dock-reveal
// @name            Quiet Dock Reveal
// @description     Auto-hide taskbar that appears instantly with a smooth custom animation, and hides a set delay after the cursor leaves
// @version         2.2
// @author          ziyaad-mallick
// @github          https://github.com/ziyaad-mallick
// @homepage        https://github.com/ziyaad-mallick/quiet-dock
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lversion -ldwmapi -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Quiet Dock Reveal

A fork of [Taskbar Auto-Hide Instant Show](https://windhawk.net/mods/taskbar-autohide-instant-show)
by [@Bo0ii](https://github.com/Bo0ii), part of
[Quiet Dock](https://github.com/ziyaad-mallick/quiet-dock). Changes:

* **Frames sync to the display.** The custom animations paced frames with
  `Sleep()`, whose 15.6ms granularity turned 60fps into uneven 30ms/2ms
  pairs. Each frame now waits for the compositor (`DwmFlush`), and progress
  comes from the clock, so a late frame catches up instead of stretching the
  animation.
* **No stall when the taskbar appears.** The show path hid the taskbar by
  toggling `WS_EX_LAYERED` around Windows' own slide call, which makes DWM
  rebuild the window's surface (40-60ms at the start of every reveal). Now
  Windows' window move is turned into a no-op instead.
* **Hide delay counts from when the cursor leaves.** Windows hides from a
  repeating timer armed when the taskbar appears, so the hide landed anywhere
  within one period. The timer is now restarted by mouse movement over the
  taskbar.
* **Overshoot drops cleanly.** It springs on the way up only; the backward
  pull on the way down read as a hitch.

Windows still keeps the taskbar up while a window preview is open.

---

# Taskbar Auto-Hide Instant Show

Removes the built-in delay before the auto-hidden taskbar starts appearing.
The animation triggers instantly when your mouse touches the screen edge.

## Animation Types

### No animation
Taskbar snaps to final position instantly — no slide, no fade.

### Slide (default)
Default Windows slide, but sped up. Control speed with the speedup settings.

### Elastic (spring overshoot)
The taskbar overshoots its target and oscillates like a spring.

![Elastic](https://raw.githubusercontent.com/Bo0ii/windhawk-mods/main/taskbar-autohide-instant-show/elastic.gif)

### Bounce
The taskbar bounces at its final position.

### Fade (opacity)
The taskbar fades in/out in place.

![Fade](https://raw.githubusercontent.com/Bo0ii/windhawk-mods/main/taskbar-autohide-instant-show/Fade.gif)

### Slide + Fade (silky smooth)
Combines sliding with a fade effect using a quintic ease — the silkiest feel.

![Slide + Fade](https://raw.githubusercontent.com/Bo0ii/windhawk-mods/main/taskbar-autohide-instant-show/fade-slide.gif)

### Overshoot
Subtle spring past the target, then settles back.

## Settings

For **Slide** mode, use *Show/Hide speedup (%)* to control speed.
For all other modes, use *Show/Hide duration (ms)* to control timing.
*Frame rate* applies to all animation types.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- animationType: slide
  $name: Animation type
  $description: How the taskbar appears and disappears
  $options:
    - none: No animation (instant)
    - slide: Slide (default with speedup)
    - elastic: Elastic (spring overshoot)
    - bounce: Bounce
    - fade: Fade (opacity)
    - slideFade: Slide + Fade (silky smooth)
    - overshoot: Overshoot (subtle spring)
- showSpeedup: 400
  $name: Show speedup % (Slide mode only)
  $description: >-
    How fast the show slide animation plays. 100 = normal Windows speed,
    200 = 2x faster, 400 = 4x faster. Only used for Slide animation type.
- hideSpeedup: 400
  $name: Hide speedup % (Slide mode only)
  $description: >-
    How fast the hide slide animation plays. 100 = normal Windows speed,
    200 = 2x faster, 400 = 4x faster. Only used for Slide animation type.
- showDuration: 200
  $name: Show duration in ms (Elastic/Bounce/Fade modes)
  $description: >-
    Duration of the show animation in milliseconds for non-Slide animation
    types. Lower = faster. Try 150-300.
- hideDuration: 200
  $name: Hide duration in ms (Elastic/Bounce/Fade modes)
  $description: >-
    Duration of the hide animation in milliseconds for non-Slide animation
    types. Lower = faster. Try 150-300.
- frameRate: 60
  $name: Animation frame rate
  $description: >-
    Target frame rate for all animations. Higher = smoother.
    60 is good for most displays, 120 for high refresh rate monitors.
- unhideDelay: 1
  $name: Unhide delay in ms
  $description: >-
    Delay before the taskbar starts appearing when you move the mouse to
    the screen edge. Default Windows value is 50. Set to 1 for instant.
- hideDelay: 1
  $name: Hide delay in ms
  $description: >-
    Delay before the taskbar starts hiding after the mouse leaves.
    Default Windows value is 500. Set to 1 for instant.
- oldTaskbarOnWin11: false
  $name: Use old taskbar on Windows 11
  $description: >-
    Enable this if you use ExplorerPatcher or similar to use the old
    Windows 10 taskbar on Windows 11.
- edgeDetection: false
  $name: Edge detection for empty monitors
  $description: >-
    Enable this if the taskbar does not appear when hovering the screen
    edge on a monitor with no visible windows, or when a fullscreen app
    swallows the mouse-edge event. Creates a background thread that polls
    the cursor position.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <dwmapi.h>
#include <math.h>
#include <psapi.h>

#include <algorithm>
#include <atomic>
#include <map>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct {
    int animationType;
    int showSpeedup;
    int hideSpeedup;
    int showDuration;
    int hideDuration;
    int frameRate;
    int unhideDelay;
    int hideDelay;
    bool oldTaskbarOnWin11;
    bool edgeDetection;
} g_settings;

enum class WinVersion {
    Unsupported,
    Win10,
    Win11,
    Win11_24H2,
};

WinVersion g_winVersion;

std::atomic<bool> g_initialized;
std::atomic<bool> g_explorerPatcherInitialized;

double g_recipCyclesPerSecond;

std::atomic<DWORD> g_slideWindowThreadId;
int g_slideWindowSpeedup;
double g_slideWindowStartTime;
double g_slideWindowLastFrameStartTime;
std::atomic<int> g_animationGeneration{0};

// While Windows' own show call runs, its window move is turned into a no-op
// so the custom animation can start from where the taskbar is.
DWORD g_suppressMoveThread = 0;
HWND g_suppressMoveWindow = nullptr;

// Windows hides the auto-hidden taskbar from a repeating "is the cursor
// outside?" timer (id 2) armed when it appears, so the hide lands anywhere
// within one period of the cursor leaving. Re-arming that timer on every
// mouse move over the taskbar makes the delay count from when it leaves.
constexpr UINT_PTR kHideTimerId = 2;
std::map<HWND, TIMERPROC> g_hideTimers;  // taskbar thread only
HHOOK g_mouseHook = nullptr;

void TimerInitialize() {
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    double cyclesPerSecond = static_cast<double>(freq.QuadPart);
    g_recipCyclesPerSecond = 1.0 / cyclesPerSecond;
}

double TimerGetCycles() {
    LARGE_INTEGER T1;
    QueryPerformanceCounter(&T1);
    return static_cast<double>(T1.QuadPart);
}

double TimerGetSeconds() {
    return TimerGetCycles() * g_recipCyclesPerSecond;
}

double EaseOutElastic(double t) {
    if (t <= 0.0) return 0.0;
    if (t >= 1.0) return 1.0;
    double p = 0.3;
    double s = p / 4.0;
    return pow(2.0, -10.0 * t) * sin((t - s) * (2.0 * M_PI) / p) + 1.0;
}

double EaseOutBounce(double t) {
    if (t < 1.0 / 2.75) {
        return 7.5625 * t * t;
    } else if (t < 2.0 / 2.75) {
        t -= 1.5 / 2.75;
        return 7.5625 * t * t + 0.75;
    } else if (t < 2.5 / 2.75) {
        t -= 2.25 / 2.75;
        return 7.5625 * t * t + 0.9375;
    } else {
        t -= 2.625 / 2.75;
        return 7.5625 * t * t + 0.984375;
    }
}

double EaseOutCubic(double t) {
    double t1 = t - 1.0;
    return t1 * t1 * t1 + 1.0;
}

double EaseInCubic(double t) {
    return t * t * t;
}

double EaseOutQuint(double t) {
    double t1 = 1.0 - t;
    return 1.0 - t1 * t1 * t1 * t1 * t1;
}

double EaseInQuint(double t) {
    return t * t * t * t * t;
}

double EaseOutBack(double t) {
    const double c1 = 1.70158;
    const double c3 = c1 + 1.0;
    double t1 = t - 1.0;
    return 1.0 + c3 * t1 * t1 * t1 + c1 * t1 * t1;
}

double EaseInBack(double t) {
    const double c1 = 1.70158;
    const double c3 = c1 + 1.0;
    return c3 * t * t * t - c1 * t * t;
}

using GetTickCount_t = decltype(&GetTickCount);
GetTickCount_t GetTickCount_Original;
DWORD WINAPI GetTickCount_Hook() {
    if (g_slideWindowThreadId != GetCurrentThreadId()) {
        return GetTickCount_Original();
    }

    DWORD ms = (DWORD)((g_slideWindowLastFrameStartTime -
                         g_slideWindowStartTime) *
                            1000.0 * (g_slideWindowSpeedup / 100.0) +
                        0.5);

    Wh_Log(L"> %u", ms);

    return ms;
}

using Sleep_t = decltype(&Sleep);
Sleep_t Sleep_Original;
void WINAPI Sleep_Hook(DWORD dwMilliseconds) {
    if (g_slideWindowThreadId != GetCurrentThreadId()) {
        Sleep_Original(dwMilliseconds);
        return;
    }

    double frameTotalTime = 1000.0 / g_settings.frameRate;

    double frameElapsedTime =
        (TimerGetSeconds() - g_slideWindowLastFrameStartTime) * 1000.0;

    int sleepTime = (int)(frameTotalTime - frameElapsedTime + 0.5);

    Wh_Log(L"> %f - %f = %d", frameTotalTime, frameElapsedTime, sleepTime);

    if (sleepTime > 0) {
        Sleep_Original(sleepTime);
    }

    g_slideWindowLastFrameStartTime = TimerGetSeconds();
}

static int Lerp(int a, int b, double t) {
    return a + (int)((b - a) * t);
}

void DoCustomAnimation(HWND hWnd,
                       const RECT* startRect,
                       const RECT* endRect,
                       bool show,
                       int myAnimGen) {
    int animType = g_settings.animationType;
    double duration = (show ? g_settings.showDuration : g_settings.hideDuration) / 1000.0;

    auto moveTo = [&](int x, int y, int w, int h) {
        SetWindowPos(hWnd, NULL, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);
    };

    if (duration <= 0) {
        moveTo(endRect->left, endRect->top, endRect->right - endRect->left,
               endRect->bottom - endRect->top);
        return;
    }

    bool usePosition = (animType == 1 || animType == 2 || animType == 4 ||
                        animType == 5);
    bool useFade = (animType == 3 || animType == 4);

    LONG_PTR originalExStyle = 0;
    if (useFade) {
        originalExStyle = GetWindowLongPtr(hWnd, GWL_EXSTYLE);
        if (!(originalExStyle & WS_EX_LAYERED)) {
            SetWindowLongPtr(hWnd, GWL_EXSTYLE,
                             originalExStyle | WS_EX_LAYERED);
        }
        SetLayeredWindowAttributes(hWnd, 0, show ? 0 : 255, LWA_ALPHA);
        DwmFlush();
    }

    if (useFade && !usePosition && show) {
        moveTo(endRect->left, endRect->top, endRect->right - endRect->left,
               endRect->bottom - endRect->top);
    }

    // Progress comes from the clock, not a frame count, and every frame waits
    // for the compositor: a late frame then skips ahead instead of stretching
    // the animation, and frames land on the display's refresh instead of on
    // Sleep()'s 15.6ms granularity.
    double startTime = TimerGetSeconds();
    for (;;) {
        if (g_animationGeneration.load() != myAnimGen) {
            break;
        }

        double t = std::min(1.0, (TimerGetSeconds() - startTime) / duration);

        double posT = t;
        double alphaT = t;

        if (show) {
            switch (animType) {
                case 1:
                    posT = EaseOutElastic(t);
                    break;
                case 2:
                    posT = EaseOutBounce(t);
                    break;
                case 3:
                    alphaT = EaseOutCubic(t);
                    break;
                case 4:
                    posT = EaseOutQuint(t);
                    alphaT = EaseOutQuint(t);
                    break;
                case 5:
                    posT = EaseOutBack(t);
                    break;
            }
        } else {
            switch (animType) {
                case 1:
                case 2:
                case 5:
                    // Overshoot springs on the way up only; on the way down a
                    // backward pull reads as a hitch.
                    posT = EaseInCubic(t);
                    break;
                case 3:
                    alphaT = EaseInCubic(t);
                    break;
                case 4:
                    posT = EaseInQuint(t);
                    alphaT = EaseInQuint(t);
                    break;
            }
        }

        if (usePosition) {
            moveTo(Lerp(startRect->left, endRect->left, posT),
                   Lerp(startRect->top, endRect->top, posT),
                   Lerp(startRect->right - startRect->left,
                        endRect->right - endRect->left, posT),
                   Lerp(startRect->bottom - startRect->top,
                        endRect->bottom - endRect->top, posT));
        }

        if (useFade) {
            BYTE alpha = (BYTE)(255.0 * (show ? alphaT : 1.0 - alphaT));
            if (alpha < 1) alpha = 1;
            SetLayeredWindowAttributes(hWnd, 0, alpha, LWA_ALPHA);
        }

        if (t >= 1.0) {
            break;
        }

        if (FAILED(DwmFlush())) {
            Sleep_Original(1);
        }
    }

    if (usePosition) {
        moveTo(endRect->left, endRect->top, endRect->right - endRect->left,
               endRect->bottom - endRect->top);
    }

    if (useFade) {
        if (!show) {
            SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
            DwmFlush();
            moveTo(endRect->left, endRect->top, endRect->right - endRect->left,
                   endRect->bottom - endRect->top);
            DwmFlush();
        }

        // Remove WS_EX_LAYERED while window is either:
        //   - off-screen (hide) — no flash possible
        //   - at full alpha (show) — seamless transition
        if (!(originalExStyle & WS_EX_LAYERED)) {
            SetWindowLongPtr(hWnd, GWL_EXSTYLE, originalExStyle);
            DwmFlush();
        }

        // Only restore full alpha if window was already layered
        if (show && (originalExStyle & WS_EX_LAYERED)) {
            SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);
        }
    }
}

using TrayUI_SlideWindow_t = void(WINAPI*)(void* pThis,
                                           HWND hWnd,
                                           const RECT* rect,
                                           HMONITOR monitor,
                                           bool show,
                                           bool animate);
TrayUI_SlideWindow_t TrayUI_SlideWindow_Original;
void WINAPI TrayUI_SlideWindow_Hook(void* pThis,
                                    HWND hWnd,
                                    const RECT* rect,
                                    HMONITOR monitor,
                                    bool show,
                                    bool animate) {
    Wh_Log(L"> show=%d animType=%d", show, g_settings.animationType);

    if (g_settings.animationType == -1) {
        // No animation — snap to final position instantly.
        TrayUI_SlideWindow_Original(pThis, hWnd, rect, monitor, show, false);
    } else if (g_settings.animationType == 0) {
        g_slideWindowSpeedup =
            show ? g_settings.showSpeedup : g_settings.hideSpeedup;
        g_slideWindowStartTime = TimerGetSeconds();
        g_slideWindowLastFrameStartTime = g_slideWindowStartTime;
        g_slideWindowThreadId = GetCurrentThreadId();

        TrayUI_SlideWindow_Original(pThis, hWnd, rect, monitor, show, animate);

        g_slideWindowThreadId = 0;
    } else {
        int animGen = ++g_animationGeneration;

        RECT startRect;
        GetWindowRect(hWnd, &startRect);

        if (show) {
            // Let the original update TrayUI's state with its window move
            // turned into a no-op, then animate the move ourselves. (Hiding
            // the window and moving it back instead means toggling
            // WS_EX_LAYERED, which makes DWM rebuild the taskbar's surface:
            // a 40-60ms stall at the start of every reveal.)
            g_suppressMoveThread = GetCurrentThreadId();
            g_suppressMoveWindow = hWnd;
            TrayUI_SlideWindow_Original(pThis, hWnd, rect, monitor, show,
                                        false);
            g_suppressMoveWindow = nullptr;
            g_suppressMoveThread = 0;

            DoCustomAnimation(hWnd, &startRect, rect, show, animGen);
        } else {
            DoCustomAnimation(hWnd, &startRect, rect, show, animGen);

            TrayUI_SlideWindow_Original(pThis, hWnd, rect, monitor, show,
                                        false);
        }
    }
}

using SetTimer_t = decltype(&SetTimer);
SetTimer_t SetTimer_Original;

using SetWindowPos_t = decltype(&SetWindowPos);
SetWindowPos_t SetWindowPos_Original;
BOOL WINAPI SetWindowPos_Hook(HWND h, HWND after, int x, int y, int cx, int cy, UINT f) {
    if (h == g_suppressMoveWindow && GetCurrentThreadId() == g_suppressMoveThread) {
        f |= SWP_NOMOVE;
    }
    return SetWindowPos_Original(h, after, x, y, cx, cy, f);
}

static bool IsTaskbarWindow(HWND hWnd) {
    wchar_t className[64];
    return GetClassName(hWnd, className, ARRAYSIZE(className)) > 0 &&
           (wcscmp(className, L"Shell_TrayWnd") == 0 ||
            wcscmp(className, L"Shell_SecondaryTrayWnd") == 0);
}

// Sees every message the taskbar thread retrieves. Mouse moves over the
// taskbar, and the leave messages sent when the cursor exits it, restart the
// hide timer, so the delay counts from the moment the cursor leaves.
LRESULT CALLBACK GetMessageHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && wParam == PM_REMOVE) {
        const MSG* msg = (const MSG*)lParam;
        switch (msg->message) {
            case WM_MOUSEMOVE:
            case WM_NCMOUSEMOVE:
            case WM_MOUSELEAVE:
            case WM_NCMOUSELEAVE:
            case WM_POINTERUPDATE:
            case WM_POINTERLEAVE: {
                HWND root = msg->hwnd ? GetAncestor(msg->hwnd, GA_ROOT) : nullptr;
                auto it = g_hideTimers.find(root);
                if (it == g_hideTimers.end()) break;
                // The taskbar also gets these when the cursor is elsewhere;
                // only real movement over it, or the cursor leaving it,
                // should restart the countdown.
                bool leave = msg->message == WM_MOUSELEAVE ||
                             msg->message == WM_NCMOUSELEAVE ||
                             msg->message == WM_POINTERLEAVE;
                POINT pt;
                RECT rc;
                bool over = GetCursorPos(&pt) && GetWindowRect(root, &rc) && PtInRect(&rc, pt);
                if (leave || over) {
                    SetTimer_Original(root, kHideTimerId, g_settings.hideDelay, it->second);
                }
                break;
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

UINT_PTR WINAPI SetTimer_Hook(HWND hWnd,
                              UINT_PTR nIDEvent,
                              UINT uElapse,
                              TIMERPROC lpTimerFunc) {
    if (hWnd && (nIDEvent == 2 || nIDEvent == 3)) {
        wchar_t className[64];
        if (GetClassName(hWnd, className, ARRAYSIZE(className)) > 0) {
            if (wcscmp(className, L"Shell_TrayWnd") == 0 ||
                wcscmp(className, L"Shell_SecondaryTrayWnd") == 0) {
                uElapse = (nIDEvent == 3) ? g_settings.unhideDelay
                                          : g_settings.hideDelay;
                if (nIDEvent == kHideTimerId) {
                    g_hideTimers[hWnd] = lpTimerFunc;
                    // We're on the taskbar's thread here, which is where a
                    // thread mouse hook has to be installed.
                    if (!g_mouseHook) {
                        g_mouseHook = SetWindowsHookEx(WH_GETMESSAGE, GetMessageHookProc, nullptr,
                                                       GetCurrentThreadId());
                    }
                }
            }
        }
    }
    return SetTimer_Original(hWnd, nIDEvent, uElapse, lpTimerFunc);
}

using KillTimer_t = decltype(&KillTimer);
KillTimer_t KillTimer_Original;
BOOL WINAPI KillTimer_Hook(HWND hWnd, UINT_PTR nIDEvent) {
    if (hWnd && nIDEvent == kHideTimerId) {
        g_hideTimers.erase(hWnd);
    }
    return KillTimer_Original(hWnd, nIDEvent);
}

// --- Edge detection for empty monitors ---

HANDLE g_edgeCheckThread = NULL;
std::atomic<bool> g_edgeCheckRunning{false};

HWND FindTaskbarForMonitor(HMONITOR hMon) {
    HWND hTaskbar = FindWindow(L"Shell_TrayWnd", NULL);
    if (hTaskbar) {
        HMONITOR tbMon =
            MonitorFromWindow(hTaskbar, MONITOR_DEFAULTTONEAREST);
        if (tbMon == hMon)
            return hTaskbar;
    }

    HWND hSecondary = NULL;
    while ((hSecondary = FindWindowEx(NULL, hSecondary,
                                      L"Shell_SecondaryTrayWnd", NULL))) {
        HMONITOR tbMon =
            MonitorFromWindow(hSecondary, MONITOR_DEFAULTTONEAREST);
        if (tbMon == hMon)
            return hSecondary;
    }
    return NULL;
}

bool IsTaskbarEffectivelyHidden(HWND hTaskbar, HMONITOR hMon) {
    RECT tr;
    GetWindowRect(hTaskbar, &tr);

    MONITORINFO mi = {sizeof(mi)};
    if (!GetMonitorInfo(hMon, &mi))
        return false;

    RECT overlap;
    if (!IntersectRect(&overlap, &tr, &mi.rcMonitor))
        return true;

    int visH = overlap.bottom - overlap.top;
    int visW = overlap.right - overlap.left;
    int tbH = tr.bottom - tr.top;
    int tbW = tr.right - tr.left;

    // Hidden if less than half visible in either dimension
    return (tbH > 0 && visH < tbH / 2) || (tbW > 0 && visW < tbW / 2);
}

bool IsCursorAtMonitorEdge(POINT pt, HMONITOR hMon) {
    MONITORINFO mi = {sizeof(mi)};
    if (!GetMonitorInfo(hMon, &mi))
        return false;

    return pt.y >= mi.rcMonitor.bottom - 1 ||
           pt.y <= mi.rcMonitor.top ||
           pt.x >= mi.rcMonitor.right - 1 ||
           pt.x <= mi.rcMonitor.left;
}

DWORD WINAPI EdgeCheckThreadProc(LPVOID) {
    // Fast poll so we notice the cursor hitting the edge immediately.
    const DWORD pollInterval = 20;
    // Re-post the unhide trigger while the cursor sits at the edge and
    // the taskbar is still hidden. No one-shot latch — a single
    // PostMessage can be swallowed by foreground apps (fullscreen Chrome,
    // terminals, games), so we keep retrying until it takes or the cursor
    // leaves.
    const DWORD retryInterval = 150;

    DWORD lastRetryTime = 0;

    while (g_edgeCheckRunning) {
        POINT pt;
        if (GetCursorPos(&pt)) {
            HMONITOR hMon =
                MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);

            if (IsCursorAtMonitorEdge(pt, hMon)) {
                DWORD now = GetTickCount();
                if (lastRetryTime == 0 ||
                    now - lastRetryTime >= retryInterval) {
                    HWND hTaskbar = FindTaskbarForMonitor(hMon);
                    if (hTaskbar &&
                        IsTaskbarEffectivelyHidden(hTaskbar, hMon)) {
                        PostMessage(hTaskbar, WM_TIMER, 3, 0);
                    }
                    lastRetryTime = now ? now : 1;
                }
            } else {
                lastRetryTime = 0;
            }
        }

        Sleep_Original ? Sleep_Original(pollInterval)
                       : Sleep(pollInterval);
    }

    return 0;
}

void StartEdgeCheckThread() {
    if (g_edgeCheckThread)
        return;

    g_edgeCheckRunning = true;
    g_edgeCheckThread =
        CreateThread(NULL, 0, EdgeCheckThreadProc, NULL, 0, NULL);
}

void StopEdgeCheckThread() {
    if (!g_edgeCheckThread)
        return;

    g_edgeCheckRunning = false;
    WaitForSingleObject(g_edgeCheckThread, 2000);
    CloseHandle(g_edgeCheckThread);
    g_edgeCheckThread = NULL;
}

bool HookTaskbarSymbols() {
    HMODULE module;
    if (g_winVersion <= WinVersion::Win10) {
        module = GetModuleHandle(nullptr);
    } else {
        module = LoadLibrary(L"taskbar.dll");
        if (!module) {
            Wh_Log(L"Couldn't load taskbar.dll");
            return false;
        }
    }

    // explorer.exe, taskbar.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: virtual void __cdecl TrayUI::SlideWindow(struct HWND__ *,struct tagRECT const *,struct HMONITOR__ *,bool,bool))"},
            &TrayUI_SlideWindow_Original,
            TrayUI_SlideWindow_Hook,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    void* pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;

    HRSRC hResource =
        FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource) {
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (hGlobal) {
            void* pData = LockResource(hGlobal);
            if (pData) {
                if (!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) ||
                    uPtrLen == 0) {
                    pFixedFileInfo = nullptr;
                    uPtrLen = 0;
                }
            }
        }
    }

    if (puPtrLen) {
        *puPtrLen = uPtrLen;
    }

    return (VS_FIXEDFILEINFO*)pFixedFileInfo;
}

WinVersion GetExplorerVersion() {
    VS_FIXEDFILEINFO* fixedFileInfo = GetModuleVersionInfo(nullptr, nullptr);
    if (!fixedFileInfo) {
        return WinVersion::Unsupported;
    }

    WORD major = HIWORD(fixedFileInfo->dwFileVersionMS);
    WORD minor = LOWORD(fixedFileInfo->dwFileVersionMS);
    WORD build = HIWORD(fixedFileInfo->dwFileVersionLS);
    WORD qfe = LOWORD(fixedFileInfo->dwFileVersionLS);

    Wh_Log(L"Version: %u.%u.%u.%u", major, minor, build, qfe);

    switch (major) {
        case 10:
            if (build < 22000) {
                return WinVersion::Win10;
            } else if (build < 26100) {
                return WinVersion::Win11;
            } else {
                return WinVersion::Win11_24H2;
            }
            break;
    }

    return WinVersion::Unsupported;
}

struct EXPLORER_PATCHER_HOOK {
    PCSTR symbol;
    void** pOriginalFunction;
    void* hookFunction = nullptr;
    bool optional = false;

    template <typename Prototype>
    EXPLORER_PATCHER_HOOK(
        PCSTR symbol,
        Prototype** originalFunction,
        std::type_identity_t<Prototype*> hookFunction = nullptr,
        bool optional = false)
        : symbol(symbol),
          pOriginalFunction(reinterpret_cast<void**>(originalFunction)),
          hookFunction(reinterpret_cast<void*>(hookFunction)),
          optional(optional) {}
};

bool HookExplorerPatcherSymbols(HMODULE explorerPatcherModule) {
    if (g_explorerPatcherInitialized.exchange(true)) {
        return true;
    }

    if (g_winVersion >= WinVersion::Win11) {
        g_winVersion = WinVersion::Win10;
    }

    EXPLORER_PATCHER_HOOK hooks[] = {
        {R"(?SlideWindow@TrayUI@@UEAAXPEAUHWND__@@PEBUtagRECT@@PEAUHMONITOR__@@_N3@Z)",
         &TrayUI_SlideWindow_Original, TrayUI_SlideWindow_Hook},
    };

    bool succeeded = true;

    for (const auto& hook : hooks) {
        void* ptr = (void*)GetProcAddress(explorerPatcherModule, hook.symbol);
        if (!ptr) {
            Wh_Log(L"ExplorerPatcher symbol%s doesn't exist: %S",
                   hook.optional ? L" (optional)" : L"", hook.symbol);
            if (!hook.optional) {
                succeeded = false;
            }
            continue;
        }

        if (hook.hookFunction) {
            Wh_SetFunctionHook(ptr, hook.hookFunction, hook.pOriginalFunction);
        } else {
            *hook.pOriginalFunction = ptr;
        }
    }

    if (!succeeded) {
        Wh_Log(L"HookExplorerPatcherSymbols failed");
    } else if (g_initialized) {
        Wh_ApplyHookOperations();
    }

    return succeeded;
}

bool IsExplorerPatcherModule(HMODULE module) {
    WCHAR moduleFilePath[MAX_PATH];
    switch (
        GetModuleFileName(module, moduleFilePath, ARRAYSIZE(moduleFilePath))) {
        case 0:
        case ARRAYSIZE(moduleFilePath):
            return false;
    }

    PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\');
    if (!moduleFileName) {
        return false;
    }

    moduleFileName++;

    if (_wcsnicmp(L"ep_taskbar.", moduleFileName, sizeof("ep_taskbar.") - 1) ==
        0) {
        Wh_Log(L"ExplorerPatcher taskbar module: %s", moduleFileName);
        return true;
    }

    return false;
}

bool HandleLoadedExplorerPatcher() {
    HMODULE hMods[1024];
    DWORD cbNeeded;
    if (EnumProcessModules(GetCurrentProcess(), hMods, sizeof(hMods),
                           &cbNeeded)) {
        for (size_t i = 0; i < cbNeeded / sizeof(HMODULE); i++) {
            if (IsExplorerPatcherModule(hMods[i])) {
                return HookExplorerPatcherSymbols(hMods[i]);
            }
        }
    }

    return true;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module && !((ULONG_PTR)module & 3) && !g_explorerPatcherInitialized) {
        if (IsExplorerPatcherModule(module)) {
            HookExplorerPatcherSymbols(module);
        }
    }

    return module;
}

void LoadSettings() {
    LPCWSTR animType = Wh_GetStringSetting(L"animationType");
    if (wcscmp(animType, L"none") == 0) {
        g_settings.animationType = -1;
    } else if (wcscmp(animType, L"elastic") == 0) {
        g_settings.animationType = 1;
    } else if (wcscmp(animType, L"bounce") == 0) {
        g_settings.animationType = 2;
    } else if (wcscmp(animType, L"fade") == 0) {
        g_settings.animationType = 3;
    } else if (wcscmp(animType, L"slideFade") == 0) {
        g_settings.animationType = 4;
    } else if (wcscmp(animType, L"overshoot") == 0) {
        g_settings.animationType = 5;
    } else {
        g_settings.animationType = 0;
    }
    Wh_FreeStringSetting(animType);

    g_settings.showSpeedup = Wh_GetIntSetting(L"showSpeedup");
    g_settings.hideSpeedup = Wh_GetIntSetting(L"hideSpeedup");
    g_settings.showDuration = Wh_GetIntSetting(L"showDuration");
    g_settings.hideDuration = Wh_GetIntSetting(L"hideDuration");
    g_settings.frameRate = Wh_GetIntSetting(L"frameRate");
    g_settings.unhideDelay = Wh_GetIntSetting(L"unhideDelay");
    g_settings.hideDelay = Wh_GetIntSetting(L"hideDelay");
    g_settings.oldTaskbarOnWin11 = Wh_GetIntSetting(L"oldTaskbarOnWin11");
    g_settings.edgeDetection = Wh_GetIntSetting(L"edgeDetection");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    g_winVersion = GetExplorerVersion();
    if (g_winVersion == WinVersion::Unsupported) {
        Wh_Log(L"Unsupported Windows version");
        return FALSE;
    }

    if (g_settings.oldTaskbarOnWin11) {
        bool hasWin10Taskbar = g_winVersion < WinVersion::Win11_24H2;

        if (g_winVersion >= WinVersion::Win11) {
            g_winVersion = WinVersion::Win10;
        }

        if (hasWin10Taskbar && !HookTaskbarSymbols()) {
            return FALSE;
        }
    } else if (!HookTaskbarSymbols()) {
        return FALSE;
    }

    if (!HandleLoadedExplorerPatcher()) {
        Wh_Log(L"HandleLoadedExplorerPatcher failed");
        return FALSE;
    }

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");

    auto pKernelBaseLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(
        kernelBaseModule, "LoadLibraryExW");
    WindhawkUtils::Wh_SetFunctionHookT(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);

    auto pKernelBaseGetTickCount = (decltype(&GetTickCount))GetProcAddress(
        kernelBaseModule, "GetTickCount");
    WindhawkUtils::Wh_SetFunctionHookT(
        pKernelBaseGetTickCount, GetTickCount_Hook, &GetTickCount_Original);

    auto pKernelBaseSleep =
        (decltype(&Sleep))GetProcAddress(kernelBaseModule, "Sleep");
    WindhawkUtils::Wh_SetFunctionHookT(pKernelBaseSleep, Sleep_Hook,
                                       &Sleep_Original);

    HMODULE user32Module = GetModuleHandle(L"user32.dll");
    auto pSetTimer =
        (decltype(&SetTimer))GetProcAddress(user32Module, "SetTimer");
    WindhawkUtils::Wh_SetFunctionHookT(pSetTimer, SetTimer_Hook,
                                       &SetTimer_Original);
    WindhawkUtils::Wh_SetFunctionHookT(
        (decltype(&SetWindowPos))GetProcAddress(user32Module, "SetWindowPos"),
        SetWindowPos_Hook, &SetWindowPos_Original);
    WindhawkUtils::Wh_SetFunctionHookT(
        (decltype(&KillTimer))GetProcAddress(user32Module, "KillTimer"),
        KillTimer_Hook, &KillTimer_Original);

    TimerInitialize();

    g_initialized = true;

    if (g_settings.edgeDetection) {
        StartEdgeCheckThread();
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (!g_explorerPatcherInitialized) {
        HandleLoadedExplorerPatcher();
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");
    StopEdgeCheckThread();
    if (g_mouseHook) {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = nullptr;
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    bool prevOldTaskbarOnWin11 = g_settings.oldTaskbarOnWin11;

    LoadSettings();

    if (g_settings.edgeDetection) {
        StartEdgeCheckThread();
    } else {
        StopEdgeCheckThread();
    }

    *bReload = g_settings.oldTaskbarOnWin11 != prevOldTaskbarOnWin11;

    return TRUE;
}
