#include <fstream>
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
		exit(1);
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
					exit(1);
				}

				return _payload;
			}
			catch (...)
			{
				MessageBox(NULL, L"Invalid Payload Name", L"AntiLeague ERROR", MB_OK | MB_ICONERROR);
				exit(1);
			}
		}
	}

	MessageBox(NULL, L"Failed To Find Payload", L"AntiLeague ERROR", MB_OK | MB_ICONERROR);
	exit(1);
}

void c_AntiLeague::LoadPayload(const std::string& payloadLocation)
{
	std::ifstream infile(payloadLocation, std::ios::binary);

	if (!infile.good())
	{
		MessageBox(NULL, L"Can't Open Payload", L"AntiLeague ERROR", MB_OK | MB_ICONERROR);
		exit(1);
	}

	OVERLAPPED Overlapped = { NULL };
	HANDLE hPayloadFile = CreateFileA(payloadLocation.c_str(), GENERIC_ALL, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	LockFileEx(hPayloadFile, NULL, NULL, MAXDWORD, MAXDWORD, &Overlapped);

	std::string payloadData((std::istreambuf_iterator<char>(infile)), (std::istreambuf_iterator<char>()));
	infile.close();

	if (payloadData.empty())
	{
		MessageBox(NULL, L"Payload Empty", L"AntiLeague ERROR", MB_OK | MB_ICONERROR);
		exit(1);
	}

	payload = std::move(payloadData);

	cipher();

	hPayload = MemoryLoadLibrary(payload.c_str(), payload.size());

	gen_dec_key();
	cipher();

	if (hPayload == NULL)
	{
		MessageBox(NULL, L"Failed To Load Payload", L"AntiLeague ERROR", MB_OK | MB_ICONERROR);
		exit(1);
	}
}