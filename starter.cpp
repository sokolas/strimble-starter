#include <windows.h>
#include "sol.hpp"

void ShowConsole() {
    HWND cwnd = GetConsoleWindow();
    ShowWindow(cwnd, SW_SHOW);
}

void HideConsole() {
    HWND cwnd = GetConsoleWindow();
    ShowWindow(cwnd, SW_HIDE);
}

void redirect()
{
    if (AllocConsole()) {
        SetConsoleCP(CP_UTF8);
        SetConsoleOutputCP(CP_UTF8);
        FILE* file = nullptr;
        freopen_s(&file, "CONOUT$", "wt", stdout);
        freopen_s(&file, "CONOUT$", "wt", stderr);
        freopen_s(&file, "CONIN$", "rt", stdin);
        SetConsoleTitle(L"Debug Console");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
        SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR lpCmdLine, int nCmdShow)
//int main(int argc, char* argv[])
{
    redirect();
    
    std::vector<wchar_t> pathBuf;
    DWORD copied = 0;
    do {
        pathBuf.resize(pathBuf.size() + MAX_PATH);
        copied = GetModuleFileName(0, &pathBuf.at(0), pathBuf.size());
    } while (copied >= pathBuf.size());

    pathBuf.resize(copied);

    std::string path(pathBuf.begin(), pathBuf.end());

    //std::cout << path;

    std::cout << "hello world from C++\n";
    //std::cout << std::string(lpCmdLine) << std::endl;

    sol::state lua;
    lua.open_libraries();

    lua["ShowConsole"] = ShowConsole;
    lua["HideConsole"] = HideConsole;
    //lua["b"] = std::string(lpCmdLine);
    sol::load_result script = lua.load(
        "local arg = table.pack(...); \n"
        "xpcall("
        "function()"
        "(loadfile('src/main.lua'))(unpack(arg));"
        "end,"
        "function(err) print('Uncaught lua script exception',debug.traceback(err)); print('Press Enter...'); io.read() end"
        ")");
    //lua.script("local f = loadfile('main.lua'); f();");
    script(path, std::string(lpCmdLine));
    //Sleep(5000);
    return 0;
}
