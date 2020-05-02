#ifndef _GL_DI_SET_H
# define _GL_DI_SET_H

# include <sys/types.h>

struct di_set *di_set_alloc (void);
int di_set_insert (struct di_set *, dev_t, ino_t) _GL_ATTRIBUTE_NONNULL ((1));
void di_set_free (struct di_set *) _GL_ATTRIBUTE_NONNULL ((1));
int di_set_lookup (struct di_set *dis, dev_t dev, ino_t ino)
  _GL_ATTRIBUTE_NONNULL ((1));

#endif
