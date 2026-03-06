#include "pch.h"
#include "Finder.h"

static constexpr const char* OutputPath = "C:/UEFN-AESFinder";

static std::string ColorText(const std::string& text, int32_t color)
{
    return std::format("\033[{}m{}\033[0m", color, text);
}

static bool CreateOutputFolder()
{
    fs::path filePath(OutputPath);
    try
    {
        if (!fs::exists(filePath))
        {
            fs::create_directories(filePath);
        }
    }
    catch (const fs::filesystem_error& e)
    {
        std::string error(std::format("Could not create the required folder: {}. Using the game one.", e.what()));
        printf("[-] %s\n", ColorText(error, COLOR_ERROR).c_str());
        MessageBoxA(nullptr, (error.c_str()), ("UEFN AESFinder"), MB_ICONERROR);
        return false;
    }

    return true;
}

#define ENTROPY 3.3

static std::unordered_set<size_t> GetPreferredKeys(std::vector<double> const& entropies, double threshold = ENTROPY)
{
    double current_max = -DBL_MAX;

    for (std::size_t i = 0; i < entropies.size(); ++i)
    {
        if (entropies[i] < threshold) continue;

        if (entropies[i] > current_max)
            current_max = entropies[i];
    }

    std::unordered_set<size_t> indices;
    for (std::size_t i = 0; i < entropies.size(); ++i)
    {
        if (entropies[i] == current_max)
            indices.insert(i);
    }

    return indices;
}

static DWORD MainThread(LPVOID param)
{
    AllocConsole();

    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);

    HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE); // js to have colors bruh
    SetConsoleMode(hOutput, ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

    printf("\n\n"); // spacing
    printf("%s \n", ColorText("[*] UEFN AESFinder - Made by @djlorenzouasset (original project by @GHFear)", COLOR_INFO).c_str());

    // UEFN is splitted in 2 different DLLs since v38.00 for some reason, and the main key is in UnrealEditorFortnite-Common-Win64-Shipping.dll
    HANDLE uefnModule = GetModuleHandleA("UnrealEditorFortnite-Common-Win64-Shipping.dll");
    if (!uefnModule)
    {
        printf("%s \n", ColorText("[*] UEFN version <v38.00: using main executable.", COLOR_INFO).c_str());
        uefnModule = GetModuleHandleA(nullptr);
    }

    printf("%s \n", ColorText("[*] Starting scanning for keys..", COLOR_INFO).c_str());

    Finder finder;
    if (!finder.ScanForKeys((uintptr_t)uefnModule))
    {
        printf("[-] %s\n\n", ColorText("No keys found!", COLOR_WARN).c_str());
        MessageBoxA(nullptr, "No AES keys found. If you think this is an error, open an issue on GitHub", ("UEFN AESFinder"), MB_ICONERROR | MB_OK);
        FreeLibraryAndExitThread((HMODULE)param, 1);
    }

    fs::path savePath;
    if (CreateOutputFolder()) savePath = fs::path(OutputPath) / "aes_list.txt";
    else savePath = "aes_list.txt";

    std::ofstream outputFile(savePath);

    std::string keysInfo = std::format("[*] Keys: {} | Entropies: {}", 
        (int32_t)finder.Keys.size(), (int32_t)finder.Entropies.size());

    printf("%s \n", ColorText(keysInfo, COLOR_DEBUG).c_str());
    outputFile << keysInfo << "\n";

    // get only the found keys that seems more like REAL keys
    auto preferredKeys = GetPreferredKeys(finder.Entropies);

    std::string preferredKeysInfo = std::format("[*] Preferred Keys: {} (Entropy > {})", (int32_t)preferredKeys.size(), ENTROPY);
    printf("%s \n", ColorText(preferredKeysInfo, COLOR_DEBUG).c_str());
    outputFile << preferredKeysInfo << "\n\n";

    for (size_t i = 0; i < finder.Keys.size(); i++)
    {
        if (!preferredKeys.count(i)) continue;

        std::string keyStr = std::format("[*] Key: 0x{} @ 0x{:X}", 
            finder.Keys[i].Key.c_str(), finder.Keys[i].Address);

        printf("%s \n", ColorText(keyStr, COLOR_WARN).c_str());
        outputFile << keyStr << "\n";
    }

    MessageBoxA(nullptr, std::format("Found {} (possibles*) AES Keys.", 
        (int32_t)preferredKeys.size()).c_str(), ("UEFN AESFinder"), MB_ICONINFORMATION | MB_OK);

    outputFile.close();
    FreeLibraryAndExitThread((HMODULE)param, 0); // automatically unload the DLL
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(module);
        HANDLE thread = CreateThread(nullptr, 0, MainThread, module, 0, nullptr);
        if (thread) CloseHandle(thread);
    }

    return TRUE;
}

