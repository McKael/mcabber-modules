/*
 *  Module "killpresence" -- Ignore current presence of an item
 *
 * /killpresence fulljid
 *  Ignore current presence for the provided JID
 * /killchatstates fulljid
 *  Reset chat states for the provided JID
 *
 * Copyright (C) 2010 Mikael Berthe <mikael@lilotux.net>
 *
 * This module is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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

#if defined XEP0022 || defined XEP0085
static void reset_chat_states(const char *fulljid)
{
  char *rname, *barejid;
  GSList *sl_buddy;
  struct xep0085 *xep85;
  struct xep0022 *xep22;

  rname = strchr(fulljid, JID_RESOURCE_SEPARATOR);
  if (!rname++)
    return;

  barejid = jidtodisp(fulljid);
  sl_buddy = roster_find(barejid, jidsearch, ROSTER_TYPE_USER);
  g_free(barejid);

  if (!sl_buddy)
    return;

  xep85 = buddy_resource_xep85(sl_buddy->data, rname);
  xep22 = buddy_resource_xep22(sl_buddy->data, rname);

  // Reset Chat States (0085)
  if (xep85) {
    if (xep85->support == CHATSTATES_SUPPORT_PROBED)
      xep85->support = CHATSTATES_SUPPORT_UNKNOWN;
    xep85->last_state_rcvd = ROSTER_EVENT_NONE;
  }
  // Reset Message Events (0022)
  if (xep22) {
    if (xep22->support == CHATSTATES_SUPPORT_PROBED)
      xep22->support = CHATSTATES_SUPPORT_UNKNOWN;
    xep22->last_state_rcvd = ROSTER_EVENT_NONE;
    g_free(xep22->last_msgid_sent);
    g_free(xep22->last_msgid_rcvd);
    xep22->last_msgid_sent = NULL;
    xep22->last_msgid_rcvd = NULL;
  }

  // Finally reset the roster hint for the UI
  buddy_resource_setevents(sl_buddy->data, rname, ROSTER_EVENT_NONE);
  update_roster = TRUE;
}
#endif

static void do_killchatstates(char *args)
{
#if defined XEP0022 || defined XEP0085
  char *jid_utf8;

  if (!args || !*args)
    return;

  jid_utf8 = to_utf8(args);
  if (!jid_utf8)
    return;

  reset_chat_states(jid_utf8);
  g_free(jid_utf8);
#else
  scr_log_print(LPRINT_NORMAL, "No Chat State support.");
#endif
}


/* Initialization */
static void killpresence_init(void)
{
  /* Add command */
  cmd_add("killpresence", "Ignore presence", COMPL_JID, 0,
          do_killpresence, NULL);
  cmd_add("killchatstates", "Reset chatstates", COMPL_JID, 0,
          do_killchatstates, NULL);
}

/* Uninitialization */
static void killpresence_uninit(void)
{
  /* Unregister command */
  cmd_del("killpresence");
  cmd_del("killchatstates");
}

/* vim: set expandtab cindent cinoptions=>2\:2(0 sw=2 ts=2:  For Vim users... */
