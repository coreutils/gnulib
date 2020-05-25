#ifndef _SYS_RANDOM_H
#define _SYS_RANDOM_H 1

#include <sys/types.h>
#define GRND_NONBLOCK 1
#define GRND_RANDOM 2
ssize_t getrandom (void *, size_t, unsigned int);

#endif
