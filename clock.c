/*
 * Alarm clock re-written in C
 */

#include "includes.h"

/*
 *================================================================
 *
 *  Type definitions
 *
 *================================================================
 */

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
 *  Local variables.
 *
 *================================================================
 */

static SDL_Renderer *renderer;

/*
 *================================================================
 *
 *  Forward declarations
 *
 *================================================================
 */

static void paint_text(
    const char  *text,
    t_font_size  font,
    t_href       href,
    t_vref       vref,
    int          hoff,
    int          voff,
    int          density);

/*
 *================================================================
 *
 *  Entry point.
 *
 *================================================================
 */

int main(void) {
  SDL_Window   *window;

  parse_config();
  dump_settings();
  printf("Hello, World!\n");
  SDL_Init(SDL_INIT_EVERYTHING);
  TTF_Init();
  init_fonts();
  window = SDL_CreateWindow("Banana",
                            SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED,
                            1024,
                            600,
                            SDL_WINDOW_FULLSCREEN);
  renderer = SDL_CreateRenderer(window, -1, 0);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderFillRect(renderer, NULL);
  paint_text("Hello!", f_large, h_centre, v_middle, 0, 0, 200);
  SDL_RenderPresent(renderer);
  SDL_ShowCursor(0);
  sleep(5);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  TTF_Quit();
  SDL_Quit();
  return 0;
}

/*
 *================================================================
 *
 *  Local functions.
 *
 *================================================================
 */

static void paint_text(
    const char  *text,
    t_font_size  font,
    t_href       href,
    t_vref       vref,
    int          hoff,
    int          voff,
    int          density) {

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

