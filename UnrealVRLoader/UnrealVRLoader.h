#pragma once

namespace UnrealVR
{
	class Loader
	{
	public:
		static void Init();

		static void Stop();

	private:
		static bool ResumeGame();
	};
}
