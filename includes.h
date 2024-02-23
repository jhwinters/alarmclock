#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define __USE_XOPEN
#include <time.h>
#include <assert.h>
#include <unistd.h>
#include <yaml.h>
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>
#if defined NEED_SDL
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#endif
#include "global.h"
#include "logging.h"
#include "linklist.h"
#include "utils.h"
#include "alarms.h"
#include "fonts.h"
#include "image.h"
#include "settings.h"

