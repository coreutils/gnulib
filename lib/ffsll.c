#define FUNC ffsll
#define TYPE long long int
#define GCC_BUILTIN __builtin_ffsll
#ifdef _WIN64
# define MSVC_BUILTIN _BitScanForward64
#endif
#include "ffsl.h"
