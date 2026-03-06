#pragma once
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <filesystem>
#include <unordered_set>

#define COLOR_INFO 32
#define COLOR_WARN 33
#define COLOR_ERROR 31
#define COLOR_DEBUG 96

namespace fs = std::filesystem;