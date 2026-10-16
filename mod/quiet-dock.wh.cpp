// ==WindhawkMod==
// @id              quiet-dock
// @name            Quiet Dock
// @description     macOS-style taskbar magnification that stops using CPU once the dock settles
// @version         1.0.0
// @author          ziyaad-mallick
// @github          https://github.com/ziyaad-mallick
// @homepage        https://github.com/ziyaad-mallick/quiet-dock
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshcore -lwindowsapp -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Quiet Dock

macOS-style icon magnification for the Windows 11 taskbar, tuned to cost
nothing while you're not moving the mouse.

A fork of [Taskbar Dock Animation Plus](https://windhawk.net/mods/taskbar-dock-animation-plus)
by [@incconutwo](https://github.com/incconutwo), itself a fork of
[Taskbar Dock Animation](https://windhawk.net/mods/taskbar-dock-animation) by
[@Ph0en1x-dev](https://github.com/Ph0en1x-dev). The magnification logic and
visual design are theirs.

## What's different

* **The frame loop sleeps once the dock settles.** Upstream keeps a per-frame
  callback subscribed for as long as the cursor is over the taskbar, which also
  makes XAML redraw the taskbar every frame. Here the callback unsubscribes as
  soon as every icon has reached its target, and the next mouse move wakes it.
* **Each monitor animates on its own.** Upstream shares one "active taskbar"
  between all monitors, so moving the cursor from one taskbar to another could
  leave icons stuck magnified (windhawk-mods#3593).
* **Less work per frame.** Transforms are looked up once per icon instead of
  about ten times per frame, and a property is only written when its value
  changed.
* **Frame timing uses fractional milliseconds**, so 120/144 Hz panels don't
  lose time to rounding.
* **The idle "breathing" bounce is off by default.** macOS doesn't do it, and
  it moves the click target away from the cursor (windhawk-mods#2790).
* **Default magnification is 140%.** Above 130% icons need empty space above
  the dock or they get clipped; the companion Taskbar Styler theme in the repo
  provides it.

**Requires Windows 11.**
*/
// ==/WindhawkModReadme==


// ==WindhawkModSettings==
/*
- AnimationType: 0
  $name: Animation Curve
  $name:uk-UA: Крива анімації
  $name:zh-CN: 动画曲线
  $name:ja-JP: アニメーション曲線
  $name:ko-KR: 애니메이션 곡선
  $name:pt-BR: Curva de animação
  $name:it-IT: Curva di animazione
  $description: Choose the feel of the scaling (0 = Cosine (Smooth), 1 = Linear, 2 = Exponential (Snappy)
  $description:uk-UA: Оберіть тип анімації (0 = Косинус (Плавно), 1 = Лінійна, 2 = Експоненціальна (Швидко)
  $description:zh-CN: 选择缩放的动画感觉（0=余弦（平滑），1=线性，2=指数（更灵敏））
  $description:ja-JP: 拡大縮小の動き方を選択（0=コサイン（滑らか）、1=線形、2=指数（キビキビ））
  $description:ko-KR: 확대/축소 애니메이션 느낌 선택 (0=코사인(부드러움), 1=선형, 2=지수(빠릿함))
  $description:pt-BR: Escolha o tipo de escala (0 = Cosseno (suave), 1 = Linear, 2 = Exponencial (rápido))
  $description:it-IT: Scegli il tipo di scala (0 = Coseno (fluida), 1 = Lineare, 2 = Esponenziale (reattiva))
- MaxScale: 157
  $name: Maximum scale (%)
  $name:uk-UA: Максимальний розмір (%)
  $name:zh-CN: 最大缩放（%）
  $name:ja-JP: 最大倍率（%）
  $name:ko-KR: 최대 확대 비율(%)
  $name:pt-BR: Escala máxima (%)
  $name:it-IT: Scala massima (%)
  $description: How large an icon can grow (e.g., 160 = +60%)
  $description:uk-UA: Наскільки сильно іконка може збільшитись (напр., 160 = +60%)
  $description:zh-CN: 图标最大可放大到多少（例如 160 = 增加 60%）
  $description:ja-JP: "アイコンの最大拡大率（例: 160 = +60%）"
  $description:ko-KR: "아이콘이 커질 수 있는 최대 크기 (예: 160 = +60%)"
  $description:pt-BR: "Até quanto o ícone pode aumentar (ex.: 160 = +60%)"
  $description:it-IT: "Quanto può ingrandirsi un’icona (es.: 160 = +60%)"
- EffectRadius: 100
  $name: Effect radius (px)
  $name:uk-UA: Радіус ефекту (пкс)
  $name:zh-CN: 效果半径（像素）
  $name:ja-JP: 効果半径（px）
  $name:ko-KR: 효과 반경(px)
  $name:pt-BR: Raio do efeito (px)
  $name:it-IT: Raggio dell’effetto (px)
  $description: Distance from the cursor where the animation is applied
  $description:uk-UA: Дистанція від курсора, на якій застосовується анімація
  $description:zh-CN: 距离光标多远范围内应用动画
  $description:ja-JP: カーソルからの距離に応じてアニメーションを適用します
  $description:ko-KR: 커서로부터 애니메이션이 적용되는 거리
  $description:pt-BR: Distância do cursor em que a animação é aplicada
  $description:it-IT: Distanza dal cursore entro cui si applica l’animazione
- SpacingFactor: 100
  $name: Spacing sensitivity (%)
  $name:uk-UA: Чутливість до проміжку (%)
  $name:zh-CN: 间距灵敏度（%）
  $name:ja-JP: 間隔の感度（%）
  $name:ko-KR: 간격 민감도(%)
  $name:pt-BR: Sensibilidade do espaçamento (%)
  $name:it-IT: Sensibilità spaziatura (%)
  $description: How much neighboring icons move apart during animation (100% = they make room for the full magnified size, like macOS)
  $description:uk-UA: Наскільки сусідні іконки розсуваються під час анімації (за замовчуванням 50%)
  $description:zh-CN: 动画时相邻图标拉开的程度（默认 50%）
  $description:ja-JP: アニメーション中に隣のアイコンが離れる度合い（既定 50%）
  $description:ko-KR: 애니메이션 중 인접 아이콘이 벌어지는 정도(기본 50%)
  $description:pt-BR: Quanto os ícones vizinhos se afastam durante a animação (padrão 50%)
  $description:it-IT: Quanto si allontanano le icone vicine durante l’animazione (predef. 50%)
- BounceDelay: 500
  $name: Bounce start delay (ms)
  $name:uk-UA: Затримка початку "дихання" (мс)
  $name:zh-CN: 弹跳开始延迟（毫秒）
  $name:ja-JP: バウンス開始遅延（ms）
  $name:ko-KR: 바운스 시작 지연(ms)
  $name:pt-BR: Atraso para iniciar o bounce (ms)
  $name:it-IT: Ritardo di inizio bounce (ms)
  $description: How long to wait after the cursor stops before starting the idle "breathing" animation (default 500ms)
  $description:uk-UA: Скільки чекати після зупинки курсора перед початком анімації "дихання" (за замовчуванням 500мс)
  $description:zh-CN: 光标停止后等待多久开始“呼吸”动画（默认 500ms）
  $description:ja-JP: カーソル停止後、待機時の「呼吸」アニメを開始するまでの遅延（既定 500ms）
  $description:ko-KR: 커서가 멈춘 뒤 ‘호흡’ 애니메이션을 시작하기까지 대기 시간(기본 500ms)
  $description:pt-BR: Quanto esperar após o cursor parar antes de iniciar a animação “respirar” (padrão 500ms)
  $description:it-IT: Quanto attendere dopo che il cursore si ferma prima di avviare l’animazione “respiro” (predef. 500ms)
- FocusDuration: 150
  $name: Focus animation duration (ms)
  $name:uk-UA: Тривалість анімації фокусу (мс)
  $name:zh-CN: 聚焦动画时长（毫秒）
  $name:ja-JP: フォーカスアニメ時間（ms）
  $name:ko-KR: 포커스 애니메이션 지속 시간(ms)
  $name:pt-BR: Duração da animação de foco (ms)
  $name:it-IT: Durata animazione focus (ms)
  $description: Duration of the fade-in and fade-out animation (default 150ms)
  $description:uk-UA: Тривалість анімації плавного з'явлення та згасання (за замовчуванням 150мс)
  $description:zh-CN: 淡入/淡出动画时长（默认 150ms）
  $description:ja-JP: フェードイン/アウトの時間（既定 150ms）
  $description:ko-KR: 페이드 인/아웃 애니메이션 지속 시간(기본 150ms)
  $description:pt-BR: Duração do fade-in e fade-out (padrão 150ms)
  $description:it-IT: Durata della dissolvenza in entrata/uscita (predef. 150ms)
- DisableVerticalBounce: false
  $name: Disable vertical "bounce" animation
  $name:uk-UA: Вимкнути вертикальне "дихання"
  $name:zh-CN: 禁用垂直“弹跳”动画
  $name:ja-JP: 垂直バウンスを無効化
  $name:ko-KR: 수직 바운스 애니메이션 비활성화
  $name:pt-BR: Desativar bounce vertical
  $name:it-IT: Disattiva bounce verticale
  $description: Disables the up/down "breathing" animation. Useful for vertical taskbars.
  $description:uk-UA: Вимикає анімацію "дихання" вгору/вниз. Корисно для вертикальних панелей.
  $description:zh-CN: 禁用上下“呼吸”动画。对垂直任务栏有用。
  $description:ja-JP: 上下の「呼吸」アニメを無効化します。縦型タスクバーに便利です。
  $description:ko-KR: 위/아래 ‘호흡’ 애니메이션을 비활성화합니다. 세로 작업 표시줄에 유용합니다.
  $description:pt-BR: Desativa a animação de “respirar” para cima/baixo. Útil para barras verticais.
  $description:it-IT: Disattiva l’animazione “respiro” su/giù. Utile per barre verticali.
- TaskbarLabelsMode: false
  $name: Taskbar Labels Compatibility
  $name:uk-UA: Сумісність із підписами (Labels)
  $name:zh-CN: 任务栏标签兼容模式
  $name:ja-JP: タスクバーラベル互換
  $name:ko-KR: 작업 표시줄 라벨 호환
  $name:pt-BR: Compatibilidade com rótulos
  $name:it-IT: Compatibilità etichette
  $description: Enable this if your taskbar shows window titles (rectangular icons) to fix animation centering.
  $description:uk-UA: Увімкніть, якщо ваша панель завдань відображає назви вікон (прямокутні кнопки), щоб виправити центрування.
  $description:zh-CN: 若任务栏显示窗口标题（矩形按钮），启用此项以修正动画居中。
  $description:ja-JP: タスクバーにウィンドウ名（矩形ボタン）が表示される場合、中央揃えを修正するために有効化します。
  $description:ko-KR: 작업 표시줄에 창 제목(직사각형 버튼)이 표시될 때 애니메이션 중앙 정렬을 수정합니다.
  $description:pt-BR: Ative se a barra mostrar títulos de janelas (ícones retangulares) para corrigir o alinhamento.
  $description:it-IT: Abilita se la barra mostra i titoli delle finestre (pulsanti rettangolari) per correggere la centratura.
- ExcludeSystemButtonsMode: 0
  $name: Exclude system buttons
  $name:uk-UA: Ігнорувати системні кнопки
  $name:zh-CN: 排除系统按钮
  $name:ja-JP: システムボタンを除外
  $name:ko-KR: 시스템 버튼 제외
  $name:pt-BR: Excluir botões do sistema
  $name:it-IT: Escludi pulsanti di sistema
  $description: 0=Animate all, 1=Exclude Start, 2=Animate apps icons only
  $description:uk-UA: 0=Анімація всього, 1=Без анімації Start, 2=Анімація тільки іконок застосунків
  $description:zh-CN: 0=全部动画，1=排除开始，2=仅应用图标动画
  $description:ja-JP: 0=すべてアニメ、1=スタート除外、2=アプリのみ
  $description:ko-KR: 0=전체, 1=시작 제외, 2=앱만
  $description:pt-BR: 0=Animar tudo, 1=Excluir Iniciar, 2=Somente apps
  $description:it-IT: 0=Anima tutto, 1=Escludi Start, 2=Solo app
- LerpSpeed: 60
  $name: Smoothing (Lerp speed)
  $name:uk-UA: Плавність (швидкість Lerp)
  $name:zh-CN: 平滑度（Lerp 速度）
  $name:ja-JP: スムージング（Lerp 速度）
  $name:ko-KR: 부드러움(Lerp 속도)
  $name:pt-BR: Suavização (velocidade do Lerp)
  $name:it-IT: Smussamento (velocità Lerp)
  $description: Higher = snappier, lower = smoother. 0 = disabled (direct)
  $description:uk-UA: Більше = швидше, менше = плавніше. 0 = вимкнути (напряму)
  $description:zh-CN: 越高越灵敏，越低越平滑。0=禁用（直接应用）
  $description:ja-JP: 大きいほどキビキビ、小さいほど滑らか。0=無効（直接適用）
  $description:ko-KR: 높을수록 빠릿함, 낮을수록 부드러움. 0=비활성화(직접)
  $description:pt-BR: Maior = mais rápido, menor = mais suave. 0 = desativado (direto)
  $description:it-IT: Più alto = più reattivo, più basso = più fluido. 0 = disattivato (diretto)
- DisableBounce: true
  $name: Disable bounce effect
  $name:uk-UA: Вимкнути bounce ефект
  $name:zh-CN: 禁用弹跳效果
  $name:ja-JP: バウンス効果を無効化
  $name:ko-KR: 바운스 효과 비활성화
  $name:pt-BR: Desativar efeito bounce
  $name:it-IT: Disattiva effetto bounce
  $description: Disables the idle "breathing" bounce entirely
  $description:uk-UA: Повністю вимикає bounce ("дихання")
  $description:zh-CN: 完全禁用空闲时的“呼吸”弹跳
  $description:ja-JP: 待機時の「呼吸」バウンスを完全に無効化します
  $description:ko-KR: 대기 상태 ‘호흡’ 바운스를 완전히 비활성화합니다
  $description:pt-BR: Desativa totalmente o bounce ocioso (“respirar”)
  $description:it-IT: Disattiva completamente il bounce inattivo (“respiro”)
- RestIconSize: 28
  $name: Rest icon size (px)
  $description: >-
    Draw icons at this size when not hovered. Windows renders them at the
    "Taskbar height and icon size" icon size, so set that to this value times
    Maximum scale (28 x 157% = 44): magnified icons then show native pixels
    instead of a stretched bitmap. Icons are also packed at this size plus the
    gap below. 0 = off (icons keep Windows' size and spacing).
- IconGap: 10
  $name: Gap between icons (px)
  $description: Space between icons at rest, when Rest icon size is on
- DockPadding: 10
  $name: Dock padding (px)
  $description: >-
    Space between the outer icons and the edge of the taskbar background, which
    is resized to fit the icons as they magnify
*/
// ==/WindhawkModSettings==


#include <windhawk_utils.h>
#undef GetCurrentTime
#include <windows.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Input.h>
#include <vector>
#include <atomic>
#include <functional>
#include <cmath>
#include <map>
#include <algorithm>
#include <chrono>
#include <unordered_map>
#include <unordered_set>
#include <cstdint>
#include <mutex>
#include <winrt/Windows.UI.Xaml.Automation.h>

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Media;
using namespace winrt::Windows::UI::Xaml::Automation;

// Settings container
struct {
    int animationType;
    double maxScale;
    int effectRadius;
    double spacingFactor;
    int bounceDelay;
    double focusDuration;
    bool disableVerticalBounce;
    bool taskbarLabelsMode;
    int excludeSystemButtonsMode;
    double lerpSpeed;
    bool disableBounce;
    double restIconSize;
    double iconGap;
    double dockPadding;
} g_settings;


// Global flags
std::atomic<bool> g_taskbarViewDllLoaded = false;
std::atomic<bool> g_hooksApplied = false;

const double MOUSE_STOP_THRESHOLD = 0.5;

const double BOUNCE_PERIOD_MS = 1200.0;
const double BOUNCE_SCALE_AMOUNT = 0.05;
const double BOUNCE_TRANSLATE_Y = -4.0;

// Below these deltas a change is invisible: skip the XAML write, and treat
// the icon as settled so the frame loop can stop.
const double SCALE_EPSILON = 0.001;
const double TRANSLATE_EPSILON = 0.05;

// A frame gap longer than this (a stall, or waking from sleep) is clamped so
// the lerp doesn't jump.
const double MAX_FRAME_MS = 100.0;

enum class TaskbarEdge {
    Bottom,
    Top,
    Left,
    Right
};

struct IconAnimState {
    double waveScale = 1.0;
    double waveTranslateX = 0.0;
};

// The transforms in an icon's TransformGroup, looked up once. Applied in
// order: base (rest size), wave (magnification), wave translate, bounce.
const uint32_t TRANSFORM_COUNT = 5;
struct IconTransforms {
    ScaleTransform baseScale{nullptr};
    ScaleTransform waveScale{nullptr};
    TranslateTransform waveTranslate{nullptr};
    ScaleTransform bounceScale{nullptr};
    TranslateTransform bounceTranslate{nullptr};
};

// Values last written to the transforms. NaN forces the next write.
struct AppliedValues {
    double baseScale = NAN;
    double waveScale = NAN;
    double waveTranslate = NAN;
    double bounceScale = NAN;
    double bounceTranslate = NAN;
};

// Per-monitor context
struct TaskbarIconInfo {
    winrt::weak_ref<FrameworkElement> element;
    double originalCenterX = 0.0;
    double elementWidth = 0.0;
    // Icon geometry inside the button, for the scale centers: the icon
    // shrinks around its center and magnifies from its (rest) bottom edge.
    double iconCenterY = 0.0;
    double iconSize = 0.0;  // 0 when the button has no "Icon" element
    double baseScale = 1.0;
    IconAnimState state;
    IconTransforms tf;
    AppliedValues applied;
};

struct HostSignature {
    int count = -1;
    double width = -1.0;
    uint64_t orderHash = 0;
};

struct DockAnimationContext {
    void* key = nullptr;  // its g_contexts key, for StartRendering
    bool isInitialized = false;
    winrt::weak_ref<FrameworkElement> taskbarFrame;
    winrt::weak_ref<FrameworkElement> iconHost;
    std::vector<TaskbarIconInfo> icons;

    HostSignature lastSig;
    std::chrono::steady_clock::time_point lastDirtyCheck{};

    TaskbarEdge edge = TaskbarEdge::Bottom;
    bool isVertical = false;
    HWND hWnd = nullptr;
    RECT lastTrayRect = {0, 0, 0, 0};

    // Animation state. Each taskbar has its own, touched only from the
    // thread that owns its window.
    winrt::event_token renderingToken{};
    bool isRenderingHooked = false;
    bool isMouseInside = false;
    double intensity = 0.0;
    double mousePos = -1.0;
    bool isBouncing = false;
    double bounceIntensity = 0.0;
    std::chrono::steady_clock::time_point lastRenderTime{};
    std::chrono::steady_clock::time_point lastSignificantMoveTime{};
    std::chrono::steady_clock::time_point bounceStartTime{};

    // Set by the layout hook: icons were added, removed or moved, so the
    // next frame rebuilds the icon list and reapplies the rest layout.
    bool needsRefresh = true;

    // The frame's background element (the dock pill in a dock theme). Its
    // horizontal margins are driven to wrap the icons; vertical ones are left
    // as the theme set them. baseMargin is restored on unload.
    winrt::weak_ref<FrameworkElement> background;
    Thickness backgroundBaseMargin{};
    bool backgroundBaseKnown = false;
    double backgroundSlotLeft = 0.0;   // its layout slot, frame coordinates
    double backgroundSlotRight = 0.0;
    double appliedBackgroundLeft = NAN;
    double appliedBackgroundRight = NAN;
    // While icons magnify, the pill is stretched with a render transform
    // (no layout pass) from its rest extent, which the margins hold.
    winrt::weak_ref<FrameworkElement> backgroundLayer;
    ScaleTransform backgroundScale{nullptr};
    TranslateTransform backgroundTranslate{nullptr};
    double appliedBackgroundScale = NAN;
    double appliedBackgroundShift = NAN;

    // Per-frame scratch buffers, kept to avoid reallocating every frame.
    std::vector<double> scales;
    std::vector<double> restSlots;
    std::vector<double> restCenters;
};

// One context per monitor (keyed by XAML instance pointer). Contexts are
// added from the taskbar threads and read from the Windhawk thread on
// settings change and unload, so map access is locked. std::map never moves
// its nodes, so a context pointer stays valid until the map is cleared.
std::map<void*, DockAnimationContext> g_contexts;
std::mutex g_contextsMutex;

// Each hook gets a different interface pointer for the same TaskbarFrame,
// so contexts are keyed by the object's IUnknown identity.
void* FrameKey(FrameworkElement const& frame) {
    return winrt::get_abi(frame.as<winrt::Windows::Foundation::IUnknown>());
}

DockAnimationContext* FindContext(void* key) {
    std::lock_guard<std::mutex> lock(g_contextsMutex);
    auto it = g_contexts.find(key);
    return it == g_contexts.end() ? nullptr : &it->second;
}

// Prototypes
void LoadSettings();
bool ApplyAnimation(DockAnimationContext& ctx, double dtSec);
DockAnimationContext* InitializeAnimationHooks(void* pThis, FrameworkElement const& taskbarFrame);
void OnTaskbarPointerMoved(DockAnimationContext& ctx, void* key, Input::PointerRoutedEventArgs const& args);
void OnTaskbarPointerExited(DockAnimationContext& ctx, void* key);
void ResetAllIconScales(std::vector<TaskbarIconInfo>& icons);
void RefreshIconPositions(DockAnimationContext& ctx);
void OnRendering(void* key);

HMODULE GetTaskbarViewModuleHandle();
bool HookTaskbarViewDllSymbols(HMODULE module);
static bool RebaseIconGeometryFast(DockAnimationContext& ctx);

HWND GetCurrentThreadTrayWindow() {
    DWORD currentThreadId = GetCurrentThreadId();
    HWND hPrimary = FindWindow(L"Shell_TrayWnd", NULL);
    if (hPrimary && GetWindowThreadProcessId(hPrimary, nullptr) == currentThreadId) {
        return hPrimary;
    }
    HWND hSec = nullptr;
    while ((hSec = FindWindowEx(NULL, hSec, L"Shell_SecondaryTrayWnd", NULL)) != nullptr) {
        if (GetWindowThreadProcessId(hSec, nullptr) == currentThreadId) {
            return hSec;
        }
    }
    POINT pt;
    if (GetCursorPos(&pt)) {
        HMONITOR hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
        if (hMonitor) {
            if (hPrimary && MonitorFromWindow(hPrimary, MONITOR_DEFAULTTONULL) == hMonitor) {
                return hPrimary;
            }
            hSec = nullptr;
            while ((hSec = FindWindowEx(NULL, hSec, L"Shell_SecondaryTrayWnd", NULL)) != nullptr) {
                if (MonitorFromWindow(hSec, MONITOR_DEFAULTTONULL) == hMonitor) {
                    return hSec;
                }
            }
        }
    }
    return hPrimary;
}

void UpdateTaskbarEdge(DockAnimationContext& ctx) {
    auto frame = ctx.taskbarFrame.get();
    if (!frame) return;

    HWND hWnd = ctx.hWnd;
    if (!hWnd || !IsWindow(hWnd)) {
        hWnd = GetCurrentThreadTrayWindow();
        ctx.hWnd = hWnd;
    }

    if (!hWnd) {
        if (frame.ActualWidth() < frame.ActualHeight()) {
            ctx.edge = TaskbarEdge::Left;
        } else {
            ctx.edge = TaskbarEdge::Bottom;
        }
        ctx.isVertical = (ctx.edge == TaskbarEdge::Left || ctx.edge == TaskbarEdge::Right);
        return;
    }

    RECT rect = {0};
    if (GetWindowRect(hWnd, &rect)) {
        if (rect.left == ctx.lastTrayRect.left &&
            rect.top == ctx.lastTrayRect.top &&
            rect.right == ctx.lastTrayRect.right &&
            rect.bottom == ctx.lastTrayRect.bottom) {
            return;
        }

        ctx.lastTrayRect = rect;

        LONG w = rect.right - rect.left;
        LONG h = rect.bottom - rect.top;
        if (w < h) {
            HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
            MONITORINFO mi = { sizeof(mi) };
            if (GetMonitorInfo(hMonitor, &mi)) {
                if (rect.left <= mi.rcMonitor.left + (mi.rcMonitor.right - mi.rcMonitor.left) / 2) {
                    ctx.edge = TaskbarEdge::Left;
                } else {
                    ctx.edge = TaskbarEdge::Right;
                }
            } else {
                ctx.edge = TaskbarEdge::Left;
            }
        } else {
            HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
            MONITORINFO mi = { sizeof(mi) };
            if (GetMonitorInfo(hMonitor, &mi)) {
                if (rect.top <= mi.rcMonitor.top + (mi.rcMonitor.bottom - mi.rcMonitor.top) / 2) {
                    ctx.edge = TaskbarEdge::Top;
                } else {
                    ctx.edge = TaskbarEdge::Bottom;
                }
            } else {
                ctx.edge = TaskbarEdge::Bottom;
            }
        }
        ctx.isVertical = (ctx.edge == TaskbarEdge::Left || ctx.edge == TaskbarEdge::Right);
    }
}

// Simple visual tree helpers
FrameworkElement EnumChildElements(
    FrameworkElement element,
    std::function<bool(FrameworkElement)> enumCallback) {
    int childrenCount = VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < childrenCount; i++) {
        auto child = VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();
        if (!child) continue;
        if (enumCallback(child)) return child;
    }
    return nullptr;
}

FrameworkElement FindChildByClassName(FrameworkElement element,
                                          PCWSTR className) {
    if (!element) return nullptr;
    return EnumChildElements(element, [className](FrameworkElement child) {
        return winrt::get_class_name(child) == className;
    });
}

// Animation math (cosine falloff)
double CalculateScale(double distance, double radius, double maxScale) {
    if (distance > radius) return 1.0;
    
    double t = distance / radius;
    double factor = 0.0;
    
    switch (g_settings.animationType) {
        case 1: // Linear
            factor = 1.0 - t;
            break;
        case 2: // Exponential
            factor = pow(1.0 - t, 3);
            break;
        case 0: // Cosine
        default:
            factor = (cos(t * 3.14159) + 1.0) / 2.0;
            break;
    }
    if (factor < 0) factor = 0;
    return (maxScale - 1.0) * factor + 1.0;
}

template <typename Setter>
static void WriteIfChanged(double& applied, double value, double epsilon, Setter&& set) {
    if (std::abs(applied - value) < epsilon) return;  // false for NaN: always writes
    set(value);
    applied = value;
}

/// Rest-size mode packs icons at their own pitch and shrinks them; it needs a
// horizontal taskbar where every button is managed by the dock.
static bool RestSizeActive(DockAnimationContext const& ctx) {
    return g_settings.restIconSize > 0 && !ctx.isVertical &&
           g_settings.excludeSystemButtonsMode == 0;
}

// Sizes the background (the dock pill). Its margins hold the rest extent,
// a layout change, so they only move when the icons change. While icons
// magnify, it is stretched to [left, right] with a render transform instead:
// a layout pass every frame re-triggers the taskbar's auto-hide logic.
static void ApplyBackgroundExtent(DockAnimationContext& ctx, double restLeft, double restRight,
                                  double left, double right) {
    auto bg = ctx.background.get();
    if (!bg || !ctx.backgroundBaseKnown || restRight <= restLeft) return;

    double marginLeft = restLeft - ctx.backgroundSlotLeft;
    double marginRight = ctx.backgroundSlotRight - restRight;
    if (!(std::abs(marginLeft - ctx.appliedBackgroundLeft) < 0.5 &&
          std::abs(marginRight - ctx.appliedBackgroundRight) < 0.5)) {
        Thickness m = bg.Margin();
        m.Left = marginLeft;
        m.Right = marginRight;
        bg.Margin(m);
        ctx.appliedBackgroundLeft = marginLeft;
        ctx.appliedBackgroundRight = marginRight;
    }

    if (!ctx.backgroundScale) {
        // The stretch goes on the background's first child (the pill's
        // layers in a dock theme): Windows replaces the TaskbarBackground's
        // own RenderTransform every frame.
        auto layer = VisualTreeHelper::GetChildrenCount(bg) > 0
                         ? VisualTreeHelper::GetChild(bg, 0).try_as<FrameworkElement>()
                         : nullptr;
        if (!layer) return;
        TransformGroup tg;
        ctx.backgroundScale = ScaleTransform();
        ctx.backgroundTranslate = TranslateTransform();
        tg.Children().Append(ctx.backgroundScale);
        tg.Children().Append(ctx.backgroundTranslate);
        layer.RenderTransform(tg);
        layer.RenderTransformOrigin({0.5f, 0.5f});
        ctx.backgroundLayer = layer;
        ctx.appliedBackgroundScale = NAN;
        ctx.appliedBackgroundShift = NAN;
    }
    double scale = (right - left) / (restRight - restLeft);
    double shift = (left + right) / 2.0 - (restLeft + restRight) / 2.0;
    WriteIfChanged(ctx.appliedBackgroundScale, scale, SCALE_EPSILON / 10, [&](double v) {
        ctx.backgroundScale.ScaleX(v);
    });
    WriteIfChanged(ctx.appliedBackgroundShift, shift, TRANSLATE_EPSILON / 10, [&](double v) {
        ctx.backgroundTranslate.X(v);
    });
}

// Advances every icon one frame toward its target and sizes the background
// to wrap them. Returns true while any icon is still moving, so the caller
// knows when the loop can stop.
//
// Icons are laid out by the dock, not by Windows: each gets a slot (its rest
// pitch, widened by its magnification), the slots are packed side by side
// around the center of Windows' layout, and each icon is translated from
// where Windows put it to the center of its slot. At intensity 0 this is the
// rest layout, which in rest-size mode is not the identity.
bool ApplyAnimation(DockAnimationContext& ctx, double dtSec) {
    const size_t n = ctx.icons.size();
    if (n == 0 || !ctx.taskbarFrame.get()) return false;

    const bool restMode = RestSizeActive(ctx);
    const double spacingFactor = g_settings.spacingFactor;
    const double mouseX = ctx.mousePos == -1.0 ? 0.0 : ctx.mousePos;
    const double intensity = ctx.intensity;

    ctx.scales.assign(n, 1.0);
    ctx.restSlots.assign(n, 0.0);
    ctx.restCenters.assign(n, 0.0);

    // Rest slots, packed around the center of Windows' layout.
    const double dockCenter =
        (ctx.icons.front().originalCenterX + ctx.icons.back().originalCenterX +
         (g_settings.taskbarLabelsMode
              ? ctx.icons.back().elementWidth
              : 0.0)) / 2.0;
    double restTotal = 0.0;
    for (size_t i = 0; i < n; i++) {
        ctx.restSlots[i] = restMode ? g_settings.restIconSize + g_settings.iconGap
                                    : ctx.icons[i].elementWidth;
        restTotal += ctx.restSlots[i];
    }
    {
        double cum = dockCenter - restTotal / 2.0;
        for (size_t i = 0; i < n; i++) {
            ctx.restCenters[i] = cum + ctx.restSlots[i] / 2.0;
            cum += ctx.restSlots[i];
        }
    }

    // Magnification from the distance to each icon's rest position, so the
    // wave doesn't chase the icons as they spread.
    size_t closestIconIndex = (size_t)-1;
    double minDistance = g_settings.effectRadius + 1.0;
    for (size_t i = 0; i < n; i++) {
        double distance;
        if (g_settings.taskbarLabelsMode && !restMode) {
            double iconStart = ctx.restCenters[i] - ctx.restSlots[i] / 2.0;
            double iconEnd = iconStart + ctx.restSlots[i];
            distance = mouseX < iconStart ? iconStart - mouseX
                     : mouseX > iconEnd   ? mouseX - iconEnd
                                          : 0.0;
        } else {
            distance = std::abs(mouseX - ctx.restCenters[i]);
        }
        ctx.scales[i] = CalculateScale(distance, (double)g_settings.effectRadius, g_settings.maxScale);
        if (distance < minDistance) {
            minDistance = distance;
            closestIconIndex = i;
        }
    }

    double bounceScaleFactor = 1.0;
    double bounceTranslateOffset = 0.0;

    const double currentBounceIntensity = ctx.bounceIntensity;

    if (currentBounceIntensity > 0.0 && closestIconIndex != (size_t)-1 &&
        minDistance < (g_settings.effectRadius / 2.0)) {
        double elapsed_ms = std::chrono::duration<double, std::milli>(
                                std::chrono::steady_clock::now() - ctx.bounceStartTime)
                                .count();
        double t = std::fmod(elapsed_ms, BOUNCE_PERIOD_MS) / BOUNCE_PERIOD_MS;
        double normalizedWave = std::sin(t * 3.14159);
        bounceScaleFactor = 1.0 + (normalizedWave * BOUNCE_SCALE_AMOUNT * currentBounceIntensity);
        bounceTranslateOffset = normalizedWave * BOUNCE_TRANSLATE_Y * currentBounceIntensity;

        if (ctx.isVertical) {
            if (ctx.edge == TaskbarEdge::Left) {
                bounceTranslateOffset *= -1.0;
            }
        } else {
            if (ctx.edge == TaskbarEdge::Top) {
                bounceTranslateOffset *= -1.0;
            }
        }
    }

    double alpha = 1.0;
    if (g_settings.lerpSpeed > 0.0) {
        alpha = 1.0 - std::exp(-g_settings.lerpSpeed * dtSec);
    }
    alpha = std::clamp(alpha, 0.0, 1.0);

    // Target slots at the current intensity, packed around the same center.
    double total = 0.0;
    for (size_t i = 0; i < n; i++) {
        double m = 1.0 + (ctx.scales[i] - 1.0) * intensity;
        ctx.scales[i] = m;
        total += ctx.restSlots[i] * (1.0 + (m - 1.0) * spacingFactor);
    }

    const bool vertical = ctx.isVertical;
    double cum = dockCenter - total / 2.0;
    bool moving = false;
    double visualLeft = INFINITY;
    double visualRight = -INFINITY;

    for (size_t i = 0; i < n; i++) {
        auto& icon = ctx.icons[i];
        auto& tf = icon.tf;
        double slot = ctx.restSlots[i] * (1.0 + (ctx.scales[i] - 1.0) * spacingFactor);
        double targetScale = ctx.scales[i];
        double targetTranslate = cum + slot / 2.0 - icon.originalCenterX;
        cum += slot;
        if (!tf.baseScale || !tf.waveScale || !tf.waveTranslate || !tf.bounceScale || !tf.bounceTranslate) continue;

        auto& state = icon.state;
        if (g_settings.lerpSpeed > 0.0 && !std::isnan(state.waveTranslateX)) {
            state.waveScale += (targetScale - state.waveScale) * alpha;
            state.waveTranslateX += (targetTranslate - state.waveTranslateX) * alpha;
        } else {
            state.waveScale = targetScale;
            state.waveTranslateX = targetTranslate;
        }

        // Snap the last invisible fraction of the lerp so the icon settles.
        if (std::abs(targetScale - state.waveScale) < SCALE_EPSILON &&
            std::abs(targetTranslate - state.waveTranslateX) < TRANSLATE_EPSILON) {
            state.waveScale = targetScale;
            state.waveTranslateX = targetTranslate;
        } else {
            moving = true;
        }

        double base = restMode ? icon.baseScale : 1.0;
        WriteIfChanged(icon.applied.baseScale, base, SCALE_EPSILON / 10, [&](double v) {
            tf.baseScale.ScaleX(v);
            tf.baseScale.ScaleY(v);
        });
        WriteIfChanged(icon.applied.waveScale, state.waveScale, SCALE_EPSILON / 10, [&](double v) {
            tf.waveScale.ScaleX(v);
            tf.waveScale.ScaleY(v);
        });
        WriteIfChanged(icon.applied.waveTranslate, state.waveTranslateX, TRANSLATE_EPSILON / 10, [&](double v) {
            if (vertical) tf.waveTranslate.Y(v); else tf.waveTranslate.X(v);
        });

        double bScale = 1.0;
        double bTranslate = 0.0;
        if (i == closestIconIndex && currentBounceIntensity > 0.0) {
            bScale = bounceScaleFactor;
            if (!g_settings.disableBounce && !g_settings.disableVerticalBounce) {
                bTranslate = bounceTranslateOffset;
            }
        }
        WriteIfChanged(icon.applied.bounceScale, bScale, SCALE_EPSILON / 10, [&](double v) {
            tf.bounceScale.ScaleX(v);
            tf.bounceScale.ScaleY(v);
        });
        WriteIfChanged(icon.applied.bounceTranslate, bTranslate, TRANSLATE_EPSILON / 10, [&](double v) {
            if (vertical) tf.bounceTranslate.X(v); else tf.bounceTranslate.Y(v);
        });

        // Visual extent of this icon's slot as currently drawn.
        double drawnSlot = ctx.restSlots[i] * (1.0 + (state.waveScale - 1.0) * spacingFactor);
        double drawnCenter = icon.originalCenterX + state.waveTranslateX;
        visualLeft = std::min(visualLeft, drawnCenter - drawnSlot / 2.0);
        visualRight = std::max(visualRight, drawnCenter + drawnSlot / 2.0);
    }

    if (!vertical && visualLeft < visualRight) {
        // The slots already include the gap; pad by what's left of it.
        double pad = g_settings.dockPadding - (restMode ? g_settings.iconGap / 2.0 : 0.0);
        ApplyBackgroundExtent(ctx, dockCenter - restTotal / 2.0 - pad, dockCenter + restTotal / 2.0 + pad,
                              visualLeft - pad, visualRight + pad);
    }

    return moving;
}

// Back to Windows' own layout: identity transforms and the theme's margin.
void ResetAllIconScales(std::vector<TaskbarIconInfo>& icons) {
    if (icons.empty()) return;
    try {
        for (auto& iconInfo : icons) {
            auto& tf = iconInfo.tf;
            for (ScaleTransform s : {tf.baseScale, tf.waveScale, tf.bounceScale}) {
                if (s) {
                    s.ScaleX(1.0);
                    s.ScaleY(1.0);
                }
            }
            for (TranslateTransform t : {tf.waveTranslate, tf.bounceTranslate}) {
                if (t) {
                    t.X(0.0);
                    t.Y(0.0);
                }
            }
            iconInfo.state = IconAnimState{};
            iconInfo.applied = AppliedValues{1.0, 1.0, 0.0, 1.0, 0.0};
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: HRESULT error in ResetAllIconScales: %s", e.message().c_str());
    }
}

static void RestoreBackground(DockAnimationContext& ctx) {
    if (auto bg = ctx.background.get(); bg && ctx.backgroundBaseKnown) {
        bg.Margin(ctx.backgroundBaseMargin);
    }
    if (auto layer = ctx.backgroundLayer.get(); layer && ctx.backgroundScale) {
        layer.RenderTransform(nullptr);
    }
    ctx.backgroundScale = nullptr;
    ctx.backgroundTranslate = nullptr;
    ctx.appliedBackgroundLeft = NAN;
    ctx.appliedBackgroundRight = NAN;
}

// Subscribes this taskbar's frame callback. Must run on the taskbar's thread.
void StartRendering(DockAnimationContext& ctx, void* key) {
    if (ctx.isRenderingHooked) return;
    ctx.lastRenderTime = std::chrono::steady_clock::now();
    ctx.renderingToken = Media::CompositionTarget::Rendering(
        [key](winrt::Windows::Foundation::IInspectable const&,
              winrt::Windows::Foundation::IInspectable const&) { OnRendering(key); });
    ctx.isRenderingHooked = true;
}

void StopRendering(DockAnimationContext& ctx) {
    if (!ctx.isRenderingHooked) return;
    ctx.isRenderingHooked = false;
    Media::CompositionTarget::Rendering(ctx.renderingToken);
}

// Event handlers
void OnTaskbarPointerMoved(DockAnimationContext& ctx, void* key, Input::PointerRoutedEventArgs const& args) {
    try {
        auto frame = ctx.taskbarFrame.get();
        if (!frame) return;
        ctx.isMouseInside = true;

        UpdateTaskbarEdge(ctx);

        auto position = args.GetCurrentPoint(frame).Position();
        double mousePos = ctx.isVertical ? position.Y : position.X;

        if (std::abs(mousePos - ctx.mousePos) > MOUSE_STOP_THRESHOLD) {
            ctx.lastSignificantMoveTime = std::chrono::steady_clock::now();
        }
        ctx.mousePos = mousePos;
        if (!ctx.isRenderingHooked) {
            Wh_Log(L"DockAnimation: pointer at %.1f, waking (icons=%zu intensity=%.2f)",
                   mousePos, ctx.icons.size(), ctx.intensity);
            StartRendering(ctx, key);
            ctx.lastSignificantMoveTime = std::chrono::steady_clock::now();
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: HRESULT Error in OnTaskbarPointerMoved: %s", e.message().c_str());
    }
}

void OnTaskbarPointerExited(DockAnimationContext& ctx, void* key) {
    try {
        ctx.isMouseInside = false;
        ctx.isBouncing = false;
        Wh_Log(L"DockAnimation: pointer exited (intensity=%.2f)", ctx.intensity);
        // The loop may be asleep on a settled dock; wake it to animate out.
        StartRendering(ctx, key);
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: HRESULT Error in OnTaskbarPointerExited: %s", e.message().c_str());
    }
}

FrameworkElement FindChildByClassNamePartial(FrameworkElement element, PCWSTR partialName) {
    if (!element) return nullptr;
    return EnumChildElements(element, [partialName](FrameworkElement child) {
        auto className = winrt::get_class_name(child);
        return std::wstring_view(className).find(partialName) != std::wstring_view::npos;
    });
}

static FrameworkElement FindIconHost(FrameworkElement const& taskbarFrame) {
    FrameworkElement host = FindChildByClassNamePartial(taskbarFrame, L"TaskbarFrameRepeater");
    if (!host) host = FindChildByClassName(taskbarFrame, L"TaskbarFrameRepeater");
    if (!host) host = FindChildByClassName(taskbarFrame, L"TaskbarItemHost");
    return host;
}

static uint64_t Fnv1aMix(uint64_t h, uint64_t v) {
    h ^= v;
    h *= 1099511628211ULL;
    return h;
}

static HostSignature ComputeHostSignature(FrameworkElement const& host, bool isVertical) {
    HostSignature sig;
    sig.count = VisualTreeHelper::GetChildrenCount(host);
    sig.width = isVertical ? host.ActualHeight() : host.ActualWidth();

    uint64_t h = 1469598103934665603ULL;
    for (int i = 0; i < sig.count; i++) {
        auto child = VisualTreeHelper::GetChild(host, i).try_as<FrameworkElement>();
        if (!child) continue;
        h = Fnv1aMix(h, (uint64_t)winrt::get_abi(child));
        double size = isVertical ? child.ActualHeight() : child.ActualWidth();
        h = Fnv1aMix(h, (uint64_t)(size * 10.0));
    }
    sig.orderHash = h;
    return sig;
}

static bool SigDifferent(HostSignature const& a, HostSignature const& b) {
    if (a.count != b.count) return true;
    if (std::abs(a.width - b.width) > 0.5) return true;
    if (a.orderHash != b.orderHash) return true;
    return false;
}

enum class ButtonKind {
    App,
    Start,
    Search,
    TaskView,
    Widgets,
    Weather,
    SystemOther
};

static std::wstring W(winrt::hstring const& s) {
    return std::wstring(s.c_str());
}

static std::wstring ToLower(std::wstring s) {
    for (auto& ch : s) ch = (wchar_t)towlower(ch);
    return s;
}

static ButtonKind ClassifyButton(FrameworkElement const& e) {
    if (!e) return ButtonKind::SystemOther;

    std::wstring cn     = W(winrt::get_class_name(e));
    std::wstring feName = W(e.Name());
    std::wstring aid    = W(AutomationProperties::GetAutomationId(e));

    std::wstring cnL = ToLower(cn);
    std::wstring aidL = ToLower(aid);
    std::wstring feL = ToLower(feName);

    auto matches = [&](const wchar_t* s) {
        return cnL.find(s) != std::wstring::npos ||
               aidL.find(s) != std::wstring::npos ||
               feL.find(s) != std::wstring::npos;
    };

    if (matches(L"startbutton")) return ButtonKind::Start;
    if (matches(L"searchbutton")) return ButtonKind::Search;
    if (matches(L"taskviewbutton")) return ButtonKind::TaskView;
    if (matches(L"widgetsbutton")) return ButtonKind::Widgets;
    if (matches(L"weather")) return ButtonKind::Weather;

    if (matches(L"appid:")) return ButtonKind::App;

    {
        if (cnL == L"taskbar.tasklistbutton" ||
            (cnL.size() >= 13 && cnL.rfind(L".tasklistbutton") == cnL.size() - 13)) {
            return ButtonKind::App;
        }
    }

    return ButtonKind::SystemOther;
}


static bool ShouldAnimateElement(FrameworkElement const& e) {
    const int mode = g_settings.excludeSystemButtonsMode;
    if (mode == 0) return true;

    ButtonKind k = ClassifyButton(e);

    if (mode == 2) return k == ButtonKind::App;
    if (mode == 1) return k != ButtonKind::Start;

    return true;
}

static void ResetElementTransforms(FrameworkElement const& element) {
    auto tg = element.RenderTransform().try_as<TransformGroup>();
    if (!tg) return;
    for (auto const& child : tg.Children()) {
        if (auto s = child.try_as<ScaleTransform>()) {
            s.ScaleX(1.0);
            s.ScaleY(1.0);
        } else if (auto t = child.try_as<TranslateTransform>()) {
            t.X(0.0);
            t.Y(0.0);
        }
    }
}

// Breadth-first search for a named descendant, a few levels deep.
static FrameworkElement FindNamedDescendant(FrameworkElement const& root, PCWSTR name, int maxNodes = 64) {
    std::vector<FrameworkElement> queue{root};
    for (size_t i = 0; i < queue.size() && (int)i < maxNodes; i++) {
        int c = VisualTreeHelper::GetChildrenCount(queue[i]);
        for (int j = 0; j < c; j++) {
            auto ch = VisualTreeHelper::GetChild(queue[i], j).try_as<FrameworkElement>();
            if (!ch) continue;
            if (ch.Name() == name) return ch;
            queue.push_back(ch);
        }
    }
    return nullptr;
}

// Reads where Windows put the button and its icon, and sets the scale
// pivots: the rest-size scale shrinks the icon around its own center, and
// magnification grows it from its rest bottom edge, the way macOS does.
// Returns true if the button moved or resized.
static bool UpdateIconGeometry(DockAnimationContext& ctx, TaskbarIconInfo& info,
                               FrameworkElement const& e, FrameworkElement const& frame) {
    double oldX = info.originalCenterX;
    double oldW = info.elementWidth;

    // Where Windows laid the button out. TransformToVisual on the button
    // itself would include the dock's own transforms and feed them back in,
    // so measure its layout offset from the (untransformed) parent instead.
    winrt::Windows::Foundation::Point pt;
    if (auto parent = VisualTreeHelper::GetParent(e).try_as<UIElement>()) {
        auto offset = e.ActualOffset();
        pt = parent.TransformToVisual(frame).TransformPoint({offset.x, offset.y});
    } else {
        pt = e.TransformToVisual(frame).TransformPoint({0, 0});
    }
    double width = e.ActualWidth();
    double height = e.ActualHeight();
    double along = ctx.isVertical ? height : width;
    info.elementWidth = along;

    if (g_settings.taskbarLabelsMode) {
        info.originalCenterX = ctx.isVertical ? pt.Y : pt.X;
    } else {
        info.originalCenterX = (ctx.isVertical ? pt.Y : pt.X) + along / 2.0;
    }

    info.iconSize = 0.0;
    info.iconCenterY = height / 2.0;
    if (auto icon = FindNamedDescendant(e, L"Icon")) {
        double size = icon.ActualHeight();
        if (size > 0) {
            info.iconSize = size;
            info.iconCenterY = icon.TransformToVisual(e).TransformPoint({0, 0}).Y + size / 2.0;
        }
    }
    info.baseScale = (g_settings.restIconSize > 0 && info.iconSize > 0)
                         ? g_settings.restIconSize / info.iconSize
                         : 1.0;

    // Pivots in element coordinates (origin at the top-left).
    double baseX = g_settings.taskbarLabelsMode ? 24.0 : width / 2.0;
    double baseY = info.iconCenterY;
    double pivotX = baseX;
    double pivotY;
    if (ctx.isVertical) {
        pivotX = (ctx.edge == TaskbarEdge::Left) ? 0.0 : width;
        pivotY = height / 2.0;
    } else if (g_settings.disableVerticalBounce) {
        pivotY = height / 2.0;
    } else if (ctx.edge == TaskbarEdge::Top) {
        pivotY = RestSizeActive(ctx) && info.iconSize > 0
                     ? info.iconCenterY - info.baseScale * info.iconSize / 2.0
                     : 0.0;
    } else {
        pivotY = RestSizeActive(ctx) && info.iconSize > 0
                     ? info.iconCenterY + info.baseScale * info.iconSize / 2.0
                     : height;
    }

    e.RenderTransformOrigin({0.0f, 0.0f});
    if (info.tf.baseScale) {
        info.tf.baseScale.CenterX(baseX);
        info.tf.baseScale.CenterY(baseY);
    }
    for (ScaleTransform s : {info.tf.waveScale, info.tf.bounceScale}) {
        if (s) {
            s.CenterX(pivotX);
            s.CenterY(pivotY);
        }
    }

    return std::abs(info.originalCenterX - oldX) > 0.5 || std::abs(along - oldW) > 0.5;
}

// The frame's background (the dock pill) and the layout slot its margins
// are measured from.
static void UpdateBackgroundGeometry(DockAnimationContext& ctx, FrameworkElement const& frame) {
    auto bg = ctx.background.get();
    if (!bg) {
        auto rootGrid = FindNamedDescendant(frame, L"RootGrid", 8);
        if (!rootGrid) return;
        bg = FindChildByClassName(rootGrid, L"Taskbar.TaskbarBackground");
        if (!bg) return;
        ctx.background = bg;
        ctx.backgroundBaseMargin = bg.Margin();
        ctx.backgroundBaseKnown = true;
        ctx.appliedBackgroundLeft = NAN;
        ctx.appliedBackgroundRight = NAN;
    }
    // Layout position only: TransformToVisual on bg would include the
    // stretch transform the dock puts on it.
    winrt::Windows::Foundation::Point p;
    if (auto parent = VisualTreeHelper::GetParent(bg).try_as<UIElement>()) {
        auto offset = bg.ActualOffset();
        p = parent.TransformToVisual(frame).TransformPoint({offset.x, offset.y});
    } else {
        p = bg.TransformToVisual(frame).TransformPoint({0, 0});
    }
    Thickness m = bg.Margin();
    ctx.backgroundSlotLeft = p.X - m.Left;
    ctx.backgroundSlotRight = p.X + bg.ActualWidth() + m.Right;
}

static bool RebaseIconGeometryFast(DockAnimationContext& ctx) {
    auto frame = ctx.taskbarFrame.get();
    if (!frame) return false;

    bool changed = false;

    for (auto it = ctx.icons.begin(); it != ctx.icons.end(); ) {
        if (!it->element.get()) {
            it = ctx.icons.erase(it);
            changed = true;
        } else {
            ++it;
        }
    }

    for (auto& info : ctx.icons) {
        auto e = info.element.get();
        if (!e) continue;
        try {
            if (UpdateIconGeometry(ctx, info, e, frame)) changed = true;
        } catch (...) {
        }
    }

    try {
        UpdateBackgroundGeometry(ctx, frame);
    } catch (...) {
    }

    if (changed && ctx.icons.size() > 1) {
        std::sort(ctx.icons.begin(), ctx.icons.end(),
            [](TaskbarIconInfo const& a, TaskbarIconInfo const& b) {
                return a.originalCenterX < b.originalCenterX;
            });
    }

    return changed;
}

// Builds or refreshes the icon list and sets up transforms for animation (scale + translate).
void RefreshIconPositions(DockAnimationContext& ctx) {
    // preserve state
    std::unordered_map<void*, IconAnimState> prevStates;
    std::unordered_set<void*> prevElements;

    for (auto& old : ctx.icons) {
        if (auto e = old.element.get()) {
            void* key = winrt::get_abi(e);
            prevStates[key] = old.state;
            prevElements.insert(key);
        }
    }

    std::vector<TaskbarIconInfo> newIcons;

    auto taskbarFrame = ctx.taskbarFrame.get();
    if (!taskbarFrame) return;

    auto IsTaskListButtonExact = [&](FrameworkElement const& e) -> bool {
        if (!e) return false;
        return winrt::get_class_name(e) == L"Taskbar.TaskListButton";
    };

    auto IsTaskListButtonPanel = [&](FrameworkElement const& e) -> bool {
        if (!e) return false;
        return winrt::get_class_name(e) == L"Taskbar.TaskListButtonPanel";
    };

    std::function<FrameworkElement(FrameworkElement)> FindFirstTaskListButton =
        [&](FrameworkElement root) -> FrameworkElement {
            if (!root) return nullptr;
            if (IsTaskListButtonExact(root)) return root;

            int c = VisualTreeHelper::GetChildrenCount(root);
            for (int i = 0; i < c; i++) {
                auto ch = VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
                if (!ch) continue;
                if (IsTaskListButtonExact(ch)) return ch;
                auto deep = FindFirstTaskListButton(ch);
                if (deep) return deep;
            }
            return nullptr;
        };

    // classifier node
    std::function<FrameworkElement(FrameworkElement)> FindClassifierNode =
        [&](FrameworkElement root) -> FrameworkElement {
            if (!root) return nullptr;

            auto aid = W(AutomationProperties::GetAutomationId(root));
            auto nm  = W(AutomationProperties::GetName(root));
            if (!aid.empty() || !nm.empty()) return root;

            int c = VisualTreeHelper::GetChildrenCount(root);
            for (int i = 0; i < c; i++) {
                auto ch = VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
                if (!ch) continue;
                auto hit = FindClassifierNode(ch);
                if (hit) return hit;
            }
            return nullptr;
        };

    // candidate in fallback: TaskListButton + TaskListButtonPanel
    auto IsFallbackCandidate = [&](FrameworkElement const& e) -> bool {
        if (!e) return false;

        if (IsTaskListButtonExact(e) || IsTaskListButtonPanel(e)) return true;

        auto cn = winrt::get_class_name(e);
        if (std::wstring_view(cn).find(L"Taskbar.") != std::wstring_view::npos) {
            auto aid = W(AutomationProperties::GetAutomationId(e));
            auto nm  = W(AutomationProperties::GetName(e));
            if (!aid.empty() || !nm.empty()) return true;
        }

        return false;
    };

    auto SetupAndAddElement = [&](FrameworkElement const& element) {
        try {
            if (!element) return;

            // Collapsed buttons (hidden Task View, Widgets...) have no size;
            // giving them a slot would leave a gap in the dock.
            if (element.ActualWidth() < 1 || element.ActualHeight() < 1) {
                if (prevElements.contains(winrt::get_abi(element))) ResetElementTransforms(element);
                return;
            }

            auto tg = element.RenderTransform().try_as<TransformGroup>();
            if (!tg || tg.Children().Size() != TRANSFORM_COUNT) {
                tg = TransformGroup();
                tg.Children().Append(ScaleTransform());       // 0 baseScale
                tg.Children().Append(ScaleTransform());       // 1 waveScale
                tg.Children().Append(TranslateTransform());   // 2 waveTranslate
                tg.Children().Append(ScaleTransform());       // 3 bounceScale
                tg.Children().Append(TranslateTransform());   // 4 bounceTranslate
                element.RenderTransform(tg);
            }

            TaskbarIconInfo info;
            info.element = element;
            info.tf.baseScale = tg.Children().GetAt(0).try_as<ScaleTransform>();
            info.tf.waveScale = tg.Children().GetAt(1).try_as<ScaleTransform>();
            info.tf.waveTranslate = tg.Children().GetAt(2).try_as<TranslateTransform>();
            info.tf.bounceScale = tg.Children().GetAt(3).try_as<ScaleTransform>();
            info.tf.bounceTranslate = tg.Children().GetAt(4).try_as<TranslateTransform>();
            // Per-frame writes only touch the axis along the taskbar; clear
            // the other one in case the taskbar changed orientation.
            if (info.tf.waveTranslate) {
                if (ctx.isVertical) info.tf.waveTranslate.X(0.0); else info.tf.waveTranslate.Y(0.0);
            }
            if (info.tf.bounceTranslate) {
                if (ctx.isVertical) info.tf.bounceTranslate.Y(0.0); else info.tf.bounceTranslate.X(0.0);
            }

            UpdateIconGeometry(ctx, info, element, taskbarFrame);

            void* key = winrt::get_abi(element);

            auto it = prevStates.find(key);
            if (it != prevStates.end()) {
                info.state = it->second;
            } else {
                // A new button jumps straight to its slot instead of sliding
                // in from where Windows placed it.
                info.state.waveTranslateX = NAN;
            }

            newIcons.push_back(info);
        } catch (winrt::hresult_error const& e) {
            Wh_Log(L"DockAnimation: HRESULT error in SetupAndAddElement: %s", e.message().c_str());
        }
    };

    // SMART host find (structure-based)
    auto FindSmartHost = [&]() -> FrameworkElement {
        std::vector<FrameworkElement> nodes;
        nodes.reserve(512);

        std::vector<FrameworkElement> stack;
        stack.reserve(512);
        stack.push_back(taskbarFrame);

        while (!stack.empty()) {
            auto cur = stack.back();
            stack.pop_back();
            if (!cur) continue;

            nodes.push_back(cur);

            int c = VisualTreeHelper::GetChildrenCount(cur);
            for (int i = 0; i < c; i++) {
                auto ch = VisualTreeHelper::GetChild(cur, i).try_as<FrameworkElement>();
                if (ch) stack.push_back(ch);
            }
        }

        std::function<bool(FrameworkElement,int)> SubtreeHasIconLike =
            [&](FrameworkElement root, int depth) -> bool {
                if (!root || depth <= 0) return false;
                if (IsTaskListButtonExact(root) || IsTaskListButtonPanel(root)) return true;

                int c = VisualTreeHelper::GetChildrenCount(root);
                for (int i = 0; i < c; i++) {
                    auto ch = VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
                    if (!ch) continue;
                    if (SubtreeHasIconLike(ch, depth - 1)) return true;
                }
                return false;
            };

        FrameworkElement best = nullptr;
        int bestHits = 0;
        double bestWidth = 0.0;

        for (auto const& n : nodes) {
            int c = VisualTreeHelper::GetChildrenCount(n);
            if (c < 3 || c > 120) continue;

            int hits = 0;
            for (int i = 0; i < c; i++) {
                auto ch = VisualTreeHelper::GetChild(n, i).try_as<FrameworkElement>();
                if (!ch) continue;
                if (SubtreeHasIconLike(ch, 5)) hits++;
            }

            if (hits >= 3) {
                double w = (double)n.ActualWidth();
                if (hits > bestHits || (hits == bestHits && w > bestWidth)) {
                    best = n;
                    bestHits = hits;
                    bestWidth = w;
                }
            }
        }

        return best;
    };

    // host selection
    auto host = ctx.iconHost.get();
    if (!host) {
        host = FindSmartHost();
        if (host) ctx.iconHost = host;
    }

    try {
        if (host) {
            // MAIN: enumerate host children
            int count = VisualTreeHelper::GetChildrenCount(host);
            for (int i = 0; i < count; i++) {
                auto child = VisualTreeHelper::GetChild(host, i).try_as<FrameworkElement>();
                if (!child) continue;

                FrameworkElement classifier = FindClassifierNode(child);
                FrameworkElement base = classifier ? classifier : child;

                bool animate = (g_settings.excludeSystemButtonsMode == 0)
                    ? true
                    : ShouldAnimateElement(base);

                // target: prefer TaskListButton inside, else child
                FrameworkElement target = FindFirstTaskListButton(child);
                if (!target) target = child;

                if (!animate) {
                    void* kT = winrt::get_abi(target);
                    if (prevElements.contains(kT)) ResetElementTransforms(target);

                    void* kC = winrt::get_abi(child);
                    if (prevElements.contains(kC)) ResetElementTransforms(child);
                    continue;
                }

                SetupAndAddElement(target);
            }

            ctx.lastSig = ComputeHostSignature(host, ctx.isVertical);
        } else {
            // FALLBACK: recurse and include TaskListButtonPanel too
            std::function<void(FrameworkElement)> recurse = [&](FrameworkElement element) {
                if (!element) return;

                if (IsFallbackCandidate(element)) {
                    FrameworkElement classifier = FindClassifierNode(element);
                    FrameworkElement base = classifier ? classifier : element;

                    bool animate = (g_settings.excludeSystemButtonsMode == 0)
                        ? true
                        : ShouldAnimateElement(base);

                    FrameworkElement target = FindFirstTaskListButton(element);
                    if (!target) target = element;

                    if (!animate) {
                        void* k = winrt::get_abi(target);
                        if (prevElements.contains(k)) ResetElementTransforms(target);
                    } else {
                        SetupAndAddElement(target);
                    }
                }

                int c = VisualTreeHelper::GetChildrenCount(element);
                for (int i = 0; i < c; i++) {
                    auto ch = VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();
                    if (ch) recurse(ch);
                }
            };

            recurse(taskbarFrame);
        }

        std::sort(newIcons.begin(), newIcons.end(),
            [](TaskbarIconInfo const& a, TaskbarIconInfo const& b) {
                return a.originalCenterX < b.originalCenterX;
            });

        // Keep only real, visible buttons. The host also holds recycled
        // buttons parked ~10000px off-screen, helpers with no icon, and the
        // odd duplicate mid-animation; any of them would throw off the dock's
        // center and spacing.
        const double frameExtent = ctx.isVertical ? taskbarFrame.ActualHeight() : taskbarFrame.ActualWidth();
        std::vector<TaskbarIconInfo> kept;
        kept.reserve(newIcons.size());
        for (auto& ic : newIcons) {
            bool keep = ic.originalCenterX >= 0 && ic.originalCenterX <= frameExtent &&
                        ic.iconSize > 0 &&
                        (kept.empty() ||
                         ic.originalCenterX - kept.back().originalCenterX >= kept.back().elementWidth / 2.0);
            if (keep) {
                kept.push_back(std::move(ic));
            } else if (auto el = ic.element.get()) {
                ResetElementTransforms(el);
            }
        }

        ctx.icons = std::move(kept);
        UpdateBackgroundGeometry(ctx, taskbarFrame);
        Wh_Log(L"DockAnimation: refreshed %zu icons", ctx.icons.size());

    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: HRESULT error during icon search: %s", e.message().c_str());
    }
}

DockAnimationContext* InitializeAnimationHooks(void* pThis, FrameworkElement const& taskbarFrame) {
    try {
        DockAnimationContext ctx;
        ctx.key = pThis;
        ctx.taskbarFrame = taskbarFrame;
        ctx.isInitialized = false;
        ctx.hWnd = GetCurrentThreadTrayWindow();
        UpdateTaskbarEdge(ctx);
        Wh_Log(L"DockAnimation: Monitor %p registered (hook-based). Edge: %d, HWND: %p", pThis, (int)ctx.edge, ctx.hWnd);
        std::lock_guard<std::mutex> lock(g_contextsMutex);
        return &g_contexts.try_emplace(pThis, std::move(ctx)).first->second;
    }
    catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: Failed to initialize context for %p: %s",
               pThis, e.message().c_str());
    }
    return nullptr;
}

// Main animation loop, one per taskbar. Runs only while something is moving:
// it stops once the dock has faded out, or has settled under a resting cursor.
void OnRendering(void* key) {
    DockAnimationContext* pCtx = FindContext(key);
    if (!pCtx) return;
    auto& ctx = *pCtx;
    try {
        auto now = std::chrono::steady_clock::now();

        double deltaTime = std::chrono::duration<double, std::milli>(now - ctx.lastRenderTime).count();
        if (deltaTime > 25.0 && ctx.lastRenderTime != std::chrono::steady_clock::time_point{}) {
            Wh_Log(L"DockAnimation: slow frame %.1f ms", deltaTime);
        }
        deltaTime = std::clamp(deltaTime, 0.0, MAX_FRAME_MS);
        ctx.lastRenderTime = now;

        double dtSec = deltaTime / 1000.0;

        double intensityChange = deltaTime / g_settings.focusDuration;

        if (ctx.isMouseInside) {
            // FocusIn
            ctx.intensity = std::min(1.0, ctx.intensity + intensityChange);
        } else {
            // FocusOut
            ctx.intensity = std::max(0.0, ctx.intensity - intensityChange);
        }

        if (!ctx.isInitialized || ctx.needsRefresh) {
            UpdateTaskbarEdge(ctx);
            RefreshIconPositions(ctx);
            ctx.isInitialized = true;
            ctx.needsRefresh = false;
        }

        // Dirty-check
        if (ctx.isMouseInside) {
            auto sinceCheck = std::chrono::duration_cast<std::chrono::milliseconds>(
                now - ctx.lastDirtyCheck).count();

            if (sinceCheck > 120) {
                ctx.lastDirtyCheck = now;

                auto frame = ctx.taskbarFrame.get();
                if (frame) {
                    UpdateTaskbarEdge(ctx);

                    auto host = ctx.iconHost.get();
                    if (!host) {
                        host = FindIconHost(frame);
                        if (host) ctx.iconHost = host;
                    }

                    if (host) {
                        HostSignature sig = ComputeHostSignature(host, ctx.isVertical);

                        if (SigDifferent(sig, ctx.lastSig)) {
                            RefreshIconPositions(ctx);
                        } else {
                            RebaseIconGeometryFast(ctx);
                        }

                        ctx.lastSig = sig;
                    }
                }
            }
        }

        if (ctx.icons.empty()) {
            // Nothing laid out yet; the layout hook wakes the loop when
            // buttons appear.
            StopRendering(ctx);
            return;
        }

        if (g_settings.disableBounce) {
            ctx.isBouncing = false;
        } else {
            double elapsedSinceMove =
                std::chrono::duration<double, std::milli>(now - ctx.lastSignificantMoveTime).count();

            if (ctx.isMouseInside && elapsedSinceMove > g_settings.bounceDelay) {
                if (!ctx.isBouncing) {
                    ctx.isBouncing = true;
                    ctx.bounceStartTime = now;
                }
            } else {
                ctx.isBouncing = false;
            }
        }

        double targetBounceIntensity = ctx.isBouncing ? 1.0 : 0.0;
        double bounceFadeDuration = g_settings.focusDuration / 1000.0;
        double bounceIntensityChange = dtSec / bounceFadeDuration;
        if (targetBounceIntensity > ctx.bounceIntensity) {
            ctx.bounceIntensity = std::min(targetBounceIntensity, ctx.bounceIntensity + bounceIntensityChange);
        } else {
            ctx.bounceIntensity = std::max(targetBounceIntensity, ctx.bounceIntensity - bounceIntensityChange);
        }

        bool moving = ApplyAnimation(ctx, dtSec);

        // Back at rest after the cursor left: stop until the next pointer or
        // layout event. The icons keep their rest transforms.
        if (!moving && !ctx.isMouseInside && ctx.intensity <= 0.0 &&
            ctx.bounceIntensity <= 0.0) {
            StopRendering(ctx);
            ctx.mousePos = -1.0;
            ctx.isBouncing = false;
            return;
        }

        // Settled under a resting cursor: stop until the next pointer event.
        // With the bounce on, a resting cursor is exactly when it plays, so
        // the loop keeps running.
        if (!moving && ctx.isMouseInside && ctx.intensity >= 1.0 &&
            g_settings.disableBounce && ctx.bounceIntensity <= 0.0) {
            StopRendering(ctx);
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: frame failed: %08X %s", (unsigned)e.code(), e.message().c_str());
        try {
            StopRendering(ctx);
        } catch (...) {
        }
    } catch (std::exception const& e) {
        Wh_Log(L"DockAnimation: frame failed: %S", e.what());
        try {
            StopRendering(ctx);
        } catch (...) {
        }
    } catch (...) {
        Wh_Log(L"DockAnimation: frame failed (unknown exception)");
        try {
            StopRendering(ctx);
        } catch (...) {
        }
    }
}

// Hooking
// Pointer event hooks (new approach: activates mod immediately)
using TaskbarFrame_OnPointerMoved_t = int(WINAPI*)(void* pThis, void* pArgs);
TaskbarFrame_OnPointerMoved_t TaskbarFrame_OnPointerMoved_Original;

int WINAPI TaskbarFrame_OnPointerMoved_Hook(void* pThis, void* pArgs) {
    auto original = [=]() {
        return TaskbarFrame_OnPointerMoved_Original(pThis, pArgs);
    };
    FrameworkElement element = nullptr;
    ((IUnknown*)pThis)->QueryInterface(winrt::guid_of<FrameworkElement>(), winrt::put_abi(element));
    if (!element)
        return original();
    auto className = winrt::get_class_name(element);
    if (className != L"Taskbar.TaskbarFrame")
        return original();
    Input::PointerRoutedEventArgs args = nullptr;
    ((IUnknown*)pArgs)->QueryInterface(winrt::guid_of<Input::PointerRoutedEventArgs>(), winrt::put_abi(args));
    if (!args)
        return original();
    // Initialize animation hooks only once
    void* key = FrameKey(element);
    DockAnimationContext* ctx = FindContext(key);
    if (!ctx) {
        ctx = InitializeAnimationHooks(key, element);
        Wh_Log(L"DockAnimation: Initialized via OnPointerMoved (%s)", className.c_str());
    }
    // Forward event to existing logic
    if (ctx) OnTaskbarPointerMoved(*ctx, key, args);
    return original();
}

using TaskbarFrame_OnPointerExited_t = int(WINAPI*)(void* pThis, void* pArgs);
TaskbarFrame_OnPointerExited_t TaskbarFrame_OnPointerExited_Original;

int WINAPI TaskbarFrame_OnPointerExited_Hook(void* pThis, void* pArgs) {
    auto original = [=]() {
        return TaskbarFrame_OnPointerExited_Original(pThis, pArgs);
    };
    FrameworkElement element = nullptr;
    ((IUnknown*)pThis)->QueryInterface(winrt::guid_of<FrameworkElement>(), winrt::put_abi(element));
    if (!element)
        return original();
    auto className = winrt::get_class_name(element);
    if (className != L"Taskbar.TaskbarFrame")
        return original();
    void* key = FrameKey(element);
    if (DockAnimationContext* ctx = FindContext(key)) {
        OnTaskbarPointerExited(*ctx, key);
    }
    return original();
}

// Runs whenever Windows lays the taskbar out (startup, a button added or
// removed). The dock's rest layout isn't Windows' layout, so it has to be
// reapplied without waiting for the mouse.
using TaskbarFrame_ArrangeOverride_t =
    int(WINAPI*)(void* pThis, winrt::Windows::Foundation::Size finalSize, winrt::Windows::Foundation::Size* result);
TaskbarFrame_ArrangeOverride_t TaskbarFrame_ArrangeOverride_Original;

int WINAPI TaskbarFrame_ArrangeOverride_Hook(void* pThis,
                                             winrt::Windows::Foundation::Size finalSize,
                                             winrt::Windows::Foundation::Size* result) {
    int ret = TaskbarFrame_ArrangeOverride_Original(pThis, finalSize, result);
    try {
        FrameworkElement element = nullptr;
        ((IUnknown*)pThis)->QueryInterface(winrt::guid_of<FrameworkElement>(), winrt::put_abi(element));
        if (!element || winrt::get_class_name(element) != L"Taskbar.TaskbarFrame") return ret;

        void* key = FrameKey(element);
        DockAnimationContext* ctx = FindContext(key);
        if (!ctx) ctx = InitializeAnimationHooks(key, element);
        if (!ctx) return ret;

        // Resizing the background re-runs this; only a change in the
        // buttons themselves needs the icon list rebuilt.
        if (ctx->isInitialized) {
            auto host = ctx->iconHost.get();
            if (host && !SigDifferent(ComputeHostSignature(host, ctx->isVertical), ctx->lastSig)) {
                return ret;
            }
        }
        ctx->needsRefresh = true;
        StartRendering(*ctx, key);
    } catch (...) {
    }
    return ret;
}

void LoadSettings() {
    int rawScale = Wh_GetIntSetting(L"MaxScale");

    if (rawScale < 101) {
        rawScale = 101;
    } else if (rawScale > 220) {
        rawScale = 220;
    }

    g_settings.animationType = Wh_GetIntSetting(L"AnimationType");
    g_settings.maxScale = (double)rawScale / 100.0;

    g_settings.effectRadius = Wh_GetIntSetting(L"EffectRadius");
    if (g_settings.effectRadius <= 0) g_settings.effectRadius = 100;

    g_settings.spacingFactor = (double)Wh_GetIntSetting(L"SpacingFactor") / 100.0;
    if (g_settings.spacingFactor < 0.0) g_settings.spacingFactor = 0.5;

    g_settings.bounceDelay = Wh_GetIntSetting(L"BounceDelay");
    if (g_settings.bounceDelay < 0) g_settings.bounceDelay = 100;

    g_settings.focusDuration = (double)Wh_GetIntSetting(L"FocusDuration");
    if (g_settings.focusDuration <= 0) g_settings.focusDuration = 150.0;

    g_settings.disableVerticalBounce = (bool)Wh_GetIntSetting(L"DisableVerticalBounce");
    g_settings.taskbarLabelsMode = (bool)Wh_GetIntSetting(L"TaskbarLabelsMode");
    g_settings.excludeSystemButtonsMode = Wh_GetIntSetting(L"ExcludeSystemButtonsMode");

    g_settings.lerpSpeed = (double)Wh_GetIntSetting(L"LerpSpeed");
    if (g_settings.lerpSpeed < 0) g_settings.lerpSpeed = 0;
    if (g_settings.lerpSpeed > 60) g_settings.lerpSpeed = 60;

    g_settings.disableBounce = (bool)Wh_GetIntSetting(L"DisableBounce");

    g_settings.restIconSize = std::max(0, Wh_GetIntSetting(L"RestIconSize"));
    g_settings.iconGap = std::max(0, Wh_GetIntSetting(L"IconGap"));
    g_settings.dockPadding = std::max(0, Wh_GetIntSetting(L"DockPadding"));

    Wh_Log(L"DockAnimation: Settings loaded.");
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) module = GetModuleHandle(L"ExplorerExtensions.dll");
    return module;
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK taskbarViewHooks[] = {
        {
            {
                LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerMoved(void *))"
            },
            &TaskbarFrame_OnPointerMoved_Original,
            TaskbarFrame_OnPointerMoved_Hook,
        },
        {
            {
                LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerExited(void *))"
            },
            &TaskbarFrame_OnPointerExited_Original,
            TaskbarFrame_OnPointerExited_Hook,
        },
        {
            {
                LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::ArrangeOverride(struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size *))"
            },
            &TaskbarFrame_ArrangeOverride_Original,
            TaskbarFrame_ArrangeOverride_Hook,
        },
    };
    if (!HookSymbols(module, taskbarViewHooks, ARRAYSIZE(taskbarViewHooks))) {
        Wh_Log(L"DockAnimation: HookSymbols failed.");
        return false;
    }
    Wh_Log(L"DockAnimation: HookSymbols succeeded (Pointer events only).");
    return true;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

// Lazy hook path (if Taskbar.View.dll loads later)
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (!module) return module;

    if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module) {
        if (!g_taskbarViewDllLoaded.exchange(true)) {
            Wh_Log(L"DockAnimation: Taskbar.View.dll loaded, hooking symbols...");
            if (HookTaskbarViewDllSymbols(module)) {
                if (!g_hooksApplied.exchange(true)) {
                    Wh_ApplyHookOperations();
                    Wh_Log(L"DockAnimation: Hooks applied (slow path).");
                }
            }
        }
    }
    return module;
}

// Windhawk entry points
BOOL Wh_ModInit() {
    Wh_Log(L"DockAnimation: Wh_ModInit");
    LoadSettings();

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) return FALSE;
    } else {
        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule, "LoadLibraryExW");
        WindhawkUtils::SetFunctionHook(
            pKernelBaseLibraryExW,
            LoadLibraryExW_Hook,
            &LoadLibraryExW_Original);
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"DockAnimation: Wh_ModAfterInit");

    if (g_taskbarViewDllLoaded) {
        g_hooksApplied = true;
        Wh_Log(L"DockAnimation: Hooks already applied by Windhawk (fast path).");
    }
}

typedef void (*RunFromWindowThreadProc_t)(PVOID);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         PVOID procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        PVOID procParam;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    RUN_FROM_WINDOW_THREAD_PARAM* param =
                        (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
                    param->proc(param->procParam);
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param;
    param.proc = proc;
    param.procParam = procParam;
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);

    UnhookWindowsHookEx(hook);
    return true;
}

// Runs `action` on each context's own taskbar thread: its frame callback and
// XAML elements belong to that thread. The lock is released before sending,
// since a taskbar thread may be waiting on it inside a pointer hook.
void ForEachContextOnItsThread(std::function<void(DockAnimationContext&)> const& action) {
    std::map<HWND, std::vector<DockAnimationContext*>> windowContexts;
    {
        std::lock_guard<std::mutex> lock(g_contextsMutex);
        for (auto& pair : g_contexts) {
            auto& ctx = pair.second;
            HWND hWnd = ctx.hWnd ? ctx.hWnd : FindWindow(L"Shell_TrayWnd", NULL);
            if (hWnd) {
                windowContexts[hWnd].push_back(&ctx);
            }
        }
    }

    for (auto& pair : windowContexts) {
        std::function<void()> run = [&ctxs = pair.second, &action]() {
            for (auto* ctx : ctxs) {
                try {
                    action(*ctx);
                } catch (...) {
                }
            }
        };
        RunFromWindowThread(
            pair.first,
            [](PVOID p) {
                auto* fn = static_cast<std::function<void()>*>(p);
                (*fn)();
            },
            &run);
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"DockAnimation: Wh_ModBeforeUninit (safe cleanup)");

    ForEachContextOnItsThread([](DockAnimationContext& ctx) {
        StopRendering(ctx);
        ResetAllIconScales(ctx.icons);
        RestoreBackground(ctx);
    });

    {
        std::lock_guard<std::mutex> lock(g_contextsMutex);
        g_contexts.clear();
    }
    g_taskbarViewDllLoaded = false;
    g_hooksApplied = false;
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"DockAnimation: Settings changed.");
    LoadSettings();

    ForEachContextOnItsThread([](DockAnimationContext& ctx) {
        ResetAllIconScales(ctx.icons);
        RestoreBackground(ctx);
        ctx.icons.clear();
        ctx.isBouncing = false;
        ctx.bounceIntensity = 0.0;
        // Reapply the rest layout under the new settings right away.
        ctx.needsRefresh = true;
        StartRendering(ctx, ctx.key);
    });
}
