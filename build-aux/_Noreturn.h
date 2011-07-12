#if ! defined _Noreturn && __STDC_VERSION__ < 201000
# if (3 <= __GNUC__ || __GNUC__ == 2 && 8 <= __GNUC_MINOR__ \
      || 0x5110 <= __SUNPRO_C)
#  define _Noreturn __attribute__ ((__noreturn__))
# else
#  define _Noreturn
# endif
#endif
