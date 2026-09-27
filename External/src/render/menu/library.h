#pragma once

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "../../../ext/imgui/imgui.h"
#include "../../../ext/imgui/imgui_internal.h"

#include <cstdlib>
#include <cstring>
#include <cmath>
#include <string>
#include <unordered_map>

namespace imGuiCustom
{
struct Theme
{
    ImVec4 WindowBg;
    ImVec4 CardBg;
    ImVec4 ControlBg;
    ImVec4 ControlInactive;
    ImVec4 Border;
    ImVec4 Accent;
    ImVec4 AccentText;
    ImVec4 Text;
    ImVec4 TextBright;
    ImVec4 KeybindBg;
};

struct Fonts
{
    ImFont* CascadiaMonoBL = nullptr;
};

inline Theme& GetThemeMutable()
{
    static Theme g_Theme = {
        ImVec4(0.0431f, 0.0431f, 0.0549f, 1.0f),  // WindowBg  #0B0B0E
        ImVec4(0.0627f, 0.0627f, 0.0784f, 1.0f),  // CardBg    #101014
        ImVec4(0.1020f, 0.1020f, 0.1255f, 1.0f),  // ControlBg #1A1A20
        ImVec4(0.1608f, 0.1608f, 0.1922f, 1.0f),  // ControlInactive
        ImVec4(1.0f, 1.0f, 1.0f, 0.06f),          // Border (subtle light)
        ImVec4(0.2392f, 0.8627f, 0.5176f, 1.0f),  // Accent #3DDC84 mint
        ImVec4(0.4510f, 0.9490f, 0.6510f, 1.0f),  // AccentText lighter
        ImVec4(0.5569f, 0.5569f, 0.5765f, 1.0f),  // Text muted #8E8E93
        ImVec4(0.9608f, 0.9608f, 0.9686f, 1.0f),  // TextBright #F5F5F7
        ImVec4(0.0902f, 0.0902f, 0.1098f, 1.0f),  // KeybindBg
    };
    return g_Theme;
}

inline const Theme& GetTheme() { return GetThemeMutable(); }

inline Fonts& GetFontsMutable()
{
    static Fonts g_Fonts = {};
    return g_Fonts;
}

inline const Fonts& GetFonts() { return GetFontsMutable(); }

inline void ApplyStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();
    const Theme& g_Theme = GetTheme();
    style.WindowRounding = 10.0f;
    style.ChildRounding = 8.0f;
    style.FrameRounding = 6.0f;
    style.PopupRounding = 8.0f;
    style.ScrollbarRounding = 8.0f;
    style.GrabRounding = 8.0f;
    style.TabRounding = 6.0f;
    style.WindowBorderSize = 0.0f;
    style.FrameBorderSize = 0.0f;
    style.WindowPadding = ImVec2(0.0f, 0.0f);
    style.FramePadding = ImVec2(0.0f, 0.0f);
    style.ItemSpacing = ImVec2(0.0f, 0.0f);
    style.ItemInnerSpacing = ImVec2(0.0f, 0.0f);
    style.AntiAliasedLines = true;
    style.AntiAliasedLinesUseTex = true;
    style.AntiAliasedFill = true;
    style.Colors[ImGuiCol_WindowBg] = g_Theme.WindowBg;
    style.Colors[ImGuiCol_PopupBg] = g_Theme.CardBg;
    style.Colors[ImGuiCol_Text] = g_Theme.Text;
    style.Colors[ImGuiCol_Button] = g_Theme.ControlBg;
    style.Colors[ImGuiCol_ButtonHovered] = g_Theme.ControlInactive;
    style.Colors[ImGuiCol_ButtonActive] = g_Theme.ControlInactive;
    style.Colors[ImGuiCol_Header] = g_Theme.ControlBg;
    style.Colors[ImGuiCol_HeaderHovered] = g_Theme.ControlInactive;
    style.Colors[ImGuiCol_HeaderActive] = g_Theme.Accent;
}

inline void Initialize(ImFont* cascadiaMonoBL)
{
    GetFontsMutable().CascadiaMonoBL = cascadiaMonoBL;
    ApplyStyle();
}

inline float AnimateFloat(ImGuiID id, bool enabled, float speed = 12.0f)
{
    (void)id;
    (void)speed;
    return enabled ? 1.0f : 0.0f;
}

inline float EaseOutCubic(float t) { float u = 1.0f - t; return 1.0f - u * u * u; }

inline ImVec4 LerpColor(const ImVec4& a, const ImVec4& b, float t)
{
    return ImVec4(ImLerp(a.x, b.x, t), ImLerp(a.y, b.y, t), ImLerp(a.z, b.z, t), ImLerp(a.w, b.w, t));
}

// soft modern borders — names kept for compat with all call sites
inline ImU32 OutlineBlack() { return ImGui::GetColorU32(IM_COL32(0, 0, 0, 170)); }
inline ImU32 OutlineInner() { return ImGui::GetColorU32(IM_COL32(255, 255, 255, 16)); }
inline ImU32 BorderSubtle(float alpha_mul = 1.0f) { return ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 0.07f * alpha_mul)); }

inline ImVec2 g_contentOffset = ImVec2(0.0f, 0.0f);
inline float g_fontScale = 1.0f;
inline float SliderTop() { return 12.0f * g_fontScale + 5.0f; }
inline float SliderStep() { return 12.0f * g_fontScale + 5.0f + 15.0f; }
inline float ComboTop() { return 12.0f * g_fontScale + 3.0f; }
inline float ComboStep() { return 22.0f; }
inline float CheckStep() { return 13.5f * g_fontScale + 5.0f; }

inline ImGuiID& ComboOpenId() { static ImGuiID v = 0; return v; }
inline int& ComboClosedFrame() { static int f = -100000; return f; }
inline bool PopupBlocking() {
    if (ImGui::GetFrameCount() == ComboClosedFrame())
        return true;
    return ComboOpenId() != 0;
}

inline ImU32 ColorU32(const ImVec4& color, float alpha_mul = 1.0f)
{
    ImVec4 c = color;
    c.w *= alpha_mul;
    return ImGui::GetColorU32(c);
}

inline void SectionHeader(const char* label, const ImVec2& pos, float width = 272.0f)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    const ImVec2 base = window->Pos;
    const ImVec2 min = ImVec2(std::floor(base.x + pos.x + g_contentOffset.x), std::floor(base.y + pos.y + g_contentOffset.y));
    const Fonts& fonts = GetFonts();
    ImFont* font = fonts.CascadiaMonoBL ? fonts.CascadiaMonoBL : ImGui::GetFont();
    const float fs = 10.5f * g_fontScale;
    const Theme& theme = GetTheme();
    ImDrawList* draw = ImGui::GetWindowDrawList();
    draw->AddText(font, fs, min + ImVec2(2.0f, 0.0f), ColorU32(theme.Text, 0.95f), label);
    (void)width;
}

inline void CardRect(ImDrawList* draw, const ImVec2& mn, const ImVec2& mx, float rounding = 10.0f, float alpha = 1.0f)
{
    const Theme& th = GetTheme();
    draw->AddRectFilled(mn, mx, ColorU32(th.CardBg, alpha), rounding);
    draw->AddRect(mn, mx, ColorU32(ImVec4(1.0f, 1.0f, 1.0f, 0.07f), alpha), rounding, 0, 1.0f);
}

inline void ShadowRect(ImDrawList* draw, const ImVec2& mn, const ImVec2& mx, float rounding = 10.0f)
{
    // cheap layered shadow (3 passes fading out)
    for (int i = 3; i >= 1; --i) {
        float o = (float)i;
        ImU32 c = ImGui::GetColorU32(IM_COL32(0, 0, 0, 18 - i * 3));
        draw->AddRectFilled(mn - ImVec2(o * 2.0f, o * 2.0f), mx + ImVec2(o * 2.0f, o * 2.0f + 3.0f), c, rounding + o * 2.0f);
    }
}

inline void AddTextWithOutline(ImDrawList* draw_list, ImFont* font, float font_size, const ImVec2& pos, ImU32 text_col, const char* text)
{
    const ImU32 shadow = ImGui::GetColorU32(IM_COL32(0, 0, 0, 140));
    if (font)
    {
        draw_list->AddText(font, font_size, pos + ImVec2(0.0f, 1.0f), shadow, text);
        draw_list->AddText(font, font_size, pos, text_col, text);
    }
    else
    {
        draw_list->AddText(pos + ImVec2(0.0f, 1.0f), shadow, text);
        draw_list->AddText(pos, text_col, text);
    }
}

inline bool Checkbox(const char* label, bool* value, const ImVec2& pos)
{
    const char* display = label;
    const char* hash = strstr(label, "##");
    std::string displayStr;
    if (hash) displayStr.assign(label, hash - label), display = displayStr.c_str();
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    const ImVec2 base = window->Pos;
    const ImVec2 min = ImVec2(std::floor(base.x + pos.x + g_contentOffset.x), std::floor(base.y + pos.y + g_contentOffset.y));
    const Fonts& fonts = GetFonts();
    ImFont* font = fonts.CascadiaMonoBL ? fonts.CascadiaMonoBL : ImGui::GetFont();
    const float fs = 12.5f * g_fontScale;
    const ImVec2 text_size = font->CalcTextSizeA(fs, FLT_MAX, 0.0f, display);
    ImGui::SetCursorScreenPos(min);
    ImGui::PushID(label);
    const bool pressed = ImGui::InvisibleButton("##check", ImVec2(24.0f + text_size.x, 18.0f));
    const bool hovered = ImGui::IsItemHovered();
    if (pressed && !PopupBlocking())
        *value = !*value;

    const ImGuiID id = ImGui::GetItemID();
    const float check = AnimateFloat(id, *value, 13.0f);
    const float hov = AnimateFloat(id + 1, hovered, 14.0f);
    const Theme& theme = GetTheme();
    ImDrawList* draw = ImGui::GetWindowDrawList();

    const ImVec2 boxMin = min + ImVec2(0.0f, 2.0f);
    const ImVec2 boxMax = boxMin + ImVec2(14.0f, 14.0f);

    ImVec4 boxCol = LerpColor(LerpColor(theme.ControlBg, theme.ControlInactive, hov * 0.5f), theme.Accent, check);
    draw->AddRectFilled(boxMin, boxMax, ColorU32(boxCol), 4.0f);
    draw->AddRect(boxMin, boxMax, check > 0.03f ? ColorU32(theme.Accent, 0.9f) : (hovered ? ColorU32(ImVec4(1, 1, 1, 0.16f)) : OutlineBlack()), 4.0f, 0, 1.0f);

    // check glyph fades in with the animation
    if (check > 0.03f) {
        const ImU32 tick = IM_COL32(6, 12, 9, (int)(255.0f * check));
        const ImVec2 t0 = boxMin + ImVec2(3.5f, 7.2f);
        const ImVec2 t1 = boxMin + ImVec2(6.4f, 10.0f);
        const ImVec2 t2 = boxMin + ImVec2(10.8f, 4.4f);
        draw->AddLine(t0, t1, tick, 1.8f);
        draw->AddLine(t1, t2, tick, 1.8f);
    }

    const ImVec4 txtCol = LerpColor(theme.Text, theme.TextBright, check * 0.55f + hov * 0.2f);
    draw->AddText(font, fs, ImVec2(min.x + 22.0f, min.y + (18.0f - text_size.y) * 0.5f), ColorU32(txtCol), display);
    ImGui::PopID();
    return pressed;
}

inline bool ButtonCore(const char* label, const ImVec2& min, const ImVec2& size, bool active = false)
{
    const char* display = label;
    const char* hash = strstr(label, "##");
    std::string displayStr;
    if (hash) displayStr.assign(label, hash - label), display = displayStr.c_str();

    ImGui::PushID(label);
    ImGui::SetCursorScreenPos(min);
    const bool pressed = ImGui::InvisibleButton("##btn", size);
    const bool hovered = ImGui::IsItemHovered();
    const bool held = ImGui::IsItemActive();
    const ImGuiID id = ImGui::GetItemID();

    const float hover_anim = AnimateFloat(id, hovered || held, 15.0f);
    const float active_anim = AnimateFloat(id + 1, active, 15.0f);
    const float press_anim = AnimateFloat(id + 2, held, 20.0f);

    const Theme& theme = GetTheme();
    ImDrawList* draw = ImGui::GetWindowDrawList();
    const float rounding = 5.0f;

    ImVec4 bg = LerpColor(theme.ControlBg, theme.ControlInactive, hover_anim * 0.55f);
    if (active_anim > 0.01f)
        bg = LerpColor(bg, theme.Accent, 0.18f * active_anim);
    if (press_anim > 0.01f)
        bg = LerpColor(bg, theme.Accent, 0.10f * press_anim);

    if ((hover_anim > 0.02f || active) && !held)
        draw->AddRectFilled(min - ImVec2(2.0f, 2.0f), min + size + ImVec2(2.0f, 2.0f), ColorU32(theme.Accent, (0.10f * hover_anim + 0.12f * active_anim)), rounding + 2.0f);

    draw->AddRectFilled(min, min + size, ColorU32(bg), rounding);
    ImU32 border = (active_anim > 0.02f) ? ColorU32(LerpColor(ImVec4(1,1,1,0.07f), theme.Accent, active_anim)) : (hovered ? ColorU32(ImVec4(1,1,1,0.13f)) : OutlineBlack());
    draw->AddRect(min, min + size, border, rounding, 0, 1.0f);
    draw->AddRect(min + ImVec2(1.0f, 1.0f), min + size - ImVec2(1.0f, 1.0f), OutlineInner(), rounding - 1.0f, 0, 1.0f);

    const Fonts& fonts = GetFonts();
    ImFont* font = fonts.CascadiaMonoBL ? fonts.CascadiaMonoBL : ImGui::GetFont();
    const float font_size = 12.0f * g_fontScale;
    const ImVec2 text_sz = font->CalcTextSizeA(font_size, FLT_MAX, 0.0f, display);
    const ImVec2 text_pos(
        std::floor(min.x + (size.x - text_sz.x) * 0.5f),
        std::floor(min.y + (size.y - text_sz.y) * 0.5f)
    );

    const ImVec4 base_text = active ? theme.AccentText : theme.Text;
    const ImVec4 text_col = LerpColor(base_text, theme.TextBright, hover_anim * 0.45f + active_anim * 0.3f);
    draw->AddText(font, font_size, text_pos, ColorU32(text_col), display);

    ImGui::PopID();
    return pressed && !PopupBlocking();
}

inline bool Button(const char* label, const ImVec2& size = ImVec2(0.0f, 22.0f), bool active = false)
{
    ImVec2 actual_size = size;
    if (actual_size.x <= 0.0f)
        actual_size.x = ImGui::GetContentRegionAvail().x;
    if (actual_size.y <= 0.0f)
        actual_size.y = 22.0f;

    const ImVec2 min = ImVec2(std::floor(ImGui::GetCursorScreenPos().x), std::floor(ImGui::GetCursorScreenPos().y));
    return ButtonCore(label, min, actual_size, active);
}

inline bool ButtonPos(const char* label, const ImVec2& pos, const ImVec2& size = ImVec2(120.0f, 22.0f), bool active = false)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    const ImVec2 base = window->Pos;
    const ImVec2 min = ImVec2(std::floor(base.x + pos.x + g_contentOffset.x), std::floor(base.y + pos.y + g_contentOffset.y));
    return ButtonCore(label, min, size, active);
}

inline bool ParseHexColor(const char* text, ImVec4& out) {
    if (!text)
        return false;
    while (*text == ' ' || *text == '\t')
        ++text;
    if (*text == '#')
        ++text;
    size_t len = 0;
    while (text[len] != '\0' && len < 9)
        ++len;
    if ((len != 6 && len != 8) || text[len] != '\0')
        return false;
    auto hexVal = [](char c) -> int {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        return -1;
    };
    unsigned int v[8];
    for (size_t i = 0; i < len; ++i) {
        int h = hexVal(text[i]);
        if (h < 0)
            return false;
        v[i] = (unsigned int)h;
    }
    out.x = (float)(v[0] * 16 + v[1]) / 255.0f;
    out.y = (float)(v[2] * 16 + v[3]) / 255.0f;
    out.z = (float)(v[4] * 16 + v[5]) / 255.0f;
    out.w = (len == 8) ? (float)(v[6] * 16 + v[7]) / 255.0f : out.w;
    return true;
}

inline bool SliderFloat(const char* label, float* value, float min_value, float max_value, const ImVec2& pos, float width, const char* text_label, const char* format);

inline bool ColorSquare(const char* id_text, ImVec4* color, const ImVec2& pos)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();

    const ImVec2 base = window->Pos;
    const ImVec2 min = ImVec2(std::floor(base.x + pos.x + g_contentOffset.x), std::floor(base.y + pos.y + g_contentOffset.y));
    const ImVec2 size(22.0f, 16.0f);

    ImGui::SetCursorScreenPos(min);

    const bool pressed = ImGui::InvisibleButton(id_text, size);
    const bool hovered = ImGui::IsItemHovered();
    const ImGuiID hid = ImGui::GetItemID();
    const float hov = AnimateFloat(hid, hovered, 16.0f);
    static bool colorDragging = false;
    static ImVec2 colorGrabOff{};

    ImDrawList* draw = ImGui::GetWindowDrawList();

    if (hov > 0.02f)
        draw->AddRectFilled(min - ImVec2(2.0f, 2.0f), min + size + ImVec2(2.0f, 2.0f), ColorU32(GetTheme().Accent, 0.18f * hov), 6.0f);
    draw->AddRectFilled(min, min + size, ColorU32(*color), 4.0f);
    // glossy top half
    draw->AddRectFilled(min, ImVec2(min.x + size.x, min.y + size.y * 0.45f), ImGui::GetColorU32(IM_COL32(255, 255, 255, 28)), 4.0f);
    // re-cover bottom rounding bleed with solid lower half (keeps pill shape clean)
    draw->AddRectFilled(ImVec2(min.x, min.y + size.y * 0.45f), min + size, ColorU32(ImVec4(color->x, color->y, color->z, color->w * 0.35f)), 0.0f);
    draw->AddRectFilled(min, min + size, ColorU32(*color, 0.0f), 4.0f); // keep rounding clip crisp
    draw->AddRect(min, min + size, hovered ? ColorU32(GetTheme().Accent, 0.7f) : OutlineBlack(), 4.0f, 0, 1.0f);
    draw->AddRect(min + ImVec2(1.0f, 1.0f), min + size - ImVec2(1.0f, 1.0f), OutlineInner(), 3.0f, 0, 1.0f);
    if (pressed)
        ImGui::OpenPopup(id_text);

    if (ImGui::BeginPopup(id_text))
    {
        ImVec2 popPos = ImGui::GetWindowPos();
        ImVec2 popSize = ImGui::GetWindowSize();
        ImGui::SetCursorScreenPos(popPos);
        ImGui::PushID("color_drag");
        ImGui::InvisibleButton("##color_drag", ImVec2(popSize.x, 10.0f));
        ImGuiID dragId = ImGui::GetItemID();
        float dragHov = AnimateFloat(dragId, ImGui::IsItemHovered(), 18.0f);
        ImDrawList* dragDraw = ImGui::GetWindowDrawList();
        for (int i = 0; i < 3; ++i) {
            float dx = popPos.x + popSize.x * 0.5f + (float)(i - 1) * 8.0f;
            dragDraw->AddCircleFilled(ImVec2(dx, popPos.y + 5.0f), 1.4f, ColorU32(LerpColor(GetTheme().Text, GetTheme().TextBright, dragHov)), 8);
        }
        if (ImGui::IsItemActivated()) {
            colorGrabOff = ImGui::GetIO().MousePos - popPos;
            colorDragging = true;
        }
        if (colorDragging) {
            if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
                ImGui::SetWindowPos(ImGui::GetIO().MousePos - colorGrabOff, ImGuiCond_Always);
            else
                colorDragging = false;
        }
        ImGui::PopID();
        ImGui::PushStyleColor(ImGuiCol_FrameBg, GetTheme().CardBg);
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, GetTheme().ControlInactive);
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, GetTheme().ControlInactive);
        ImGui::PushStyleColor(ImGuiCol_SliderGrab, GetTheme().Accent);
        ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, GetTheme().Accent);
        ImGui::PushStyleColor(ImGuiCol_Button, GetTheme().ControlBg);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, GetTheme().ControlInactive);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, GetTheme().ControlInactive);
        ImGui::PushStyleColor(ImGuiCol_Header, GetTheme().CardBg);
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, GetTheme().ControlInactive);
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, GetTheme().ControlInactive);
        ImGui::ColorPicker4("##picker", (float*)color, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_PickerHueBar);
        ImGui::PopStyleColor(11);
        const Theme& ptheme = GetTheme();
        ImFont* pfont = GetFonts().CascadiaMonoBL ? GetFonts().CascadiaMonoBL : ImGui::GetFont();
        ImGuiWindow* pwin = ImGui::GetCurrentWindow();
        ImVec2 prel = ImGui::GetCursorScreenPos() - pwin->Pos + ImVec2(0.0f, 16.0f);
        SliderFloat("hex_opacity", &color->w, 0.0f, 1.0f, prel, 180.0f, "Opacity", "%.2f");
        if (color->w < 0.0f) color->w = 0.0f;
        if (color->w > 1.0f) color->w = 1.0f;
        char hex[16];
        ImFormatString(hex, IM_ARRAYSIZE(hex), "#%02X%02X%02X%02X",
            (int)(ImClamp(color->x, 0.0f, 1.0f) * 255.0f),
            (int)(ImClamp(color->y, 0.0f, 1.0f) * 255.0f),
            (int)(ImClamp(color->z, 0.0f, 1.0f) * 255.0f),
            (int)(ImClamp(color->w, 0.0f, 1.0f) * 255.0f));
        ImVec2 hexRel = prel + ImVec2(0.0f, 15.0f);
        ImVec2 hexMin = pwin->Pos + hexRel;
        ImVec2 hexTs = pfont->CalcTextSizeA(12.0f * g_fontScale, FLT_MAX, 0.0f, hex);
        ImGui::SetCursorScreenPos(hexMin);
        ImGui::PushID("hex_ctx_btn");
        ImGui::InvisibleButton("##hex", ImVec2(hexTs.x + 6.0f, 15.0f));
        ImGuiID hexId = ImGui::GetItemID();
        float hexHov = AnimateFloat(hexId, ImGui::IsItemHovered(), 18.0f);
        ImGui::GetWindowDrawList()->AddText(pfont, 12.0f * g_fontScale, hexMin, ColorU32(LerpColor(ptheme.Text, ptheme.TextBright, hexHov)), hex);
        static ImGuiID hexCtxOpen = 0;
        if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
            hexCtxOpen = (hexCtxOpen == hexId) ? 0 : hexId;
        bool hexCtx = (hexCtxOpen == hexId);
        float hexCtxAnim = AnimateFloat(hexId + 40, hexCtx, 18.0f);
        const float hexRowH = 18.0f;
        const float hexPad = 4.0f;
        const float hexPopW = 116.0f;
        const float hexFullH = hexPad * 2.0f + hexRowH * 2.0f;
        ImVec2 hexPopMin(hexMin.x, hexMin.y + 16.0f);
        if (hexCtx && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImRect(hexPopMin, ImVec2(hexPopMin.x + hexPopW, hexPopMin.y + hexFullH)).Contains(ImGui::GetIO().MousePos))
            hexCtxOpen = 0;
        if (hexCtxAnim > 0.01f) {
            ImDrawList* hexFg = ImGui::GetForegroundDrawList();
            hexFg->PushClipRect(hexPopMin, ImVec2(hexPopMin.x + hexPopW, hexPopMin.y + hexFullH * hexCtxAnim), true);
            ImVec2 hexBoxMax = ImVec2(hexPopMin.x + hexPopW, hexPopMin.y + hexFullH);
            hexFg->AddRectFilled(hexPopMin, hexBoxMax, ColorU32(ptheme.ControlBg, hexCtxAnim), 6.0f);
            hexFg->AddRect(hexPopMin, hexBoxMax, ImGui::GetColorU32(IM_COL32(0, 0, 0, (int)(170 * hexCtxAnim))), 6.0f, 0, 1.0f);
            const char* hexOpts[2] = {"Copy Hex", "Paste Hex"};
            for (int i = 0; i < 2; ++i) {
                ImVec2 iMin(hexPopMin.x + 3.0f, hexPopMin.y + hexPad + hexRowH * i);
                ImVec2 iMax(hexPopMin.x + hexPopW - 3.0f, iMin.y + hexRowH);
                bool hov2 = ImRect(iMin, iMax).Contains(ImGui::GetIO().MousePos);
                ImGui::PushID(100 + i);
                ImGuiID iid = ImGui::GetID("hex_opt");
                float ih = AnimateFloat(iid, hov2, 18.0f);
                ImVec4 parsed{};
                bool valid = (i == 1) ? ParseHexColor(ImGui::GetClipboardText(), parsed) : true;
                if (ih > 0.01f)
                    hexFg->AddRectFilled(iMin, iMax, ColorU32(LerpColor(ptheme.ControlBg, ptheme.ControlInactive, ih * 0.8f), hexCtxAnim), 4.0f);
                hexFg->AddText(pfont, 12.0f * g_fontScale, ImVec2(iMin.x + 6.0f, iMin.y + 3.0f), ColorU32(valid ? LerpColor(ptheme.Text, ptheme.TextBright, ih * 0.35f) : ImVec4(0.45f, 0.45f, 0.45f, 1.0f), hexCtxAnim), hexOpts[i]);
                if (hexCtx && hexCtxAnim > 0.70f && hov2 && valid && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                    if (i == 0)
                        ImGui::SetClipboardText(hex);
                    else
                        *color = parsed;
                    hexCtxOpen = 0;
                }
                ImGui::PopID();
            }
            hexFg->PopClipRect();
        }
        ImGui::PopID();
        popPos = ImGui::GetWindowPos();
        popSize = ImGui::GetWindowSize();
        ImGui::EndPopup();
        ImDrawList* popFg = ImGui::GetForegroundDrawList();
        popFg->AddRect(popPos, popPos + popSize, OutlineBlack(), 8.0f, 0, 1.0f);
        popFg->AddRect(popPos + ImVec2(1.0f, 1.0f), popPos + popSize - ImVec2(1.0f, 1.0f), OutlineInner(), 7.0f, 0, 1.0f);
    } else {
        colorDragging = false;
    }

    return pressed;
}

inline const char* KeyName(int key)
{
    if (key == 0)
        return "None";
    switch (key)
    {
    case 0x01: return "LMB";
    case 0x02: return "RMB";
    case 0x04: return "MMB";
    case 0x05: return "M4";
    case 0x06: return "M5";
    case 0x08: return "Back";
    case 0x09: return "Tab";
    case 0x0D: return "Enter";
    case 0x10: return "Shift";
    case 0x11: return "Ctrl";
    case 0x12: return "Alt";
    case 0x14: return "Caps";
    case 0x1B: return "Esc";
    case 0x20: return "Space";
    case 0x25: return "Left";
    case 0x26: return "Up";
    case 0x27: return "Right";
    case 0x28: return "Down";
    case 0x2D: return "Insert";
    case 0x2E: return "Delete";
    default: break;
    }
    if (key >= 0x30 && key <= 0x39) { static char n[2]; n[0] = (char)key; n[1] = '\0'; return n; }
    if (key >= 0x70 && key <= 0x87) { static char f[4]; ImFormatString(f, IM_ARRAYSIZE(f), "F%d", key - 0x6F); return f; }
    if (key >= ImGuiKey_NamedKey_BEGIN && key < ImGuiKey_NamedKey_END)
        return ImGui::GetKeyName((ImGuiKey)key);

    static char name[16];

    if (key >= 'A' && key <= 'Z')
        ImFormatString(name, IM_ARRAYSIZE(name), "%c", key);
    else
        ImFormatString(name, IM_ARRAYSIZE(name), "Key %d", key);

    return name;
}

inline bool Keybind(const char* label, int* key, const ImVec2& pos, const ImVec2& size = ImVec2(68.0f, 13.0f), int* mode = nullptr)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    const ImVec2 base = window->Pos;
    const ImVec2 min = ImVec2(std::floor(base.x + pos.x + g_contentOffset.x), std::floor(base.y + pos.y + g_contentOffset.y));
    ImGui::SetCursorScreenPos(min);
    const bool pressed = ImGui::InvisibleButton(label, size);
    const ImGuiID id = ImGui::GetItemID();
    const bool right_clicked = ImGui::IsItemClicked(ImGuiMouseButton_Right);

    static std::unordered_map<ImGuiID, int> s_keybind_modes;
    int& current_mode = mode ? *mode : s_keybind_modes[id];

    static ImGuiID context_open_id = 0;
    if (right_clicked)
    {
        context_open_id = (context_open_id == id) ? 0 : id;
    }

    static ImGuiID waiting_id = 0;
    static bool wait_mouse_release = false;
    if (pressed && !PopupBlocking())
    {
        context_open_id = 0;
        waiting_id = id;
        wait_mouse_release = true;
        ImGui::SetActiveID(id, window);
    }

    const bool active = waiting_id == id;
    if (active)
    {
        ImGuiIO& io = ImGui::GetIO();
        bool any_mouse_down = false;
        for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); ++i)
            any_mouse_down |= io.MouseDown[i];
        if (!any_mouse_down)
            wait_mouse_release = false;

        if (ImGui::IsKeyPressed(ImGuiKey_Escape))
        {
            *key = 0;
            waiting_id = 0;
            ImGui::ClearActiveID();
        }

        for (int key_code = ImGuiKey_NamedKey_BEGIN; key_code < ImGuiKey_NamedKey_END; ++key_code)
        {
            ImGuiKey imgui_key = (ImGuiKey)key_code;
            if (ImGui::IsKeyPressed(imgui_key) && imgui_key != ImGuiKey_Escape)
            {
                *key = key_code;
                waiting_id = 0;
                ImGui::ClearActiveID();
                break;
            }
        }

        if (waiting_id == id && !wait_mouse_release)
        {
            for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); ++i)
            {
                if (ImGui::IsMouseClicked(i))
                {
                    *key = i == 0 ? 0x01 : i == 1 ? 0x02 : 0x04;
                    waiting_id = 0;
                    ImGui::ClearActiveID();
                    break;
                }
            }
        }
    }

    const float hovA = AnimateFloat(id, active || ImGui::IsItemHovered(), 16.0f);
    const float pulse = active ? 1.0f : 0.0f;
    const Theme& theme = GetTheme();
    ImDrawList* draw = ImGui::GetWindowDrawList();
    ImVec4 kbBg = LerpColor(theme.KeybindBg, theme.ControlInactive, hovA * 0.4f);
    if (active) kbBg = LerpColor(kbBg, theme.Accent, 0.25f * (0.5f + 0.5f * pulse));
    draw->AddRectFilled(min, min + size, ColorU32(kbBg), 6.0f);
    draw->AddRect(min, min + size, active ? ColorU32(theme.Accent, 0.65f + 0.3f * pulse) : (hovA > 0.1f ? ColorU32(ImVec4(1,1,1,0.14f)) : OutlineBlack()), 6.0f, 0, 1.0f);
    draw->AddRect(min + ImVec2(1.0f, 1.0f), min + size - ImVec2(1.0f, 1.0f), OutlineInner(), 5.0f, 0, 1.0f);

    const Fonts& fonts = GetFonts();
    ImFont* font = fonts.CascadiaMonoBL ? fonts.CascadiaMonoBL : ImGui::GetFont();
    const float font_size = 11.5f * g_fontScale;
    const char* text = active ? "..." : (current_mode == 2 && *key == 0 ? "Always" : KeyName(*key));
    const ImVec2 text_size = font->CalcTextSizeA(font_size, FLT_MAX, 0.0f, text);
    const ImVec4 txtC = active ? theme.AccentText : ImVec4(0.839f, 0.839f, 0.86f, 1.0f);
    draw->AddText(font, font_size, ImVec2(min.x + (size.x - text_size.x) * 0.5f, min.y + (size.y - text_size.y) * 0.5f), ColorU32(txtC), text);

    const bool context_open = (context_open_id == id);
    const float context_anim = AnimateFloat(id + 20, context_open, 18.0f);
    const float popup_w = size.x + 14.0f;
    const float row_height = 18.0f;
    const float popup_padding = 4.0f;
    const float full_height = popup_padding * 2.0f + row_height * 3.0f;
    const float visible_height = full_height * context_anim;

    const ImVec2 popup_min(min.x + size.x - popup_w, min.y + size.y + 3.0f);
    const ImVec2 popup_max(popup_min.x + popup_w, popup_min.y + visible_height);
    const ImRect total_rect(min, ImVec2(popup_min.x + popup_w, popup_min.y + full_height));

    if (context_open && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !total_rect.Contains(ImGui::GetIO().MousePos))
    {
        context_open_id = 0;
    }

    if (context_anim > 0.01f)
    {
        ImDrawList* overlay = ImGui::GetForegroundDrawList();
        overlay->PushClipRect(popup_min, popup_max, true);
        const ImVec2 popup_box_max = ImVec2(popup_min.x + popup_w, popup_min.y + full_height);
        // shadow
        overlay->AddRectFilled(popup_min + ImVec2(0, 3), popup_box_max + ImVec2(0, 3), ImGui::GetColorU32(IM_COL32(0, 0, 0, (int)(60 * context_anim))), 7.0f);
        overlay->AddRectFilled(popup_min, popup_box_max, ColorU32(theme.ControlBg, context_anim), 7.0f);
        overlay->AddRect(popup_min, popup_box_max, ImGui::GetColorU32(IM_COL32(0, 0, 0, (int)(170 * context_anim))), 7.0f, 0, 1.0f);

        struct ModeDef { const char* label; int value; };
        static const ModeDef mode_list[3] = {
            { "Toggle", 1 },
            { "Hold",   0 },
            { "Always", 2 }
        };

        for (int i = 0; i < 3; ++i)
        {
            const ImVec2 item_min(popup_min.x + 3.0f, popup_min.y + popup_padding + row_height * i);
            const ImVec2 item_max(popup_min.x + popup_w - 3.0f, item_min.y + row_height);
            const ImRect item_rect(item_min, item_max);
            const bool item_hovered = item_rect.Contains(ImGui::GetIO().MousePos);
            const bool item_pressed = context_open && context_anim > 0.70f && item_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left);

            ImGui::PushID(i);
            const ImGuiID item_id = ImGui::GetID("kb_mode");
            const float item_hover = AnimateFloat(item_id, item_hovered, 18.0f);
            const bool is_current = (current_mode == mode_list[i].value);
            const float item_selected = AnimateFloat(item_id + 1, is_current, 18.0f);

            const ImVec4 row_color = LerpColor(theme.ControlBg, theme.ControlInactive, item_hover * 0.8f + item_selected * 0.35f);
            if (item_hover > 0.01f || item_selected > 0.01f)
                overlay->AddRectFilled(item_min, item_max, ColorU32(row_color, context_anim), 5.0f);
            if (is_current)
                overlay->AddRectFilled(ImVec2(item_min.x, item_min.y + 3.0f), ImVec2(item_min.x + 2.0f, item_max.y - 3.0f), ColorU32(theme.Accent, context_anim), 1.0f);

            const ImVec4 base_text_color = is_current ? theme.AccentText : theme.Text;
            const ImVec4 text_color = LerpColor(base_text_color, theme.TextBright, item_hover * 0.35f);
            overlay->AddText(font, font_size, ImVec2(item_min.x + 8.0f, item_min.y + (row_height - font_size) * 0.5f), ColorU32(text_color, context_anim), mode_list[i].label);

            if (item_pressed)
            {
                current_mode = mode_list[i].value;
                if (mode) *mode = mode_list[i].value;
                context_open_id = 0;
                ComboClosedFrame() = ImGui::GetFrameCount();
            }
            ImGui::PopID();
        }

        overlay->PopClipRect();
    }
    if (context_open_id == id)
        ComboOpenId() = id;
    else if (ComboOpenId() == id)
        ComboOpenId() = 0;
    return pressed;
}

inline bool SliderFloat(const char* label, float* value, float min_value, float max_value, const ImVec2& pos, float width, const char* text_label, const char* format = "%.0f")
{
    const float font_size = 12.0f * g_fontScale;
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    const ImVec2 base = window->Pos;
    const ImVec2 origin = ImVec2(std::floor(base.x + pos.x + g_contentOffset.x), std::floor(base.y + pos.y + g_contentOffset.y));
    const float height = 7.0f;

    const float step_w = 16.0f;
    const float step_gap = 5.0f;
    const ImVec2 minus_min(origin.x, origin.y);
    const ImVec2 minus_max(origin.x + step_w, origin.y + height + 4.0f);
    const ImVec2 track_min(origin.x + step_w + step_gap, origin.y + 2.0f);
    const float track_w = width - (step_w + step_gap) * 2.0f;
    const ImVec2 track_max(track_min.x + track_w, origin.y + height + 2.0f);
    const ImVec2 plus_min(track_max.x + step_gap, origin.y);
    const ImVec2 plus_max(plus_min.x + step_w, origin.y + height + 4.0f);

    ImGui::PushID(label);
    ImGui::SetCursorScreenPos(origin);
    const bool pressed = ImGui::InvisibleButton("##slider_total", ImVec2(width, height + 6.0f));
    const ImGuiID id = ImGui::GetItemID();

    const ImVec2 mouse = ImGui::GetIO().MousePos;
    const bool minus_hovered = (mouse.x >= minus_min.x - 2.0f && mouse.x <= minus_max.x + 2.0f &&
                                mouse.y >= origin.y - 4.0f && mouse.y <= origin.y + height + 8.0f);
    const bool plus_hovered = (mouse.x >= plus_min.x - 2.0f && mouse.x <= plus_max.x + 2.0f &&
                               mouse.y >= origin.y - 4.0f && mouse.y <= origin.y + height + 8.0f);
    const bool track_hovered = (mouse.x >= track_min.x - 4.0f && mouse.x <= track_max.x + 4.0f &&
                                mouse.y >= origin.y - 4.0f && mouse.y <= origin.y + height + 8.0f);

    const bool active = ImGui::IsItemActive();
    bool changed = false;

    const float step = (max_value - min_value <= 5.0f) ? 0.1f : 1.0f;
    if (!PopupBlocking() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        if (minus_hovered)
        {
            *value = ImClamp(*value - step, min_value, max_value);
            changed = true;
        }
        else if (plus_hovered)
        {
            *value = ImClamp(*value + step, min_value, max_value);
            changed = true;
        }
    }
    const float down_dur = ImGui::GetIO().MouseDownDuration[0];
    if (down_dur > 0.35f && ImGui::IsMouseDown(ImGuiMouseButton_Left))
    {
        static float last_time = 0.0f;
        const float cur_time = (float)ImGui::GetTime();
        if (cur_time - last_time > 0.08f)
        {
            if (minus_hovered)
            {
                *value = ImClamp(*value - step, min_value, max_value);
                changed = true;
                last_time = cur_time;
            }
            else if (plus_hovered)
            {
                *value = ImClamp(*value + step, min_value, max_value);
                changed = true;
                last_time = cur_time;
            }
        }
    }

    if (active && !minus_hovered && !plus_hovered && !PopupBlocking())
    {
        float t = (mouse.x - track_min.x) / track_w;
        t = ImClamp(t, 0.0f, 1.0f);
        const float new_value = min_value + (max_value - min_value) * t;
        if (*value != new_value)
        {
            *value = new_value;
            changed = true;
        }
    }

    const float target_t = ImClamp((*value - min_value) / (max_value - min_value), 0.0f, 1.0f);
    static std::unordered_map<ImGuiID, float> slider_values;
    float& animated_t = slider_values[id];
    animated_t = target_t;
    const float hover = AnimateFloat(id + 1, track_hovered || active, 16.0f);
    const float minus_anim = AnimateFloat(id + 2, minus_hovered, 16.0f);
    const float plus_anim = AnimateFloat(id + 3, plus_hovered, 16.0f);

    const Theme& theme = GetTheme();
    ImDrawList* draw = ImGui::GetWindowDrawList();
    const Fonts& fonts = GetFonts();
    ImFont* font = fonts.CascadiaMonoBL ? fonts.CascadiaMonoBL : ImGui::GetFont();

    // label row
    char value_text[64];
    char prefix_text[128];
    ImFormatString(value_text, IM_ARRAYSIZE(value_text), format, *value);
    ImFormatString(prefix_text, IM_ARRAYSIZE(prefix_text), "%s", text_label);
    const ImVec2 label_pos(origin.x + 1.0f, origin.y - font_size - 5.0f);
    const ImVec2 prefix_sz = font->CalcTextSizeA(font_size, FLT_MAX, 0.0f, prefix_text);
    const ImVec2 value_sz = font->CalcTextSizeA(font_size, FLT_MAX, 0.0f, value_text);
    draw->AddText(font, font_size, label_pos, ColorU32(theme.Text), prefix_text);

    // value pill on the right of label row
    static ImGuiID s_editId = 0;
    static char s_editBuf[64] = {};
    static bool s_editFocus = false;
    const ImVec2 box_pad(6.0f, 2.0f);
    const float box_w = (std::max)(value_sz.x + box_pad.x * 2.0f, 54.0f);
    const float box_h = font_size + box_pad.y * 2.0f;
    const ImVec2 box_min(origin.x + width - box_w, label_pos.y - box_pad.y);
    const ImVec2 box_max(box_min.x + box_w, box_min.y + box_h);
    if (s_editId != id)
    {
        draw->AddRectFilled(box_min, box_max, ColorU32(theme.ControlBg), 5.0f);
        draw->AddRect(box_min, box_max, OutlineBlack(), 5.0f, 0, 1.0f);
        draw->AddRect(box_min + ImVec2(1.0f, 1.0f), box_max - ImVec2(1.0f, 1.0f), OutlineInner(), 4.0f, 0, 1.0f);
        draw->AddText(font, font_size, ImVec2(box_min.x + box_pad.x, box_min.y + box_pad.y), ColorU32(theme.TextBright), value_text);
        ImGui::SetCursorScreenPos(box_min);
        ImGui::InvisibleButton("##slider_edit", box_max - box_min);
        if (!PopupBlocking() && ImGui::IsItemHovered() &&
            (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) || ImGui::IsMouseClicked(ImGuiMouseButton_Left)) &&
            s_editId == 0)
        {
            ImFormatString(s_editBuf, IM_ARRAYSIZE(s_editBuf), "%g", (double)*value);
            s_editId = id;
            s_editFocus = true;
        }
    }
    else
    {
        ImGui::SetCursorScreenPos(box_min);
        ImGui::SetNextItemWidth(box_w - 4.0f);
        if (s_editFocus)
        {
            ImGui::SetKeyboardFocusHere();
            s_editFocus = false;
        }
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 2.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ColorU32(theme.ControlBg));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ColorU32(theme.ControlBg));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ColorU32(theme.ControlBg));
        ImGui::PushStyleColor(ImGuiCol_Text, ColorU32(theme.TextBright));
        ImGui::PushStyleColor(ImGuiCol_Border, ColorU32(ImVec4(0.0f, 0.0f, 0.0f, 0.0f)));
        ImGui::PushStyleColor(ImGuiCol_TextSelectedBg, ColorU32(theme.ControlInactive));
        bool done = ImGui::InputText("##slider_input", s_editBuf, IM_ARRAYSIZE(s_editBuf),
                                     ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);
        ImGui::PopStyleColor(6);
        ImGui::PopStyleVar();
        ImDrawList* fdl = ImGui::GetWindowDrawList();
        const ImVec2 bmin = ImGui::GetItemRectMin(), bmax = ImGui::GetItemRectMax();
        fdl->AddRect(bmin, bmax, ColorU32(theme.Accent, 0.7f), 5.0f, 0, 1.0f);
        const bool esc = ImGui::IsKeyPressed(ImGuiKey_Escape);
        if (done)
        {
            *value = ImClamp((float)atof(s_editBuf), min_value, max_value);
            changed = true;
            s_editId = 0;
        }
        else if (esc)
        {
            s_editId = 0;
        }
        else if ((ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGui::IsMouseClicked(ImGuiMouseButton_Right)) &&
                 !ImGui::IsItemActive() && !ImGui::IsItemHovered())
        {
            s_editId = 0;
        }
    }

    // stepper buttons
    auto stepper = [&](const ImVec2& a, const ImVec2& b, float anim, const char* glyph) {
        draw->AddRectFilled(a, b, ColorU32(LerpColor(theme.ControlBg, theme.ControlInactive, anim * 0.6f)), 4.0f);
        draw->AddRect(a, b, anim > 0.1f ? ColorU32(ImVec4(1,1,1,0.14f)) : OutlineBlack(), 4.0f, 0, 1.0f);
        const ImVec2 ts = font->CalcTextSizeA(12.0f * g_fontScale, FLT_MAX, 0.0f, glyph);
        draw->AddText(font, 12.0f * g_fontScale, ImVec2(std::floor(a.x + (step_w - ts.x) * 0.5f), std::floor(a.y + (height + 4.0f - ts.y) * 0.5f - 1.0f)),
            ColorU32(LerpColor(ImVec4(0.6f,0.6f,0.64f,1.0f), ImVec4(1,1,1,1), anim)), glyph);
    };
    stepper(minus_min, minus_max, minus_anim, "-");
    stepper(plus_min, plus_max, plus_anim, "+");

    // track
    const float trackH = 4.0f;
    const ImVec2 tbMin(track_min.x, track_min.y + 3.5f);
    const ImVec2 tbMax(track_max.x, track_min.y + 3.5f + trackH);
    draw->AddRectFilled(tbMin, tbMax, ColorU32(LerpColor(theme.ControlBg, theme.ControlInactive, hover * 0.3f)), 3.0f);
    draw->AddRect(tbMin, tbMax, OutlineBlack(), 3.0f, 0, 1.0f);
    if (animated_t > 0.001f)
    {
        const ImVec2 fillMax(tbMin.x + track_w * animated_t, tbMax.y);
        if (fillMax.x > tbMin.x + 1.0f) {
            if (animated_t > 0.02f)
                draw->AddRectFilled(tbMin - ImVec2(0, 2), fillMax + ImVec2(0, 2), ColorU32(theme.Accent, 0.16f), 3.0f);
            draw->AddRectFilled(tbMin, fillMax, ColorU32(LerpColor(theme.Accent, theme.TextBright, hover * 0.12f)), 3.0f);
        }
        // knob
        const float kx2 = tbMin.x + track_w * animated_t;
        const float ky2 = (tbMin.y + tbMax.y) * 0.5f;
        const float kr = 5.5f + hover * 1.0f;
        draw->AddCircleFilled(ImVec2(kx2, ky2 + 1.0f), kr, ImGui::GetColorU32(IM_COL32(0, 0, 0, 100)), 20);
        draw->AddCircleFilled(ImVec2(kx2, ky2), kr, IM_COL32(245, 245, 248, 255), 20);
        draw->AddCircle(ImVec2(kx2, ky2), kr, ColorU32(theme.Accent), 20, 1.5f);
        draw->AddCircleFilled(ImVec2(kx2, ky2), 2.0f, ColorU32(theme.Accent), 10);
    }
    else
    {
        const float ky2 = (tbMin.y + tbMax.y) * 0.5f;
        draw->AddCircleFilled(ImVec2(tbMin.x, ky2), 5.0f, IM_COL32(200, 200, 208, 255), 16);
    }

    ImGui::PopID();
    return pressed || changed;
}

struct ComboTextAnimation
{
    std::string Previous;
    std::string Current;
    float Blend = 1.0f;
};

inline void DrawChevron(ImDrawList* draw, ImVec2 c, float size, ImU32 col, float open01)
{
    // morphs down -> up with open amount
    float dir = 1.0f - open01 * 2.0f;
    ImVec2 p1(c.x - size, c.y - size * 0.35f * dir);
    ImVec2 p2(c.x, c.y + size * 0.35f * dir);
    ImVec2 p3(c.x + size, c.y - size * 0.35f * dir);
    draw->AddLine(p1, p2, col, 1.6f);
    draw->AddLine(p2, p3, col, 1.6f);
}

inline bool Combo(const char* label, int* current_item, const char* const items[], int items_count, const ImVec2& pos, float width, const char* text_label)
{
    const float font_size = 12.0f * g_fontScale;
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    const ImVec2 base = window->Pos;
    const ImVec2 min = ImVec2(std::floor(base.x + pos.x + g_contentOffset.x), std::floor(base.y + pos.y + g_contentOffset.y));
    const ImVec2 size(width, 20.0f);
    ImGui::PushID(label);
    ImGui::SetCursorScreenPos(min);
    const bool pressed = ImGui::InvisibleButton("##combo_preview", size);

    const bool hovered = ImGui::IsItemHovered();
    const ImGuiID id = ImGui::GetItemID();
    static ImGuiID open_id = 0;
    if (pressed)
        open_id = open_id == id ? 0 : id;

    const bool open = open_id == id;
    const float hover = AnimateFloat(id, hovered, 18.0f);
    const float open_anim = AnimateFloat(id + 10, open, 16.0f);
    const Theme& theme = GetTheme();
    ImDrawList* draw = ImGui::GetWindowDrawList();

    if (hover > 0.02f || open)
        draw->AddRectFilled(min - ImVec2(2, 2), min + size + ImVec2(2, 2), ColorU32(theme.Accent, (0.10f * hover + 0.12f * open_anim)), 8.0f);
    draw->AddRectFilled(min, min + size, ColorU32(LerpColor(theme.ControlBg, theme.ControlInactive, hover * 0.35f)), 6.0f);
    draw->AddRect(min, min + size, open ? ColorU32(theme.Accent, 0.6f) : (hovered ? ColorU32(ImVec4(1,1,1,0.13f)) : OutlineBlack()), 6.0f, 0, 1.0f);
    draw->AddRect(min + ImVec2(1.0f, 1.0f), min + size - ImVec2(1.0f, 1.0f), OutlineInner(), 5.0f, 0, 1.0f);
    draw->AddLine(min + ImVec2(6.0f, 1.0f), min + ImVec2(size.x - 6.0f, 1.0f), ColorU32(ImVec4(1,1,1,0.05f)), 1.0f);

    const Fonts& fonts = GetFonts();
    ImFont* font = fonts.CascadiaMonoBL ? fonts.CascadiaMonoBL : ImGui::GetFont();
    if (text_label)
        draw->AddText(font, font_size, ImVec2(min.x, min.y - font_size - 3.0f), ColorU32(theme.Text), text_label);
    const char* preview = (*current_item >= 0 && *current_item < items_count) ? items[*current_item] : "";
    static std::unordered_map<ImGuiID, ComboTextAnimation> text_animations;
    ComboTextAnimation& text_anim = text_animations[id];
    if (text_anim.Current.empty())
        text_anim.Current = preview;
    if (text_anim.Current != preview)
    {
        text_anim.Previous = text_anim.Current;
        text_anim.Current = preview;
        text_anim.Blend = 0.0f;
    }
    text_anim.Blend = 1.0f;
    ImVec2 preview_sz = font->CalcTextSizeA(font_size, FLT_MAX, 0.0f, text_anim.Current.c_str());
    const ImVec2 preview_pos(min.x + 8.0f, min.y + (size.y - preview_sz.y) * 0.5f);
    if (!text_anim.Previous.empty() && text_anim.Blend < 0.98f)
        draw->AddText(font, font_size, preview_pos, ColorU32(theme.Text, 1.0f - text_anim.Blend), text_anim.Previous.c_str());
    draw->AddText(font, font_size, preview_pos, ColorU32(LerpColor(theme.Text, theme.TextBright, open_anim * 0.5f), text_anim.Blend), text_anim.Current.c_str());
    DrawChevron(draw, ImVec2(min.x + size.x - 12.0f, min.y + size.y * 0.5f), 4.0f, ColorU32(LerpColor(theme.Text, theme.AccentText, open_anim)), open_anim);

    bool changed = false;
    const float row_height = 19.0f;
    const float popup_padding = 4.0f;
    const float full_height = popup_padding * 2.0f + row_height * items_count;
    const float visible_height = full_height * EaseOutCubic(ImClamp(open_anim, 0.0f, 1.0f));
    ImVec2 boxMin(min.x, min.y + size.y + 4.0f);
    if (boxMin.y + full_height > ImGui::GetIO().DisplaySize.y && min.y - 4.0f - full_height >= 0.0f)
        boxMin.y = min.y - 4.0f - full_height;
    const ImVec2 popup_max(boxMin.x + width, boxMin.y + visible_height);
    const float totalTop = (std::min)(boxMin.y, min.y);
    const float totalBottom = (std::max)(boxMin.y + full_height, min.y + size.y);
    const ImRect total_rect(ImVec2(min.x, totalTop), ImVec2(min.x + width, totalBottom));

    if (open && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !total_rect.Contains(ImGui::GetIO().MousePos)) {
        open_id = 0;
        ComboClosedFrame() = ImGui::GetFrameCount();
    }

    if (open_anim > 0.01f)
    {
        ImDrawList* overlay = ImGui::GetForegroundDrawList();
        overlay->PushClipRect(boxMin - ImVec2(0, 2), popup_max + ImVec2(0, 2), true);
        const ImVec2 popup_box_max = ImVec2(boxMin.x + width, boxMin.y + full_height);
        overlay->AddRectFilled(boxMin + ImVec2(0, 4), popup_box_max + ImVec2(0, 4), ImGui::GetColorU32(IM_COL32(0, 0, 0, (int)(70 * open_anim))), 8.0f);
        overlay->AddRectFilled(boxMin, popup_box_max, ColorU32(theme.CardBg, open_anim), 8.0f);
        overlay->AddRect(boxMin, popup_box_max, ImGui::GetColorU32(IM_COL32(0, 0, 0, (int)(170 * open_anim))), 8.0f, 0, 1.0f);
        overlay->AddRect(boxMin + ImVec2(1, 1), popup_box_max - ImVec2(1, 1), ImGui::GetColorU32(IM_COL32(255, 255, 255, (int)(16 * open_anim))), 7.0f, 0, 1.0f);

        for (int i = 0; i < items_count; ++i)
        {
            const ImVec2 item_min(boxMin.x + 3.0f, boxMin.y + popup_padding + row_height * i);
            const ImVec2 item_max(boxMin.x + width - 3.0f, item_min.y + row_height);
            const ImRect item_rect(item_min, item_max);
            const bool item_hovered = item_rect.Contains(ImGui::GetIO().MousePos);
            const bool item_pressed = open && open_anim > 0.60f && item_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left);
            ImGui::PushID(i);
            const ImGuiID item_id = ImGui::GetID("combo_item");
            const float item_hover = AnimateFloat(item_id, item_hovered, 18.0f);
            const float item_selected = AnimateFloat(item_id + 1, i == *current_item, 18.0f);
            const ImVec4 row_color = LerpColor(theme.CardBg, theme.ControlInactive, item_hover * 0.8f + item_selected * 0.35f);
            if (item_hover > 0.01f || item_selected > 0.01f)
                overlay->AddRectFilled(item_min, item_max, ColorU32(row_color, open_anim), 5.0f);
            if (i == *current_item)
                overlay->AddRectFilled(ImVec2(item_min.x, item_min.y + 3.0f), ImVec2(item_min.x + 2.0f, item_max.y - 3.0f), ColorU32(theme.Accent, open_anim), 1.0f);
            const ImVec4 text_color = LerpColor(theme.Text, theme.TextBright, item_hover * 0.35f + item_selected * 0.55f);
            overlay->AddText(font, font_size, ImVec2(item_min.x + 8.0f, item_min.y + (row_height - font_size) * 0.5f), ColorU32(text_color, open_anim), items[i]);
            if (item_pressed)
            {
                *current_item = i;
                changed = true;
                open_id = 0;
            }
            ImGui::PopID();
        }
        overlay->PopClipRect();
    }
    if (open_id == id)
        ComboOpenId() = id;
    else if (ComboOpenId() == id) {
        ComboOpenId() = 0;
        ComboClosedFrame() = ImGui::GetFrameCount();
    }
    ImGui::PopID();
    return changed;
}

struct MultiComboTextAnimation
{
    std::string Previous;
    std::string Current;
    float Blend = 1.0f;
};

inline bool MultiCombo(const char* label, bool values[], const char* const items[], int items_count, const ImVec2& pos, float width, const char* text_label)
{
    const float font_size = 12.0f * g_fontScale;
    std::string preview;
    int selected_count = 0;
    for (int i = 0; i < items_count; ++i)
    {
        if (!values[i])
            continue;
        ++selected_count;
        if (!preview.empty())
            preview += ", ";
        preview += items[i];
    }
    if (selected_count > 3)
        preview = std::to_string(selected_count) + " selected";
    if (preview.empty())
        preview = "Select...";

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    const ImVec2 base = window->Pos;
    const ImVec2 min = ImVec2(std::floor(base.x + pos.x + g_contentOffset.x), std::floor(base.y + pos.y + g_contentOffset.y));
    const ImVec2 size(width, 20.0f);
    const ImVec2 hit_size(width, 22.0f);
    ImGui::PushID(label);
    ImGui::SetCursorScreenPos(min - ImVec2(0.0f, 0.0f));
    const bool pressed = ImGui::InvisibleButton("##multicombo_preview", hit_size);

    const bool hovered = ImGui::IsItemHovered();
    const ImGuiID id = ImGui::GetItemID();
    static ImGuiID open_id = 0;
    if (pressed)
        open_id = open_id == id ? 0 : id;

    const bool open = open_id == id;
    const float hover = AnimateFloat(id, hovered, 16.0f);
    const float open_anim = AnimateFloat(id + 10, open, 16.0f);
    const Theme& theme = GetTheme();
    ImDrawList* draw = ImGui::GetWindowDrawList();
    if (hover > 0.02f || open)
        draw->AddRectFilled(min - ImVec2(2, 2), min + size + ImVec2(2, 2), ColorU32(theme.Accent, 0.10f * hover + 0.12f * open_anim), 8.0f);
    draw->AddRectFilled(min, min + size, ColorU32(LerpColor(theme.ControlBg, theme.ControlInactive, hover * 0.35f)), 6.0f);
    draw->AddRect(min, min + size, open ? ColorU32(theme.Accent, 0.6f) : (hovered ? ColorU32(ImVec4(1,1,1,0.13f)) : OutlineBlack()), 6.0f, 0, 1.0f);
    draw->AddRect(min + ImVec2(1.0f, 1.0f), min + size - ImVec2(1.0f, 1.0f), OutlineInner(), 5.0f, 0, 1.0f);

    const Fonts& fonts = GetFonts();
    ImFont* font = fonts.CascadiaMonoBL ? fonts.CascadiaMonoBL : ImGui::GetFont();
    if (text_label)
        draw->AddText(font, font_size, ImVec2(min.x, min.y - font_size - 3.0f), ColorU32(theme.Text), text_label);
    static std::unordered_map<ImGuiID, MultiComboTextAnimation> text_animations;
    MultiComboTextAnimation& text_anim = text_animations[id];
    if (text_anim.Current.empty())
        text_anim.Current = preview;
    if (text_anim.Current != preview)
    {
        text_anim.Previous = text_anim.Current;
        text_anim.Current = preview;
        text_anim.Blend = 0.0f;
    }
    text_anim.Blend = 1.0f;
    const ImVec2 preview_pos(min.x + 8.0f, min.y + (size.y - font_size) * 0.5f);
    // clip preview text to box
    draw->PushClipRect(min + ImVec2(4, 0), min + size - ImVec2(18, 0), true);
    if (!text_anim.Previous.empty() && text_anim.Blend < 0.98f)
        draw->AddText(font, font_size, preview_pos, ColorU32(theme.Text, 1.0f - text_anim.Blend), text_anim.Previous.c_str());
    draw->AddText(font, font_size, preview_pos, ColorU32(selected_count ? theme.TextBright : theme.Text, text_anim.Blend), text_anim.Current.c_str());
    draw->PopClipRect();
    if (selected_count > 0) {
        char cnt[16]; ImFormatString(cnt, IM_ARRAYSIZE(cnt), "%d", selected_count);
        ImVec2 cs = font->CalcTextSizeA(10.0f * g_fontScale, FLT_MAX, 0, cnt);
        ImVec2 bmin(min.x + size.x - 22.0f - cs.x - 10.0f, min.y + (size.y - 14.0f) * 0.5f);
        // badge sits left of chevron
        draw->AddRectFilled(bmin - ImVec2(4, 1), bmin + ImVec2(cs.x + 4, 13), ColorU32(theme.Accent, 0.85f), 4.0f);
        draw->AddText(font, 10.0f * g_fontScale, bmin, IM_COL32(255, 255, 255, 255), cnt);
    }
    DrawChevron(draw, ImVec2(min.x + size.x - 12.0f, min.y + size.y * 0.5f), 4.0f, ColorU32(LerpColor(theme.Text, theme.AccentText, open_anim)), open_anim);

    bool changed = false;
    const float row_height = 19.0f;
    const float popup_padding = 4.0f;
    const float full_height = popup_padding * 2.0f + row_height * items_count;
    const float visible_height = full_height * EaseOutCubic(ImClamp(open_anim, 0.0f, 1.0f));
    const ImVec2 popup_min(min.x, min.y + size.y + 4.0f);
    const ImVec2 popup_max(popup_min.x + width, popup_min.y + visible_height);
    const ImRect total_rect(min, ImVec2(min.x + width, popup_min.y + full_height));

    if (open && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !total_rect.Contains(ImGui::GetIO().MousePos)) {
        open_id = 0;
        ComboClosedFrame() = ImGui::GetFrameCount();
    }

    if (open_anim > 0.01f)
    {
        ImDrawList* overlay = ImGui::GetForegroundDrawList();
        overlay->PushClipRect(popup_min - ImVec2(0, 2), popup_max + ImVec2(0, 2), true);
        const ImVec2 popup_box_max = ImVec2(popup_min.x + width, popup_min.y + full_height);
        overlay->AddRectFilled(popup_min + ImVec2(0, 4), popup_box_max + ImVec2(0, 4), ImGui::GetColorU32(IM_COL32(0, 0, 0, (int)(70 * open_anim))), 8.0f);
        overlay->AddRectFilled(popup_min, popup_box_max, ColorU32(theme.CardBg, open_anim), 8.0f);
        overlay->AddRect(popup_min, popup_box_max, ImGui::GetColorU32(IM_COL32(0, 0, 0, (int)(170 * open_anim))), 8.0f, 0, 1.0f);
        overlay->AddRect(popup_min + ImVec2(1, 1), popup_box_max - ImVec2(1, 1), ImGui::GetColorU32(IM_COL32(255, 255, 255, (int)(16 * open_anim))), 7.0f, 0, 1.0f);

        for (int i = 0; i < items_count; ++i)
        {
            const ImVec2 item_min(popup_min.x + 3.0f, popup_min.y + popup_padding + row_height * i);
            const ImVec2 item_max(popup_min.x + width - 3.0f, item_min.y + row_height);
            const ImRect item_rect(item_min, item_max);
            const bool item_hovered = item_rect.Contains(ImGui::GetIO().MousePos);
            const bool item_pressed = open && open_anim > 0.60f && item_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left);
            ImGui::PushID(i);
            const ImGuiID item_id = ImGui::GetID("multicombo_item");
            const float item_hover = AnimateFloat(item_id, item_hovered, 18.0f);
            const float item_selected = AnimateFloat(item_id + 1, values[i], 18.0f);
            const ImVec4 row_color = LerpColor(theme.CardBg, theme.ControlInactive, item_hover * 0.8f + item_selected * 0.35f);
            if (item_hover > 0.01f || item_selected > 0.01f)
                overlay->AddRectFilled(item_min, item_max, ColorU32(row_color, open_anim), 5.0f);
            // mini switch on the left
            const ImVec2 swMin(item_min.x + 6.0f, item_min.y + (row_height - 12.0f) * 0.5f);
            const ImVec2 swMax(swMin.x + 20.0f, swMin.y + 12.0f);
            overlay->AddRectFilled(swMin, swMax, ColorU32(LerpColor(theme.ControlBg, theme.Accent, item_selected), open_anim), 6.0f);
            const float kxx = swMin.x + 3.0f + item_selected * (20.0f - 6.0f);
            overlay->AddCircleFilled(ImVec2(kxx, (swMin.y + swMax.y) * 0.5f), 4.0f, ImGui::GetColorU32(IM_COL32(245, 245, 248, (int)(255 * open_anim))), 12);
            const ImVec4 text_color = LerpColor(theme.Text, theme.TextBright, item_hover * 0.35f + item_selected * 0.55f);
            overlay->AddText(font, font_size, ImVec2(item_min.x + 32.0f, item_min.y + (row_height - font_size) * 0.5f), ColorU32(text_color, open_anim), items[i]);
            if (item_pressed)
            {
                values[i] = !values[i];
                changed = true;
            }
            ImGui::PopID();
        }
        overlay->PopClipRect();
    }
    if (open_id == id)
        ComboOpenId() = id;
    else if (ComboOpenId() == id) {
        ComboOpenId() = 0;
        ComboClosedFrame() = ImGui::GetFrameCount();
    }
    ImGui::PopID();
    return changed;
}
}
