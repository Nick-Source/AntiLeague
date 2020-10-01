#include <filesystem>
#include "c_AntiLeague.h"

std::string c_AntiLeague::getPayload()
{
	std::string _payload;
	std::vector<std::string> _Directories;

	namespace FS = std::filesystem;

	wchar_t szExeFileName[MAX_PATH];
	GetModuleFileName(NULL, szExeFileName, MAX_PATH);

	std::wstring path = szExeFileName;

	try
	{
		path.erase(path.find_last_of(L"\\/"));
	}
	catch (...)
	{
		MessageBox(NULL, L"Can't Retrieve Path", L"AntiLeague ERROR", MB_OK | MB_ICONERROR);
		std::exit(1);
	}

	for (auto it = FS::begin(FS::directory_iterator(path)); it != FS::end(FS::directory_iterator(path)); ++it)
	{
		_Directories.push_back(it->path().string());
	}

	for (auto itr = _Directories.begin(); itr != _Directories.end(); ++itr)
	{
		if (itr->find("AntiLeague - ") != std::string::npos)
		{
			try
			{
				_payload = itr->c_str();
				dec_key = itr->substr(itr->rfind("AntiLeague - ") + 13);

				if (dec_key.size() != 10)
				{
					MessageBox(NULL, L"Invalid Decryption Key Format", L"AntiLeague ERROR", MB_OK | MB_ICONERROR);
					std::exit(1);
				}

				return _payload;
			}
			catch (...)
			{
				MessageBox(NULL, L"Invalid Payload Name", L"AntiLeague ERROR", MB_OK | MB_ICONERROR);
				std::exit(1);
			}
		}
	}

	MessageBox(NULL, L"Failed To Find Payload", L"AntiLeague ERROR", MB_OK | MB_ICONERROR);
	std::exit(1);
}