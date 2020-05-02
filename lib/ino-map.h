#ifndef _GL_INO_MAP_H
# define _GL_INO_MAP_H

# include <sys/types.h>

# define INO_MAP_INSERT_FAILURE ((size_t) -1)

struct ino_map *ino_map_alloc (size_t);
void ino_map_free (struct ino_map *) _GL_ATTRIBUTE_NONNULL ((1));
size_t ino_map_insert (struct ino_map *, ino_t) _GL_ATTRIBUTE_NONNULL ((1));

#endif
