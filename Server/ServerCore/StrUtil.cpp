#include "pch.h"
#include "StrUtil.h"
#pragma warning(disable : 4244)

std::wstring StrUtil::ToUpper(const std::wstring& str)
{
	std::wstring ret = str;
	for (int32_t i = 0; i < str.size(); i++)
		ret[i] = std::toupper(str[i]);
	return ret;
}

std::wstring StrUtil::ToLower(const std::wstring& str)
{
	std::wstring ret = str;
	for (int32_t i = 0; i < str.size(); i++)
		ret[i] = std::tolower(str[i]);
	return ret;
}

std::wstring StrUtil::ToWideStr(const std::string& str)
{
	std::wstring wstr;
	wstr.assign(str.begin(), str.end());
	return wstr;
}

std::wstring StrUtil::ToWideStr(const CHAR* str)
{
	wchar_t buffer[4096] = {};
	auto len = ::MultiByteToWideChar(CP_UTF8, 0, str, -1, buffer, sizeof(buffer) / sizeof(wchar_t));
	if (len == 0)
		return L"";
	return buffer;
}

std::wstring StrUtil::ToWideStr(const WCHAR* str)
{
	return ToWideStr(ToString(str));
}

std::string StrUtil::ToString(const std::wstring& str)
{
	char buffer[4096] = {};
	auto len = ::WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, buffer, sizeof(buffer), nullptr, nullptr);
	return buffer;
}

std::string StrUtil::ToString(const WCHAR* str)
{
	char buffer[4096] = {};
	auto len = ::WideCharToMultiByte(CP_ACP, 0, str, -1, buffer, sizeof(buffer), nullptr, nullptr);
	return buffer;
}

std::string StrUtil::ToString(const CHAR* str)
{
	return ToString(ToWideStr(str));
}
