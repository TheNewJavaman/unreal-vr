#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <string>
#include <iostream>

namespace UnrealVR
{
	// TODO
	class Logger
	{
	public:
		static void Init(bool bNewWindow);
		static void Info(std::wstring message);
		static void Error(std::wstring message);
	};
}