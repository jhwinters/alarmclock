
#define NUM_FONTS 3

typedef enum {
  f_large,
  f_medium,
  f_small
} t_font_size;

typedef struct {
  int width;
  int height;
} t_box;

/*
 *================================================================
 *
 *  External declarations
 *
 *================================================================
 */

extern void init_fonts(void);

extern void set_font_file_name(
  t_font_size        which_font,
  const yaml_char_t *file_name);

extern void set_font_size(
  t_font_size         which_font,
  const yaml_char_t  *size_str);

extern void dump_fonts(void);

extern t_box size_text(
    t_font_size  which_font,
    const char  *text);

extern SDL_Surface *render_font(
    t_font_size  which_font,
    const char  *text,
    SDL_Color    colour);
