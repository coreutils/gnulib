#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct savewd;
ptrdiff_t mkancesdirs (char *, struct savewd *,
                       int (*) (char const *, char const *, void *), void *);

#ifdef __cplusplus
}
#endif
