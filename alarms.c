/*
 *  Module to handle individual alarms.
 */

#include "includes.h"

/*
 *================================================================
 *
 *  Local data.
 *
 *================================================================
 */

static t_LL_Header anchor = LL_EMPTY;

static const char *known_days[] = {
  "Sunday",
  "Monday",
  "Tuesday",
  "Wednesday",
  "Thursday",
  "Friday",
  "Saturday"
};

/*
 *================================================================
 *
 *  Forward declarations.
 *
 *================================================================
 */

static void dump_alarm(t_individual_alarm *alarm);

/*
 *================================================================
 *
 *  Externally visible routines.
 *
 *================================================================
 */

bool add_alarm(t_individual_alarm new_alarm) {

  t_individual_alarm *alarm;
  int                 i;
  bool                result = FALSE;

  /*
   *  No validation as yet.
   */
  alarm = LL_Malloc(sizeof(t_individual_alarm));
  if (alarm) {
    alarm->trigger_time = new_alarm.trigger_time;
    for (i = 0; i < 7; i++) {
      alarm->days[i] = new_alarm.days[i];
    }
    LL_AddToTail(&anchor, &alarm->header);
    LOG_Debug("Added alarm.\n");
    result = TRUE;
  } else {
    LOG_Error("Failed to allocate memory for alarm.\n");
  }
  return result;
}


int identify_alarm_day(yaml_char_t *candidate) {
  int   i;
  char *ptr;
  int   result = -1;

  ptr = (char *) candidate;
  for (i = 0; i < 7; i++) {
    if (strcmp(ptr, known_days[i]) == 0) {
      result = i;
      break;
    }
  }
  return result;
}


int interpret_alarm_time(yaml_char_t *candidate) {
  char   *ptr;
  int     result = -1;
  struct  tm tm;

  /*
   * Might be a simple integer or a time in the form HH:MM[:SS]
   */
  ptr = (char *) candidate;
  if (strchr(ptr, ':') == NULL) {
    LOG_Debug("A number of seconds from midnight.\n");
    result = (int) strtol(ptr, NULL, 10);
  } else {
    LOG_Debug("A formatted time.\n");
    tm.tm_hour = 0;
    tm.tm_min  = 0;
    tm.tm_sec  = 0;
    strptime(ptr, "%H:%M:%S", &tm);
    LOG_Debug("Hours %d, minutes %d, seconds %d\n",
              tm.tm_hour, tm.tm_min, tm.tm_sec);
    result = (((tm.tm_hour * 60) + tm.tm_min) * 60) + tm.tm_sec;
  }
  return result;
}

void dump_alarms(void) {
  /*
   * List all known alarms for debug purposes.
   */
  t_individual_alarm *current;

  current = LL_FirstItem(&anchor);
  while (current != NULL) {
    dump_alarm(current);
    current = LL_NextItem(&anchor, &current->header);
  }
}

/*
 *================================================================
 *
 *  Local routines.
 *
 *================================================================
 */

static void dump_alarm(t_individual_alarm *alarm) {
  int i;

  LOG_Debug("Alarm at %d\n", alarm->trigger_time);
  for (i = 0; i < 7; i++) {
    if (alarm->days[i]) {
      LOG_Debug("  %s\n", known_days[i]);
    }
  }
}
