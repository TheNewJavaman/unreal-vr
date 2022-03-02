#include <string>
#include "UnrealVRLauncher.h"

int main(int argc, char* argv[])
{
	std::wstring gamePath = L"C:\\Program Files (x86)\\GOG Galaxy\\Games\\Ghostrunner\\Ghostrunner\\Binaries\\Win64\\Ghostrunner-Win64-Shipping.exe";
	if (!UnrealVR::Launcher::StartGame(gamePath))
	{
		std::cout << "Failed to start game\n";
		return 1;
	}
	if (!UnrealVR::Launcher::InjectDLL(std::string(argv[0]), "UnrealEngineModLoader.dll"))
	{
		std::cout << "Failed to inject UnrealEngineModLoader DLL\n";
		return 1;
	}
	if (!UnrealVR::Launcher::InjectDLL(std::string(argv[0]), "UnrealVRLoader.dll"))
	{
		std::cout << "Failed to inject UnrealVR DLL\n";
		return 1;
	}
	if (!UnrealVR::Launcher::CloseHandles())
	{
		std::cout << "Failed to close handles\n";
		return 1;
	}
	return 0;
}