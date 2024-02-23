/*
 *================================================================
 *
 *  Type definitions.
 *
 *================================================================
 */

typedef struct {
  int width;
  int height;
} t_box;

/*
 *================================================================
 *
 *  External declarations.
 *
 *================================================================
 */

extern void safe_copy(
    char       *dest,
    const char *src,
    int         limit,
    const char *setting);

extern int integer(const char *string);


