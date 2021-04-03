/* Return the NIS domain name, without size limitations.  */
#include "xgetdomainname.h"
#define GETANAME getdomainname
#define XGETANAME xgetdomainname
#include "xgethostname.c"
