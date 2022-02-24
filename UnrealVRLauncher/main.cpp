#include <string>
#include "Launcher.h"

int main(int argc, char* argv[])
{
	UnrealVR::Logger::Init(false);
	std::wstring gamePath = L"C:\\Program Files (x86)\\GOG Galaxy\\Games\\Ghostrunner\\Ghostrunner\\Binaries\\Win64\\Ghostrunner-Win64-Shipping.exe";
	if (!UnrealVR::Launcher::StartGame(gamePath))
	{
		UnrealVR::Logger::Error(L"Failed to start game");
		return 1;
	}
	if (!UnrealVR::Launcher::InjectDLL(std::string(argv[0])))
	{
		UnrealVR::Logger::Error(L"Failed to inject DLL");
		return 1;
	}
	Sleep(3000);
	return 0;
}