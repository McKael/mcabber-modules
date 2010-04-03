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
#include <string.h>
#include <sys/wait.h>
#include <glib/gstdio.h>

#include <mcabber/modules.h>
#include <mcabber/commands.h>
#include <mcabber/settings.h>
#include <mcabber/compl.h>
#include <mcabber/utils.h>
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
  gchar strwinheight[32];
  gchar *fpath;
  gboolean winsplit, ret;

  // screen -r -X screen $path/extsay.sh [jid [winsplit [height]]]
  fpath = (gchar*)settings_opt_get("extsay_script_path");

  // Helper script path
  if (!fpath || !fpath[0]) {
    scr_log_print(LPRINT_NORMAL, "Please set option 'extsay_script_path'.");
    return;
  }
  fpath = expand_filename(fpath);
  argv[4] = fpath;

  // Helper script parameter #1
  if (args && *args)
    argv[5] = (gchar*)args;
  else
    argv[5] = ".";

  // Update parameters for the helper script
  winsplit = settings_opt_get_int("extsay_split_win");
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

  g_free(fpath);
}

static void do_extsayng(gchar *args)
{
  gboolean expandfjid = FALSE;
  gchar *fjid;

  if (args && !strncmp(args, "." JID_RESOURCE_SEPARATORSTR, 2))
    expandfjid = TRUE;

  if (!args || !*args || expandfjid || !g_strcmp0(args, ".")) {
    const gchar *res = args+2;
    gpointer bud;

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
    if (expandfjid && *res) {
      char *res_utf8 = to_utf8(res);
      fjid = g_strdup_printf("%s%c%s", args, JID_RESOURCE_SEPARATOR, res_utf8);
      g_free(res_utf8);
    } else {
      fjid = g_strdup(args);
    }
  } else {
    fjid = to_utf8(args);
  }

  if (check_jid_syntax(fjid))
    scr_LogPrint(LPRINT_NORMAL, "Please specify a valid Jabber ID.");
  else
    screen_run_script(fjid); // Launch helper script with resulting JID

  g_free(fjid);
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
