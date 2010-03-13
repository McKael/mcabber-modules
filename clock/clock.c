/*
   Copyright 2010 Mikael Berthe

  Module "clock"      -- Displays date and time

Options:
- clock_precision_onesec: boolean (default: 0)
  If true, the clock will be updated every second,
  if false, the clock is updated once per minute.
- clock_strfmt: string (default: "%Y-%m-%d %H:%M")
  strftime format string.

This module is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <time.h>

#include <mcabber/modules.h>
#include <mcabber/settings.h>
#include <mcabber/screen.h>

static void clock_init(void);
static void clock_uninit(void);

/* Module description */
module_info_t info_clock = {
        .branch         = MCABBER_BRANCH,
        .api            = MCABBER_API_VERSION,
        .version        = "1.00",
        .description    = "Simple clock module\n"
                          "Uses the 'info' option to display the time.",
        .requires       = NULL,
        .init           = clock_init,
        .uninit         = clock_uninit,
        .next           = NULL,
};

static guint srcno = 0;
static gboolean precision_onesec;
static gchar *backup_info;
static gchar *strfmt;
static const gchar dfltstrfmt[] = "%Y-%m-%d %H:%M"; // Default string format

static gboolean clock_cb(void)
{
  char buf[256];
  time_t now_t;
  struct tm *now;

  time(&now_t);
  now = localtime(&now_t);
  strftime(buf, sizeof(buf), strfmt, now);
  settings_set(SETTINGS_TYPE_OPTION, "info", buf);
  scr_UpdateChatStatus(TRUE);

  if (precision_onesec)
    return TRUE;  // Let's be called again in 1 second

  // Set up new timeout event
  srcno = g_timeout_add_seconds((now->tm_sec < 60 ? 60 - now->tm_sec : 1),
                                (GSourceFunc)clock_cb, NULL);
  return FALSE;   // Destroy the old timeout
}

static void clock_setup_timer(gboolean activate)
{
  if ((activate && srcno) || (!activate && !srcno))
    return;

  if (activate) {
    srcno = g_timeout_add_seconds(1, (GSourceFunc)clock_cb, NULL);
  } else {
    g_source_remove(srcno);
    srcno = 0;
  }
}

//static void do_clock(char *args)
//{
//}

/* Initialization */
static void clock_init(void)
{
  backup_info = g_strdup(settings_opt_get("info"));
  precision_onesec = settings_opt_get_int("clock_precision_onesec");
  strfmt = g_strdup(settings_opt_get("clock_strfmt"));
  if (!strfmt)
    strfmt = g_strdup(dfltstrfmt);
  clock_setup_timer(TRUE);
}

/* Deinitialization */
static void clock_uninit(void)
{
  clock_setup_timer(FALSE);
  settings_set(SETTINGS_TYPE_OPTION, "info", backup_info);
  g_free(backup_info);
  g_free(strfmt);
  backup_info = strfmt = NULL; // probably useless...
  scr_UpdateChatStatus(TRUE);
}

/* vim: set expandtab cindent cinoptions=>2\:2(0:  For Vim users... */
