#pragma once

#ifdef UNREALVRLOADER_EXPORTS
#define UNREALVR_API __declspec(dllexport)
#else
#define UNREALVR_API __declspec(dllimport)
#endif