#include "UE4Manager.h"

#include <Utilities/Globals.h>

namespace UnrealVR
{
    void UE4Manager::AddEvents()
    {
        Global::GetGlobals()->eventSystem.registerEvent(
            new Event<UE4::AActor*>("BeginPlay", &BeginPlayCallback)
        );
    }

    void UE4Manager::BeginPlayCallback(UE4::AActor* Actor)
    {
        const auto now = std::chrono::system_clock::now();
        const std::chrono::duration<double> elapsed = now - lastCallback;
        if (elapsed.count() > MAX_CALLBACK_SECONDS)
        {
            const auto playerController = UE4::UGameplayStatics::GetPlayerController(0);
            if (playerController != nullptr)
            {
                Log::Info("PlayerController exists");
                if (playerController->DoesObjectContainFunction("GetViewTarget"))
                {
                    Log::Info("Has GetViewTarget");
                }
                else
                {
                    Log::Info("Doesn't have GetViewTarget");
                }
                if (playerController->DoesObjectContainFunction("ClientSetViewTarget"))
                {
                    Log::Info("Has SetViewTarget");
                }
                else
                {
                    Log::Info("Doesn't have SetViewTarget");
                }
            }
            else
            {
                Log::Info("PlayerController doesn't exist");
            }
        }
        lastCallback = now;
    }
}
