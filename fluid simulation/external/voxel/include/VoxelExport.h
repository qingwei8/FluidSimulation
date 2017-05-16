#ifdef VOXEL_EXPORTS
#define VOXEL_DLL_EXPORTS __declspec(dllexport)
#else
#define  VOXEL_DLL_EXPORTS __declspec(dllimport)
#endif