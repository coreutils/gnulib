#include <time.h>
int fdutimens (char const *, int, struct timespec const [2]);
int gl_futimens (int, char const *, struct timespec const [2]);
int utimens (char const *, struct timespec const [2]);
int lutimens (char const *, struct timespec const [2]);
