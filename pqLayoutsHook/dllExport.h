#pragma once

#ifdef PQLAYOUTSHOOK_EXPORTS
#define PQHOOK_API __declspec(dllexport)
#else
#define PQHOOK_API __declspec(dllimport)
#endif
