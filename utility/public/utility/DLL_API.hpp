#pragma once

#ifdef CompilingUtilityDLL
#define UTILITY_API   __declspec( dllexport ) 
#else 
#define UTILITY_API   __declspec( dllimport ) 
#endif