// https://github.com/GHFear/AESDumpster/blob/main/AESDumpster/AESDumpster/KeyTools/KeyDumpster.h

#pragma once
#include "framework.h"

struct Key
{
public:
    std::string Key;
    const char* Pattern;
    uintptr_t Address;
};

class Finder
{
private:
    /*
    * Here im using only 2 set of offsets and 2 patterns.
    * The original project uses 4 and 4 (https://github.com/GHFear/AESDumpster/blob/main/AESDumpster/AESDumpster/KeyTools/KeyDumpster.h#L67)
    * but we tested it in UEFN and only those are used, so to reduce the scanning I left only these 2 for now.
    */
    const std::vector<std::vector<int32_t>> offsets = {
        { 3, 10, 17, 24, 35, 42, 49, 56 },
        { 3, 10, 21, 28, 35, 42, 49, 56 },
    };

    const std::vector<const char*> patterns = {
        "C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ?",
        "C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? 48 ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ?",
    };

public:
    std::vector<Key> Keys;
    std::vector<double> Entropies;

private:
    std::vector<uint8_t*> ScanPattern(const char* Pattern, uint8_t* Start, uint8_t* End);

    std::string Concatenate(uint8_t* addr, int32_t type);

    double CalcEntropy(std::string Key);
    void GenerateEntropies();

public:
    bool ScanForKeys(uintptr_t moduleBase);
};