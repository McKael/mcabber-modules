/*
 * Copyright (C) 2010 Mikael Berthe <mikael@lilotux.net>


  Module "libkillpresence" -- Ignore current presence of an item

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

#include <string.h>

#include <mcabber/modules.h>
#include <mcabber/commands.h>
#include <mcabber/compl.h>
#include <mcabber/roster.h>
#include <mcabber/screen.h>
#include <mcabber/utils.h>

static void killpresence_init(void);
static void killpresence_uninit(void);

/* Module description */
module_info_t info_killpresence = {
        .branch         = MCABBER_BRANCH,
        .api            = MCABBER_API_VERSION,
        .version        = "0.01",
        .description    = "Ignore an item's current presence",
        .requires       = NULL,
        .init           = killpresence_init,
        .uninit         = killpresence_uninit,
        .next           = NULL,
};

static void do_killpresence(char *args)
{
  char *jid_utf8, *res;

  if (!args || !*args)
    return;

  jid_utf8 = to_utf8(args);
  if (!jid_utf8)
    return;

  res = strchr(jid_utf8, JID_RESOURCE_SEPARATOR);
  if (res)
    *res++ = '\0';
  else
    return;

  roster_setstatus(jid_utf8, res, 0,
                   offline, "Killed by killpresence.",
                   0L, role_none, affil_none, NULL);
  buddylist_build();
  scr_draw_roster();

  g_free(jid_utf8);
}

/* Initialization */
static void killpresence_init(void)
{
  /* Add command */
  cmd_add("killpresence", "Ignore presence", COMPL_JID, 0,
          do_killpresence, NULL);
}

/* Uninitialization */
static void killpresence_uninit(void)
{
  /* Unregister command */
  cmd_del("killpresence");
}

/* vim: set expandtab cindent cinoptions=>2\:2(0 sw=2 ts=2:  For Vim users... */
