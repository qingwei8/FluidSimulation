#ifdef FLUIDSIMULATION_EXPORTS
#define FLUIDSIMULATION_DLL_EXPORT __declspec(dllexport)
#else
#define FLUIDSIMULATION_DLL_EXPORT __declspec(dllimport)
#endif