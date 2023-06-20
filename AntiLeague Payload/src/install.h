#pragma once

class Installer
{
private:
	const std::string& payload;
	const std::string& dec_key;

	std::string getInstallPath();

public:
	Installer(const std::string& payload, const std::string& dec_key) 
		: payload(payload), dec_key(dec_key)
	{ }

	void Install();
	static void Uninstall();
};