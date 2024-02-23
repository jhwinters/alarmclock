
#define NEED_SDL
#include "includes.h"

#define MAX_FILENAME_LEN 256
#define NUM_FONTS 3

/*
 *================================================================
 *
 *  Type definitions.
 *
 *================================================================
 */

typedef struct {
  char file_name[MAX_FILENAME_LEN + 1];
  int  size;
} t_font_record;

/*
 *================================================================
 *
 *  Local data.
 *
 *================================================================
 */

static t_font_record fonts[NUM_FONTS] = {
  {"/usr/share/fonts/truetype/freefont/FreeSerifBoldItalic.ttf", 240},
  {"/usr/share/fonts/truetype/freefont/FreeSerif.ttf",            50},
  {"/usr/share/fonts/truetype/freefont/FreeSans.ttf",             32}
};

static TTF_Font *font_handles[NUM_FONTS];

/*
 *================================================================
 *
 *  Forward declarations.
 *
 *================================================================
 */

static SDL_Surface *render_font(
    t_font_size  which_font,
    const char  *text,
    SDL_Color    colour);

/*
 *================================================================
 *
 *  Externally visible routes.
 *
 *================================================================
 */

void init_fonts(void) {
  int i;

  for (i = 0; i < NUM_FONTS; i++) {
    font_handles[i] = TTF_OpenFont(fonts[i].file_name, fonts[i].size);
    if (font_handles[i] == NULL) {
      LOG_Error("Failed to open font \"%s\".\n", fonts[i].file_name);
    }
  }
}

void set_font_file_name(
  t_font_size        which_font,
  const yaml_char_t *file_name) {

  t_font_record *target;

  if ((which_font == f_large) ||
      (which_font == f_medium) ||
      (which_font == f_small)) {
    target = fonts + which_font;
    safe_copy(target->file_name,
              (const char *) file_name,
              MAX_FILENAME_LEN,
              "Font file name");
  }
}

void set_font_size(
  t_font_size        which_font,
  const yaml_char_t *size_str) {

  t_font_record *target;

  if ((which_font == f_large) ||
      (which_font == f_medium) ||
      (which_font == f_small)) {
    target = fonts + which_font;
    target->size = integer((const char *) size_str);
  }
}

t_box size_text(
    t_font_size  which_font,
    const char  *text) {

  t_box result = {0, 0};

  TTF_SizeText(
    font_handles[which_font],
    text,
    &result.width,
    &result.height);
  return result;
}


void paint_text(
    SDL_Renderer *renderer,
    const char     *text,
    t_font_size     font,
    t_href          href,
    t_vref          vref,
    int             hoff,
    int             voff,
    int             density) {

  t_box        box;
  SDL_Color    colour;
  int          hpos;
  int          vpos;
  SDL_Rect     rectangle;
  SDL_Surface *surface;
  SDL_Texture *texture;

  box = size_text(f_large, text);
  switch (href) {
    case h_left:
      hpos = hoff;
      break;

    case h_right:
      hpos = (1024 - box.width) - hoff;
      break;

    case h_centre:
      hpos = (1024 - box.width) / 2 + hoff;
      break;
  }
  switch (vref) {
    case v_top:
      vpos = voff;
      break;

    case v_bottom:
      vpos = (600 - box.height) - voff;
      break;

    case v_middle:
      vpos = (600 - box.height) / 2 + voff;
      break;

  }
  colour.r = density;
  colour.g = density;
  colour.b = density;
  colour.a = 255;
  surface = render_font(font, text, colour);
  texture = SDL_CreateTextureFromSurface(
      renderer,
      surface);
  rectangle.x  = hpos;
  rectangle.y  = vpos;
  rectangle.w  = box.width;
  rectangle.h  = box.height;
  SDL_RenderCopy(renderer, texture, NULL, &rectangle);
  SDL_DestroyTexture(texture);
  SDL_FreeSurface(surface);
}


void dump_fonts(void) {
  LOG_Debug("Large font\n");
  LOG_Debug("  %3d %s\n", fonts[f_large].size, fonts[f_large].file_name);
  LOG_Debug("Medium font\n");
  LOG_Debug("  %3d %s\n", fonts[f_medium].size, fonts[f_medium].file_name);
  LOG_Debug("Small font\n");
  LOG_Debug("  %3d %s\n", fonts[f_small].size, fonts[f_small].file_name);

}

/*
 *================================================================
 *
 *  Local routines.
 *
 *================================================================
 */

static SDL_Surface *render_font(
    t_font_size  which_font,
    const char  *text,
    SDL_Color    colour) {

  return TTF_RenderText_Solid(font_handles[which_font],
                              text,
                              colour);
}

