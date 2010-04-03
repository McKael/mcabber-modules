/*
   Copyright 2010 Mikael Berthe

  Module "extsayng"   -- adds a /extsay command
                         Spawns an external editor, using screen

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

#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <glib/gstdio.h>

#include <mcabber/modules.h>
#include <mcabber/commands.h>
#include <mcabber/settings.h>
#include <mcabber/compl.h>
#include <mcabber/logprint.h>

static void extsayng_init(void);
static void extsayng_uninit(void);

/* Module description */
module_info_t info_extsayng = {
        .branch         = MCABBER_BRANCH,
        .api            = MCABBER_API_VERSION,
        .version        = "0.01",
        .description    = "Use external editor to send a message",
        .requires       = NULL,
        .init           = extsayng_init,
        .uninit         = extsayng_uninit,
        .next           = NULL,
};


// Run the external helper script with parameters
static void screen_run_script(const gchar *args)
{
  GError *err = NULL;
  gchar *argv[] = { "screen", "-r", "-X", "screen", NULL,
                    NULL, NULL, NULL, NULL };
  gboolean ret;
  gchar strwinheight[32];
  gboolean winsplit = settings_opt_get_int("extsay_split_win");

  // screen -r -X screen $path/extsay.sh [jid [winsplit [height]]]
  argv[4] = (gchar*)settings_opt_get("extsay_script_path");

  // Helper script path
  if (!argv[4] || !argv[4][0]) {
    scr_log_print(LPRINT_NORMAL, "Please set option 'extsay_script_path'.");
    return;
  }

  // Helper script parameter #1
  if (args && *args)
    argv[5] = (gchar*)args;
  else
    argv[5] = ".";

  // Update environment variables for the helper script
  if (winsplit) {
    gint winheight = settings_opt_get_int("extsay_win_height");
    argv[6] = "winsplit";       // Helper script parameter #2
    if (winheight > 0 && winheight < 256) {
      snprintf(strwinheight, sizeof strwinheight, "%d", winheight);
      argv[7] = strwinheight;   // Helper script parameter #3
    }
  }

  ret = g_spawn_async(NULL, argv, NULL,
                      G_SPAWN_SEARCH_PATH |
                        G_SPAWN_STDOUT_TO_DEV_NULL|G_SPAWN_STDERR_TO_DEV_NULL,
                      NULL, NULL, NULL, &err);

  if (!ret)
    scr_LogPrint(LPRINT_NORMAL, err->message);
}

static void do_extsayng(gchar *args)
{
  gpointer bud;

  if (!args || !*args || !g_strcmp0(args, ".")) {
    if (!current_buddy) {
      scr_LogPrint(LPRINT_NORMAL, "Please select a buddy.");
      return;
    }

    bud = BUDDATA(current_buddy);
    if (!(buddy_gettype(bud) &
          (ROSTER_TYPE_USER|ROSTER_TYPE_AGENT|ROSTER_TYPE_ROOM))) {
      scr_LogPrint(LPRINT_NORMAL, "This is not a user.");
      return;
    }

    args = (gchar*)buddy_getjid(bud);
  }

  screen_run_script(args);
}

static void extsayng_init(void)
{
  cmd_add("extsay", "Use external editor to write a message",
          COMPL_JID, 0, do_extsayng, NULL);
}

static void extsayng_uninit(void)
{
  cmd_del("extsay");
}

/* vim: set expandtab cindent cinoptions=>2\:2(0:  For Vim users... */
