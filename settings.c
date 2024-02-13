
#include "includes.h"

/*
 *================================================================
 *
 *  Constants
 *
 *================================================================
 */

#define MAX_STRING_LENGTH 128

/*
 *================================================================
 *
 *  Type definitions
 *
 *================================================================
 */

typedef enum {
  initial,
  started,
  outer_mapping,
  had_settings,
  in_settings,
  had_setting_item,
  had_fonts,
  in_fonts,
  had_font_size,
  in_font,
  had_font_item,
  had_alarms,
  in_alarms,
  in_alarm,
  had_alarm_time,
  had_alarm_days,
  in_alarm_days,
  finished
} t_parsing_state;

typedef enum {
  k_settings,
  k_title,
  k_screen_width,
  k_screen_height,
  k_alarm_sound_file,
  k_dim_delay,
  k_bright,
  k_dim,
  k_fonts,
  k_large,
  k_medium,
  k_small,
  k_file,
  k_size,
  k_alarms,
  k_time,
  k_days,
  k_unknown
} t_known_keyword;

/*
 *================================================================
 *
 *  Local data.
 *
 *================================================================
 */

static char title[MAX_STRING_LENGTH + 1] = "<Unset>";
static char sound_file_name[MAX_STRING_LENGTH + 1] = "<Unset>";
static int screen_width = -1;
static int screen_height = -1;
static int dim_delay = -1;
static int bright_value = -1;
static int dim_value = -1;

/*
 *================================================================
 *
 *  Forward declarations
 *
 *================================================================
 */

static const char *state_text(t_parsing_state state);

static t_known_keyword identify_keyword(yaml_char_t *candidate);

static bool setting_keyword(t_known_keyword keyword);

static bool a_font_size(t_known_keyword keyword);

static bool a_font_setting(t_known_keyword keyword);

static bool store_away(
    t_known_keyword    keyword,
    const yaml_char_t *value);

static bool save_font_detail(
    t_known_keyword font_size,
    t_known_keyword attribute,
    const yaml_char_t *value);

static void safe_copy(
    char       *dest,
    const char *src,
    const char *setting);

static int integer(const char *string);

/*
 *================================================================
 *
 *  Externally visible functions.
 *
 *================================================================
 */

bool parse_config(void) {
  t_individual_alarm    building_alarm;
  FILE                 *config_file;
  bool                  done = FALSE;
  yaml_event_t          event;
  t_known_keyword       font_size = k_unknown;
  bool                  handled;
  int                   i;
  int                   index;
  t_known_keyword       keyword = k_unknown;
  yaml_parser_t         parser;
  t_parsing_state       parsing_state = initial;
  bool                  result = FALSE;

  config_file = fopen("config.yaml", "r");
  if (config_file == NULL) {
    LOG_Error("Failed to open configuration file.\n");
  } else {
    yaml_parser_initialize(&parser);
    yaml_parser_set_input_file(&parser, config_file);
    while (!done) {
      if (yaml_parser_parse(&parser, &event)) {
        handled = FALSE;
        /*
        LOG_Debug("Current state is \"%s\".\n", state_text(parsing_state));
        */
        switch (parsing_state) {
          case initial:
            switch (event.type) {
              case YAML_STREAM_START_EVENT:
                /* Ignore */
                handled = TRUE;
                break;

              case YAML_DOCUMENT_START_EVENT:
                parsing_state = started;
                handled = TRUE;
                break;

              default:
                break;

            }
            break;

          case started:
            switch (event.type) {
              case YAML_MAPPING_START_EVENT:
                parsing_state = outer_mapping;
                handled = TRUE;

              default:
                break;

            }
            break;

          case outer_mapping:
            switch (event.type) {
              case YAML_SCALAR_EVENT:
                keyword = identify_keyword(event.data.scalar.value);
                if (keyword == k_settings) {
                  parsing_state = had_settings;
                  handled = TRUE;
                } else if (keyword == k_alarms) {
                  parsing_state = had_alarms;
                  handled = TRUE;
                }
                break;

              case YAML_MAPPING_END_EVENT:
                parsing_state = finished;
                handled = TRUE;
                break;

              default:
                break;

            }
            break;

          case had_settings:
            switch (event.type) {
              case YAML_MAPPING_START_EVENT:
                parsing_state = in_settings;
                handled = TRUE;
                break;

              default:
                break;

            }
            break;

          case in_settings:
            switch (event.type) {
              case YAML_SCALAR_EVENT:
                keyword = identify_keyword(event.data.scalar.value);
                if (setting_keyword(keyword)) {
                  parsing_state = had_setting_item;
                  handled = TRUE;
                } else if (keyword == k_fonts) {
                  parsing_state = had_fonts;
                  handled = TRUE;
                }
                break;

              case YAML_MAPPING_END_EVENT:
                parsing_state = outer_mapping;
                handled = TRUE;
                break;

              default:
                break;

            }
            break;

          case had_setting_item:
            switch (event.type) {
              case YAML_SCALAR_EVENT:
                if (store_away(keyword, event.data.scalar.value)) {
                  parsing_state = in_settings;
                  handled = TRUE;
                }
                break;

              default:
                break;

            }
            break;

          case had_fonts:
            switch (event.type) {
              case YAML_MAPPING_START_EVENT:
                parsing_state = in_fonts;
                handled = TRUE;
                break;

              default:
                break;

            }
            break;

          case in_fonts:
            switch (event.type) {
              case YAML_SCALAR_EVENT:
                keyword = identify_keyword(event.data.scalar.value);
                if (a_font_size(keyword)) {
                  font_size = keyword;
                  parsing_state = had_font_size;
                  handled = TRUE;
                }
                break;

              case YAML_MAPPING_END_EVENT:
                parsing_state = in_settings;
                handled = TRUE;
                break;

              default:
                break;

            }
            break;

          case had_font_size:
            switch (event.type) {
              case YAML_MAPPING_START_EVENT:
                parsing_state = in_font;
                handled = TRUE;
                break;

              default:
                break;

            }
            break;

          case in_font:
            switch (event.type) {
              case YAML_SCALAR_EVENT:
                keyword = identify_keyword(event.data.scalar.value);
                if (a_font_setting(keyword)) {
                  parsing_state = had_font_item;
                  handled = TRUE;
                }
                break;

              case YAML_MAPPING_END_EVENT:
                parsing_state = in_fonts;
                handled = TRUE;
                break;

              default:
                break;

            }
            break;

          case had_font_item:
            switch (event.type) {
              case YAML_SCALAR_EVENT:
                if (save_font_detail(font_size,
                                     keyword,
                                     event.data.scalar.value)) {
                  parsing_state = in_font;
                  handled = TRUE;
                }
                break;

              default:
                break;

            }
            break;

          case had_alarms:
            switch (event.type) {
              case YAML_SEQUENCE_START_EVENT:
                parsing_state = in_alarms;
                handled = TRUE;
                break;

              default:
                break;

            }
            break;

          case in_alarms:
            switch (event.type) {
              case YAML_MAPPING_START_EVENT:
                /*
                 * Zero the alarm parameters.
                 */
                building_alarm.trigger_time = -1;  /* Invalid */
                for (i = 0; i < 7; i++) {
                  building_alarm.days[i] = TRUE;   /* Default to all days */
                }
                parsing_state = in_alarm;
                handled = TRUE;
                break;

              case YAML_SEQUENCE_END_EVENT:
                parsing_state = outer_mapping;
                handled = TRUE;
                break;

              default:
                break;

            }
            break;

          case in_alarm:
            switch (event.type) {
              case YAML_SCALAR_EVENT:
                keyword = identify_keyword(event.data.scalar.value);
                if (keyword == k_time) {
                  parsing_state = had_alarm_time;
                  handled = TRUE;
                } else if (keyword == k_days) {
                  /*
                   * Hitting this keyword makes them all default to off.
                   */
                  for (i = 0; i < 7; i++) {
                    building_alarm.days[i] = FALSE;
                  }
                  parsing_state = had_alarm_days;
                  handled = TRUE;
                }
                break;

              case YAML_MAPPING_END_EVENT:
                if (add_alarm(building_alarm)) {
                  building_alarm.trigger_time = -1;  /* Invalid */
                  for (i = 0; i < 7; i++) {
                    building_alarm.days[i] = TRUE;   /* Default to all days */
                  }
                  parsing_state = in_alarms;
                  handled = TRUE;
                }
                break;

              default:
                break;

            }
            break;

          case had_alarm_time:
            switch (event.type) {
              case YAML_SCALAR_EVENT:
                building_alarm.trigger_time =
                  interpret_alarm_time(event.data.scalar.value);
                if (building_alarm.trigger_time != -1) {
                  parsing_state = in_alarm;
                  handled = TRUE;
                }
                break;

              default:
                break;

            }
            break;

          case had_alarm_days:
            switch (event.type) {
              case YAML_SEQUENCE_START_EVENT:
                parsing_state = in_alarm_days;
                handled = TRUE;
                break;

              default:
                break;

            }
            break;

          case in_alarm_days:
            switch (event.type) {
              case YAML_SCALAR_EVENT:
                index = identify_alarm_day(event.data.scalar.value);
                if (index != -1) {
                  building_alarm.days[index] = TRUE;
                  handled = TRUE;
                }
                break;

              case YAML_SEQUENCE_END_EVENT:
                parsing_state = in_alarm;
                handled = TRUE;
                break;

              default:
                break;

            }
            break;

          case finished:
            switch (event.type) {
              case YAML_DOCUMENT_END_EVENT:
                handled = TRUE;
                break;

              case YAML_STREAM_END_EVENT:
                handled = TRUE;
                done = TRUE;
                break;

              default:
                break;

            }
            break;

        }
        if (!handled) {
          /*
           * Log anything which I haven't explicitly processed.
           */
          switch (event.type) {

            case YAML_STREAM_START_EVENT:
              LOG_Debug("Stream start.\n");
              break;

            case YAML_STREAM_END_EVENT:
              LOG_Debug("Stream end.\n");
              done = TRUE;
              break;

            case YAML_DOCUMENT_START_EVENT:
              LOG_Debug("Document start.\n");
              break;

            case YAML_DOCUMENT_END_EVENT:
              LOG_Debug("Document end.\n");
              break;

            case YAML_ALIAS_EVENT:
              LOG_Debug("Alias event.\n");
              break;

            case YAML_SCALAR_EVENT:
              LOG_Debug("Scalar event.\n");
  /*            LOG_Debug("Anchor - %s\n", event.data.scalar.anchor);        */
  /*            LOG_Debug("Tag    - %s\n", event.data.scalar.tag);           */
              LOG_Debug("Value  - %s\n", event.data.scalar.value);
              break;

            case YAML_SEQUENCE_START_EVENT:
              LOG_Debug("Sequence start event.\n");
              break;

            case YAML_SEQUENCE_END_EVENT:
              LOG_Debug("Sequence end event.\n");
              break;

            case YAML_MAPPING_START_EVENT:
              LOG_Debug("Mapping start event.\n");
              LOG_Debug("Anchor - %s\n", event.data.mapping_start.anchor);
              LOG_Debug("Tag    - %s\n", event.data.mapping_start.tag);
              break;

            case YAML_MAPPING_END_EVENT:
              LOG_Debug("Mapping end event.\n");
              break;

            default:
              LOG_Debug("Got event %d\n", event.type);
              break;

          }
          LOG_Debug("Final state is \"%s\".\n", state_text(parsing_state));
          exit(EXIT_FAILURE);
        }       /* !handled */
      } else {
        break;
      }
      if (done) {
        result = TRUE;
      }
    }
    yaml_parser_delete(&parser);
  }
  return result;
}

void dump_settings(void) {
  /*
   *  Print out all the settings for debug purposes.
   */
  LOG_Debug("Title - \"%s\"\n", title);
  LOG_Debug("Sound file name - \"%s\"\n", sound_file_name);
  LOG_Debug("Screen width - %d\n", screen_width);
  LOG_Debug("Screen height - %d\n", screen_height);
  LOG_Debug("Dim delay - %d\n", dim_delay);
  LOG_Debug("Bright value - %d\n", bright_value);
  LOG_Debug("Dim value - %d\n", dim_value);

  dump_alarms();
}

/*
 *================================================================
 *
 *  Local functions.
 *
 *================================================================
 */

static const char *state_text(t_parsing_state state) {
  const char *texts[] = {
    "initial",
    "started",
    "outer_mapping",
    "had_settings",
    "in_settings",
    "had_setting_item",
    "had_fonts",
    "in_fonts",
    "had_font_size",
    "in_font",
    "had_font_item",
    "had_alarms",
    "in_alarms",
    "in_alarm",
    "had_alarm_time",
    "had_alarm_days",
    "in_alarm_days",
    "finished"
  };

  return texts[state];
}


static t_known_keyword identify_keyword(yaml_char_t *candidate) {
  const char *texts[] = {
    ":settings",
    ":title",
    ":screen_width",
    ":screen_height",
    ":alarm_sound_file",
    ":dim_delay",
    ":bright",
    ":dim",
    ":fonts",
    ":large",
    ":medium",
    ":small",
    ":file",
    ":size",
    ":alarms",
    ":time",
    ":days"
  };

  t_known_keyword index = k_settings;   /* The first one */

  while (index < k_unknown) {           /* The last one */
    /*
     * Annoyingly the YAML library uses an unnecessary type for the string
     * which it returns.
     */
    if (strcmp((char *) candidate, texts[index]) == 0) {
      break;
    } else {
      index++;
    }
  }
  return index;
}


static bool setting_keyword(t_known_keyword keyword) {
  /*
   * Is this keyword one for an individual setting?
   */
  return (keyword == k_title) ||
         (keyword == k_screen_width) ||
         (keyword == k_screen_height) ||
         (keyword == k_alarm_sound_file) ||
         (keyword == k_dim_delay) ||
         (keyword == k_bright) ||
         (keyword == k_dim);
}


static bool a_font_size(t_known_keyword keyword) {
  /*
   * Is this keyword one for a font size.
   */
  return (keyword == k_large) ||
         (keyword == k_medium) ||
         (keyword == k_small);
}


static bool a_font_setting(t_known_keyword keyword) {
  /*
   * Is this keyword one for a font setting.
   */
  return (keyword == k_file) ||
         (keyword == k_size);
}


static bool store_away(
    t_known_keyword    keyword,
    const yaml_char_t *value) {

  int   result = TRUE;
  char *ptr;

  ptr = (char *) value;
  switch (keyword) {
    case k_title:
      safe_copy(title, ptr, "Title");
      break;

    case k_screen_width:
      screen_width = integer(ptr);
      break;

    case k_screen_height:
      screen_height = integer(ptr);
      break;

    case k_alarm_sound_file:
      safe_copy(sound_file_name, ptr, "Sound file name");
      break;

    case k_dim_delay:
      dim_delay = integer(ptr);
      break;

    case k_bright:
      bright_value = integer(ptr);
      break;

    case k_dim:
      dim_value = integer(ptr);
      break;


    default:
      result = FALSE;
      break;

  }
  return result;
}


static bool save_font_detail(
    t_known_keyword font_size,
    t_known_keyword attribute,
    const yaml_char_t *value) {
  return TRUE;
}


static void safe_copy(
    char       *dest,
    const char *src,
    const char *setting) {

  if (strlen(src) > MAX_STRING_LENGTH) {
    LOG_Warning("Setting \"%s\" truncated.\n", setting);
    strncpy(dest, src, MAX_STRING_LENGTH);
    dest[MAX_STRING_LENGTH] = '\0';
  } else {
    strcpy(dest, src);
  }
}


static int integer(const char *string) {
  return (int) strtol(string, NULL, 10);
}


