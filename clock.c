/*
 * Alarm clock re-written in C
 */

#define NEED_SDL
#include "includes.h"

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
  init_images(window);
  renderer = SDL_CreateRenderer(window, -1, 0);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderFillRect(renderer, NULL);
  paint_text(renderer, "Hello!", f_large, h_centre, v_middle, 0, 0, 200);
  paint_menu(renderer);
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

