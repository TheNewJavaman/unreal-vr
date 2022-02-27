#include <string>
#include "Launcher.h"

int main(int argc, char* argv[])
{
	std::wstring gamePath = L"C:\\Program Files (x86)\\GOG Galaxy\\Games\\Ghostrunner\\Ghostrunner\\Binaries\\Win64\\Ghostrunner-Win64-Shipping.exe";
	if (!UnrealVR::Launcher::StartGame(gamePath))
	{
		std::cout << "Failed to start game\n";
		return 1;
	}
	if (!UnrealVR::Launcher::InjectDLL(std::string(argv[0])))
	{
		std::cout << "Failed to inject DLL\n";
		return 1;
	}
	return 0;
}