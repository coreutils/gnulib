#include <config.h>
#define _GL_STDBIT_INLINE _GL_EXTERN_INLINE
#define COUNT_LEADING_ZEROS_INLINE _GL_EXTERN_INLINE
#define COUNT_TRAILING_ZEROS_INLINE _GL_EXTERN_INLINE
#define COUNT_ONE_BITS_INLINE _GL_EXTERN_INLINE
#include <stdbit.h>
#if 1500 <= _MSC_VER && (defined _M_IX86 || defined _M_X64)
int popcount_support = -1;
#endif
