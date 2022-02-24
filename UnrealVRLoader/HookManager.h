#pragma once
#pragma once

#include <string>
#include "MinHook.h"
#include "Logger.h"

namespace VUE
{
	class HookManager
	{
	public:
		static bool Init();

		template <typename T>
		static bool Add(DWORD_PTR pTarget, LPVOID pDetour, T** ppOriginal, std::wstring displayName);

		static bool Stop();
	};
}