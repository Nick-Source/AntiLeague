#pragma once
#include <string>

class c_Install
{
private:
	std::string  getInstallPath();
	std::string* payload;
	const std::string& dec_key;

public:
	c_Install(std::string* _payload, const std::string& _dec_key) 
		: payload(_payload), dec_key(_dec_key)
	{ }

	void Install();
};