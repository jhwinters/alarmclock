
#define NUM_FONTS 3

/*
 *================================================================
 *
 *  Type definitions
 *
 *================================================================
 */

typedef enum {
  f_large,
  f_medium,
  f_small
} t_font_size;

typedef enum {
  h_left,
  h_right,
  h_centre
} t_href;

typedef enum {
  v_top,
  v_middle,
  v_bottom
} t_vref;
  
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

#if defined NEED_SDL
extern void paint_text(
    SDL_Renderer *renderer,
    const char  *text,
    t_font_size  font,
    t_href       href,
    t_vref       vref,
    int          hoff,
    int          voff,
    int          density);

#endif
