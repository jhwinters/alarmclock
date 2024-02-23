#define NEED_SDL
#include "includes.h"

static SDL_Surface *optimized_menu_icon;

void init_images(SDL_Window *window) {
  int          flags = IMG_INIT_PNG;
  SDL_Surface *raw_menu_icon;
  int          result;
  SDL_Surface *window_surface;

  result = IMG_Init(flags);
  if ((result & flags) == 0) {
    LOG_Error("Failed to initialize image handling.\n");
  } else {
    raw_menu_icon = IMG_Load("menu.png");
    if (raw_menu_icon == NULL) {
      LOG_Error("Failed to load menu icon.\n");
    } else {
      optimized_menu_icon = raw_menu_icon;
    /*
      window_surface = SDL_GetWindowSurface(window);
      optimized_menu_icon =
        SDL_ConvertSurface(raw_menu_icon, window_surface->format, 0);
      SDL_FreeSurface(raw_menu_icon);
    */
    }
  }
}

void paint_menu(SDL_Renderer *renderer) {
  SDL_Texture *menu_icon;
  SDL_Rect     rectangle;
 
  rectangle.x  = 10;
  rectangle.y  = 10;
  rectangle.w  = 60;
  rectangle.h  = 60;
  menu_icon = SDL_CreateTextureFromSurface(renderer, optimized_menu_icon);
  SDL_RenderCopy(renderer, menu_icon, NULL, &rectangle);
  SDL_DestroyTexture(menu_icon);
}
