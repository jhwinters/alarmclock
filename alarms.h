
/*
 *================================================================
 *
 *  Type definitions.
 *
 *================================================================
 */

typedef struct {
  t_LL_Header header;
  int         trigger_time;    /* Seconds since midnight */
  bool        days[7];        /* 0 = Sunday, etc. */
} t_individual_alarm;

/*
 *================================================================
 *
 *  External declarations.
 *
 *================================================================
 */

extern bool add_alarm(t_individual_alarm new_alarm);

extern int identify_alarm_day(yaml_char_t *candidate);

extern int interpret_alarm_time(yaml_char_t *candidate);

extern void dump_alarms(void);
