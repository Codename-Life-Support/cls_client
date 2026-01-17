#pragma once
struct GuidHasher
{
	std::size_t operator()(const GUID& g) const noexcept
	{
		const uint64_t* p = reinterpret_cast<const uint64_t*>(&g);
		// Combine the two 64-bit halves
		return std::hash<uint64_t>()(p[0]) ^ (std::hash<uint64_t>()(p[1]) << 1);
	}
};

struct GuidEqual
{
	bool operator()(const GUID& a, const GUID& b) const noexcept
	{
		return memcmp(&a, &b, sizeof(GUID)) == 0;
	}
};



std::string wchar_to_utf8(const std::wstring& wstr) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	return conv.to_bytes(wstr);
}

