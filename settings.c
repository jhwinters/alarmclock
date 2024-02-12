
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <yaml.h>
#include "global.h"
#include "settings.h"

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
  had_alarm_setting
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
 *  Forward declarations
 *
 *================================================================
 */

static const char *state_text(t_parsing_state state);

static t_known_keyword identify_keyword(yaml_char_t *candidate);

static bool setting_keyword(t_known_keyword keyword);

static bool a_font_size(t_known_keyword keyword);

static bool a_font_setting(t_known_keyword keyword);

static bool an_alarm_setting(t_known_keyword keyword);

static bool store_away(
    t_known_keyword    keyword,
    const yaml_char_t *value);

static bool save_font_detail(
    t_known_keyword font_size,
    t_known_keyword attribute,
    const yaml_char_t *value);

/*
 *================================================================
 *
 *  Externally visible functions.
 *
 *================================================================
 */

bool parse_config(void) {
  FILE                 *config_file;
  bool                  done = FALSE;
  yaml_event_t          event;
  t_known_keyword       font_size = k_unknown;
  bool                  handled;
  t_known_keyword       keyword = k_unknown;
  yaml_parser_t         parser;
  t_parsing_state       parsing_state = initial;
  bool                  result = FALSE;

  config_file = fopen("config.yaml", "r");
  if (config_file == NULL) {
    fprintf(stderr, "Failed to open configuration file.\n");
  } else {
    yaml_parser_initialize(&parser);
    yaml_parser_set_input_file(&parser, config_file);
    while (!done) {
      if (yaml_parser_parse(&parser, &event)) {
        handled = FALSE;
        printf("Current state is \"%s\".\n", state_text(parsing_state));
        switch (parsing_state) {
          case initial:
            switch (event.type) {
              case YAML_STREAM_START_EVENT:
                /* Ignore */
                handled = TRUE;
                break;

              case YAML_DOCUMENT_START_EVENT:
                printf("Started.\n");
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
                 * Should zero the alarm parameters here.
                 */
                parsing_state = in_alarm;
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
                if (an_alarm_setting(keyword)) {
                  parsing_state = had_alarm_setting;
                  handled = TRUE;
                }
                break;

              default:
                break;

            }
            break;

          default:
            break;

        }
        if (!handled) {
          /*
           * Log anything which I haven't explicitly processed.
           */
          switch (event.type) {

            case YAML_STREAM_START_EVENT:
              printf("Stream start.\n");
              break;

            case YAML_STREAM_END_EVENT:
              printf("Stream end.\n");
              done = TRUE;
              break;

            case YAML_DOCUMENT_START_EVENT:
              printf("Document start.\n");
              break;

            case YAML_DOCUMENT_END_EVENT:
              printf("Document end.\n");
              break;

            case YAML_ALIAS_EVENT:
              printf("Alias event.\n");
              break;

            case YAML_SCALAR_EVENT:
              printf("Scalar event.\n");
  /*            printf("Anchor - %s\n", event.data.scalar.anchor);        */
  /*            printf("Tag    - %s\n", event.data.scalar.tag);           */
              printf("Value  - %s\n", event.data.scalar.value);
              break;

            case YAML_SEQUENCE_START_EVENT:
              printf("Sequence start event.\n");
              break;

            case YAML_SEQUENCE_END_EVENT:
              printf("Sequence end event.\n");
              break;

            case YAML_MAPPING_START_EVENT:
              printf("Mapping start event.\n");
              printf("Anchor - %s\n", event.data.mapping_start.anchor);
              printf("Tag    - %s\n", event.data.mapping_start.tag);
              break;

            case YAML_MAPPING_END_EVENT:
              printf("Mapping end event.\n");
              break;

            default:
              printf("Got event %d\n", event.type);
              break;

          }
          printf("Final state is \"%s\".\n", state_text(parsing_state));
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
    "had_alarm_setting"
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
  return (keyword != k_settings) &&
         (keyword != k_fonts) &&
         (keyword != k_unknown);
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


static bool an_alarm_setting(t_known_keyword keyword) {
  return (keyword == k_time) ||
         (keyword == k_days);
}

static bool store_away(
    t_known_keyword    keyword,
    const yaml_char_t *value) {

  return TRUE;
}


static bool save_font_detail(
    t_known_keyword font_size,
    t_known_keyword attribute,
    const yaml_char_t *value) {
  return TRUE;
}




