
#include "includes.h"

#define MAX_FILENAME_LEN 256

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

/*
 *================================================================
 *
 *  Externally visible routes.
 *
 *================================================================
 */

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


void dump_fonts(void) {
  LOG_Debug("Large font\n");
  LOG_Debug("  %3d %s\n", fonts[f_large].size, fonts[f_large].file_name);
  LOG_Debug("Medium font\n");
  LOG_Debug("  %3d %s\n", fonts[f_medium].size, fonts[f_medium].file_name);
  LOG_Debug("Small font\n");
  LOG_Debug("  %3d %s\n", fonts[f_small].size, fonts[f_small].file_name);

}
