#include "pch.h"
#include "Finder.h"

std::vector<uint8_t*> Finder::ScanPattern(const char* Pattern, uint8_t* Start, uint8_t* End)
{
	auto Pattern2Bytes = [](const char* pattern) -> std::vector<int16_t>
	{
		std::vector<int16_t> bytes;

		char* const start = const_cast<char*>(pattern);
		char* const end = const_cast<char*>(pattern) + strlen(pattern);
		for (char* current = start; current < end; ++current)
		{
			if (*current == '?')
			{
				++current;
				if (*current == '?') ++current;
				bytes.push_back(-1);
			}
			else
			{
				bytes.push_back(static_cast<int16_t>(strtoul(current, &current, 16)));
			}
		}

		return bytes;
	};

	std::vector<uint8_t*> keysLoc;

	const std::vector<int16_t> pat = Pattern2Bytes(Pattern);
	const size_t patLen = pat.size();

	for (uint8_t* pCur = Start; pCur <= End - patLen; ++pCur)
	{
		bool found = true;
		for (size_t i = 0; i < patLen; ++i)
		{
			if (pat[i] != -1 && pCur[i] != static_cast<uint8_t>(pat[i]))
			{
				found = false;
				break;
			}
		}
		if (found) keysLoc.push_back(pCur);
	}

	return keysLoc;
}

std::string Finder::Concatenate(uint8_t* addr, int32_t type)
{
	auto ToHexStr = [](uint8_t* data, int32_t len = 4) -> std::string
	{
		std::stringstream ss;
		ss << std::hex;
		for (int32_t i = 0; i < len; i++)
		{
			ss << std::setw(2) << std::setfill('0') << (int32_t)data[i];
		}

		return ss.str();
	};

	std::string hex_string = "";
	for (size_t i = 0; i < offsets[type].size(); i++)
	{
		hex_string += ToHexStr(&addr[offsets[type][i]]);
	}

	std::transform(hex_string.begin(), hex_string.end(), hex_string.begin(), ::toupper);
	return hex_string;
}

double Finder::CalcEntropy(std::string Key)
{
	std::map<char, int> frequencies;
	for (char c : Key) frequencies[c]++;

	size_t numlen = Key.length();
	double infocontent = 0;
	for (std::pair<char, int> p : frequencies)
	{
		double freq = static_cast<double>(p.second) / numlen;
		infocontent += freq * (log(freq) / log(2));
	}

	infocontent *= -1;
	return infocontent;
}

void Finder::GenerateEntropies()
{
	for (size_t i = 0; i < Keys.size(); i++)
	{
		Entropies.push_back(CalcEntropy(Keys[i].Key));
	}
}

bool Finder::ScanForKeys(uintptr_t moduleBase)
{
	IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)moduleBase;
	IMAGE_NT_HEADERS* ntHeaders = (IMAGE_NT_HEADERS*)(moduleBase + dosHeader->e_lfanew);
	IMAGE_SECTION_HEADER* section = IMAGE_FIRST_SECTION(ntHeaders);

	for (WORD i = 0; i < ntHeaders->FileHeader.NumberOfSections; i++, section++)
	{
		uint8_t* start = (uint8_t*)(moduleBase + section->VirtualAddress);
		uint8_t* end = start + section->Misc.VirtualSize;

		for (size_t j = 0; j < patterns.size(); j++)
		{
			std::vector<uint8_t*> keys_addresses = ScanPattern(patterns[j], start, end);
			for (size_t k = 0; k < keys_addresses.size(); k++)
			{
				Key key{};
				key.Key = Concatenate(keys_addresses[k], (int32_t)j);
				key.Pattern = patterns[j];
				key.Address = (uintptr_t)keys_addresses[k];
				Keys.push_back(key);
			}
		}
	}

	GenerateEntropies();

	if (Entropies.empty() || Keys.empty())
		return false;

	return true;
}
