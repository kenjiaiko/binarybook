#ifdef SAMPLEDLL_EXPORTS
#define SAMPLEDLL_API __declspec(dllexport)
#else
#define SAMPLEDLL_API __declspec(dllimport)
#endif
