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

// Forks and run the external helper script
static void screen_run_script(const char *args)
{
  GError *err = NULL;
  gchar *argv[] = { "screen", "-r", "-X", "screen", NULL, NULL, NULL };
  gboolean ret;

  // screen -r -X screen $path/extsay.sh
  argv[4] = (gchar*)settings_opt_get("extsay_script_path");

  if (!argv[4] || !argv[4][0]) {
    scr_log_print(LPRINT_NORMAL, "Please set option 'extsay_script_path'.");
    return;
  }

  if (args && *args)
    argv[5] = (gchar*)args;
  else
    argv[5] = ".";

  ret = g_spawn_async(NULL, argv, NULL,
                      G_SPAWN_SEARCH_PATH, //|
                      //  G_SPAWN_STDOUT_TO_DEV_NULL|G_SPAWN_STDERR_TO_DEV_NULL,
                      NULL, NULL, NULL, &err);

  if (!ret)
    scr_LogPrint(LPRINT_NORMAL, err->message);
}

static void do_extsayng(char *args)
{
  // TODO - check the selected item is a real contact
  // (not a special buffer, nor a group...)
  // TODO provide JID if it isn't provided
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
