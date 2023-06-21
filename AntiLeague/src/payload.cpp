#include "stdafx.h"
#include "AntiLeague.h"

std::string AntiLeague::getPayload()
{
	std::string payloadPath;
	std::vector<std::string> directories;

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
		directories.push_back(it->path().string());
	}

	for (auto itr = directories.begin(); itr != directories.end(); ++itr)
	{
		if (itr->find("AntiLeague - ") != std::string::npos)
		{
			try
			{
				payloadPath = itr->c_str();
				dec_key = itr->substr(itr->rfind("AntiLeague - ") + 13);

				if (dec_key.size() != 10)
				{
					MessageBox(NULL, L"Invalid Decryption Key Format", L"AntiLeague ERROR", MB_OK | MB_ICONERROR);
					exit(1);
				}

				return payloadPath;
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

void AntiLeague::LoadPayload(const std::string& payloadPath)
{
	std::ifstream infile(payloadPath, std::ios::binary);

	if (!infile.good())
	{
		MessageBox(NULL, L"Can't Open Payload", L"AntiLeague ERROR", MB_OK | MB_ICONERROR);
		exit(1);
	}

	std::string payloadData((std::istreambuf_iterator<char>(infile)), (std::istreambuf_iterator<char>()));
	infile.close();

	OVERLAPPED Overlapped = { NULL };
	HANDLE hPayloadFile = CreateFileA(payloadPath.c_str(), GENERIC_ALL, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	LockFileEx(hPayloadFile, LOCKFILE_EXCLUSIVE_LOCK, NULL, MAXDWORD, MAXDWORD, &Overlapped);

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