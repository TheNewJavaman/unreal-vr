#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <string>
#include <iostream>

namespace UnrealVR
{
	// TODO
	namespace Logger
	{
		void Init(bool bNewWindow);
		void Info(std::wstring message);
		void Error(std::wstring message);
	};
}