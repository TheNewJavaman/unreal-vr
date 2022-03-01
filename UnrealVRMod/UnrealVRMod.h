#pragma once

// TODO: Include the correct header
typedef unsigned char byte;

#include "Utilities/MinHook.h"
#include "Mod/Mod.h"

namespace UnrealVR
{
	class CoreMod : public Mod
	{
	public:
		CoreMod()
		{
			ModName = "UnrealVR";
			ModVersion = "0.1.0";
			ModDescription = "Adds virtual reality support to flatscreen Unreal Engine games";
			ModAuthors = "TheNewJavaman";
			ModLoaderVersion = "2.2.0";

			ModRef = this;
			CompleteModCreation();
		}

		virtual void InitializeMod() override;
	};
}
