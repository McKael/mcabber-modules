/*
   Copyright 2009,2010 Andreas Fett
   Copyright 2010 Mikael Berthe

  Module "extsay"     -- adds a /extsay command
                         Spawns an external editor
  Original code from Andreas Fett.

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
#include <mcabber/screen.h>

static void extsay_init(void);
static void extsay_uninit(void);

/* Module description */
module_info_t info_extsay = {
        .branch         = MCABBER_BRANCH,
        .api            = MCABBER_API_VERSION,
        .version        = "0.01",
        .description    = "Use external editor to send a message\n"
                          "WARNING: use at your own risk - "
                          "mcabber is stuck while you write a message, "
                          "this is not recommended!",
        .requires       = NULL,
        .init           = extsay_init,
        .uninit         = extsay_uninit,
        .next           = NULL,
};

// XXX Not very clean, internal function...
char *load_message_from_file(const char *filename);

static char* spawn_editor() {
  GError *err = NULL;
  gchar *argv[] = { NULL, NULL };
  gchar *tmpfile = NULL;
  gchar *tmpl = "mcabber-XXXXXX";
  gchar *msg;
  gboolean ret;
  gint fd;
  gint exit_status = 0;

  argv[0] = (gchar*)g_getenv("EDITOR");
  if (argv[0] == NULL) {
    scr_LogPrint(LPRINT_NORMAL, "Environment variable EDITOR not set.");
    return NULL;
  }

  fd = g_file_open_tmp(tmpl, &tmpfile, &err);
  if ( fd < 0 ) {
    scr_LogPrint(LPRINT_NORMAL, err->message);
    g_error_free(err);
    return NULL;
  }
  close(fd);

  argv[1] = tmpfile;

  endwin();
  ret = g_spawn_sync(NULL, argv, NULL,
                     G_SPAWN_CHILD_INHERITS_STDIN|G_SPAWN_SEARCH_PATH,
                     NULL, NULL, NULL, NULL, &exit_status, &err);

  raw();
  readline_refresh_screen();

  if (!ret) {
    scr_LogPrint(LPRINT_NORMAL, err->message);
    return NULL;
  }

  if (WEXITSTATUS(exit_status) != EXIT_SUCCESS) {
    scr_LogPrint(LPRINT_NORMAL,
                 "Editor exited with error, discarding message.");
    return NULL;
  }

  msg = load_message_from_file(tmpfile);

  if (g_unlink(tmpfile) != 0)
    scr_LogPrint(LPRINT_NORMAL, "Warning, could not remove temp file.");

  return msg;
}

static void do_extsay(char *args)
{
  char *msg = spawn_editor();
  if (msg) {
    say_cmd(msg, 0);
    g_free(msg);
  }
}

/* Initialization */
static void extsay_init(void)
{
  /* Add command */
  cmd_add("extsay", "Use external editor to write a message",
          0, 0, do_extsay, NULL);
  scr_LogPrint(LPRINT_NORMAL, "Loading module extsay...\n"
               "** Be careful, everything is on hold while you run the "
               "external editor.\n"
               "** Do not run it for too long!"
               );
}

/* Deinitialization */
static void extsay_uninit(void)
{
  /* Unregister command */
  cmd_del("extsay");
}

/* vim: set expandtab cindent cinoptions=>2\:2(0:  For Vim users... */
