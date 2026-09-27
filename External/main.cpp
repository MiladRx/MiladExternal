#include <atomic>
#include <cstdint>
#include <iostream>
#include <thread>
#include <chrono>
#include <windows.h>
#include <shellapi.h>
#include "src/core/app/app.h"
#include "src/core/logger/logger.h"
#include "src/memory/memory.h"
#include "src/core/globals/globals.h"
#include "src/sdk/offsets.h"

namespace {
constexpr WORD C_WHITE  = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
constexpr WORD C_DIM    = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
constexpr WORD C_GREEN  = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
constexpr WORD C_RED    = FOREGROUND_RED | FOREGROUND_INTENSITY;
constexpr WORD C_YELLOW = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
constexpr WORD C_CYAN   = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;

void con_color(WORD c) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void rule() {
    con_color(C_DIM);
    std::cout << "  ------------------------------\n";
    con_color(C_WHITE);
}

void header() {
    std::cout << "\n";
    con_color(C_CYAN);
    std::cout << "  MiladExternal";
    con_color(C_DIM);
    std::cout << "  v1.0\n";
    con_color(C_WHITE);
    rule();
}

void row(const char* key, const char* val, WORD valColor) {
    char padded[16];
    snprintf(padded, sizeof(padded), "%-10s", key);
    con_color(C_DIM);
    std::cout << "  " << padded << "  ";
    con_color(valColor);
    std::cout << val << "\n";
    con_color(C_WHITE);
}

bool is_admin() {
    BOOL admin = FALSE;
    SID_IDENTIFIER_AUTHORITY nt = SECURITY_NT_AUTHORITY;
    PSID group = nullptr;
    if (AllocateAndInitializeSid(&nt, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS,
        0, 0, 0, 0, 0, 0, &group)) {
        CheckTokenMembership(nullptr, group, &admin);
        FreeSid(group);
    }
    return admin != FALSE;
}

void relaunch_as_admin() {
    wchar_t exe[MAX_PATH]{};
    GetModuleFileNameW(nullptr, exe, MAX_PATH);
    SHELLEXECUTEINFOW sei{};
    sei.cbSize = sizeof(sei);
    sei.lpVerb = L"runas";
    sei.lpFile = exe;
    sei.nShow = SW_SHOWNORMAL;
    if (!ShellExecuteExW(&sei))
        row("admin", "elevation declined - run as administrator", C_RED);
}

void stage_watcher(std::atomic<bool>& stop) {
    static const char* spin = "|/-\\";
    int si = 0;
    int shown = 0;
    while (!stop.load()) {
        const int s = App::stage.load();
        if (s < 1) {
            con_color(C_DIM);
            std::cout << "\r  roblox      " << spin[si++ % 4] << "  waiting for RobloxPlayerBeta.exe" << std::flush;
            con_color(C_WHITE);
        } else {
            if (shown < 1) {
                shown = 1;
                std::cout << "\r" << std::string(56, ' ') << "\r";
                char buf[192];
                snprintf(buf, sizeof(buf), "pid %u  base 0x%llX  %s",
                    (unsigned)memory->get_process_id(),
                    (unsigned long long)memory->get_module_address(),
                    Offsets::ClientVersion.c_str());
                row("attached", buf, C_WHITE);
            }
            if (s >= 2 && shown < 2) {
                shown = 2;
                row("overlay", "ready", C_GREEN);
            }
            if (s >= 3 && shown < 3) {
                shown = 3;
                row("menu", "press INSERT", C_CYAN);
                rule();
                return;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(120));
    }
    std::cout << "\n";
    con_color(C_WHITE);
}
}

std::int32_t main() {
    SetUnhandledExceptionFilter(CrashHandler);
    AddVectoredExceptionHandler(1, [](PEXCEPTION_POINTERS ep)->LONG {
        DWORD code = ep->ExceptionRecord->ExceptionCode;
        if (code==0x406D1388 || code==0x40010006 || code==0xE06D7363) return EXCEPTION_CONTINUE_SEARCH;
        static long long lastMs=0; static int count=0;
        long long now = GetTickCount64();
        if (now - lastMs < 500) return EXCEPTION_CONTINUE_SEARCH;
        if (count++ > 20) return EXCEPTION_CONTINUE_SEARCH;
        lastMs = now;
        Logger::write_crash(ep, "VEH");
        return EXCEPTION_CONTINUE_SEARCH;
    });
    Logger::quiet = true;
    Logger::init();
    SetConsoleTitleA("MiladExternal");

    header();
    if (!is_admin()) {
        row("admin", "elevation required, relaunching", C_YELLOW);
        relaunch_as_admin();
        return 0;
    }
    row("admin", "ok", C_GREEN);
    std::atomic<bool> stopWatcher{false};
    std::thread watcher(stage_watcher, std::ref(stopWatcher));
    const std::int32_t code = App::Run();
    stopWatcher.store(true);
    if (watcher.joinable()) watcher.join();
    if (code == 0) {
        row("session", "ended cleanly", C_GREEN);
    } else {
        char buf[128]; snprintf(buf, sizeof(buf), "exited with code %d - see MiladExternal_log.txt", code);
        row("session", buf, C_RED);
        system("pause >nul");
    }
    return code;
}
