/*
 *  Module "killpresence" -- Ignore current presence of an item
 *
 * /killpresence [-p] fulljid
 *  Ignore current presence for the provided JID
 *  Useful for kicking ghosts from the roster...
 *  Shortcuts can be used for the full jid.  Example:
 *    /killpresence ./resource
 *  Also, resource '*' stands for all resources.
 *  If the option -p is given, a presence probe will be sent
 *  to the user after removing the resource(s).
 *
 * /killchatstates fulljid
 *  Reset chat states for the provided JID
 *
 * /probe barejid
 *  Send a presence probe to the provided JID
 *
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
#include <mcabber/xmpp.h>

static void killpresence_init(void);
static void killpresence_uninit(void);

static void do_probe(char *);

/* Module description */
module_info_t info_killpresence = {
        .branch         = MCABBER_BRANCH,
        .api            = MCABBER_API_VERSION,
        .version        = "0.10",
        .description    = "Ignore an item's current presence(s)\n"
                          " Provides the following commands:\n"
                          " /killpresence [-p] $fulljid\n"
                          " /killchatstates $fulljid"
                          " /probe $barejid",
        .requires       = NULL,
        .init           = killpresence_init,
        .uninit         = killpresence_uninit,
        .next           = NULL,
};

#ifdef MCABBER_API_HAVE_CMD_ID
static gpointer killpresence_cmdid, killchatstates_cmdid, probe_cmdid;
#endif

static void do_killpresence(char *args)
{
  char *jid_utf8, *res;
  const char *targetjid = NULL;
  bool probe = false;

  if (!args || !*args) {
    scr_log_print(LPRINT_NORMAL, "I need a full JID.");
    return;
  }

  if (!strncmp(args, "-p ", 3)) {
    for (args += 3; *args && *args == ' '; args++)
      ;
    probe = true;
  }

  jid_utf8 = to_utf8(args);
  if (!jid_utf8)
    return;

  res = strchr(jid_utf8, JID_RESOURCE_SEPARATOR);
  if (res) {
    *res++ = '\0';
    if (!strcmp(jid_utf8, ".")) {
      if (current_buddy)
        targetjid = CURRENT_JID;
    } else {
      targetjid = jid_utf8;
    }
  }

  if (!targetjid) {
    scr_log_print(LPRINT_NORMAL, "I need a /full/ JID.");
    g_free(jid_utf8);
    return;
  }


  if (!strcmp(res, "*")) {
    // Kill all resources!
    GSList *sl_user = roster_find(targetjid, jidsearch, ROSTER_TYPE_USER);
    if (sl_user) {
      scr_log_print(LPRINT_NORMAL,
                    "Killing all resources from <%s> now!", targetjid);
      buddy_del_all_resources(sl_user->data);
    } else {
      scr_log_print(LPRINT_NORMAL, "Cannot find <%s>...", targetjid);
    }
  } else {
    roster_setstatus(targetjid, res, 0,
                     offline, "Killed by killpresence.",
                     0L, role_none, affil_none, NULL);
  }
  buddylist_build();
  scr_draw_roster();

  if (probe)
    do_probe((char *)targetjid);

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
  if (!rname++) {
    scr_log_print(LPRINT_NORMAL, "I need a /full/ JID.");
    return;
  }

  barejid = jidtodisp(fulljid);
  sl_buddy = roster_find(barejid, jidsearch, ROSTER_TYPE_USER);
  g_free(barejid);

  if (!sl_buddy) {
    scr_log_print(LPRINT_NORMAL, "Resource not found.");
    return;
  }

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

  if (!args || !*args) {
    scr_log_print(LPRINT_NORMAL, "I need a full JID.");
    return;
  }

  jid_utf8 = to_utf8(args);
  if (!jid_utf8)
    return;

  reset_chat_states(jid_utf8);
  g_free(jid_utf8);
#else
  scr_log_print(LPRINT_NORMAL, "No Chat State support.");
#endif
}

static void do_probe(char *args)
{
  char *jid_utf8;
  LmMessage *m;
  const char *targetjid = NULL;

  if (!args || !*args) {
    scr_log_print(LPRINT_NORMAL, "I need a JID.");
    return;
  }
  if (strchr(args, JID_RESOURCE_SEPARATOR)) {
    scr_log_print(LPRINT_NORMAL, "I need a *bare* JID.");
    // XXX We could just drop the resource...
    return;
  }

  if (!xmpp_is_online())
    return;

  jid_utf8 = to_utf8(args);
  if (!jid_utf8)
    return;

  if (!strcmp(jid_utf8, ".")) {
    if (current_buddy)
      targetjid = CURRENT_JID;
  } else {
    targetjid = jid_utf8;
  }

  // Create presence message with type "probe"
  m = lm_message_new(targetjid, LM_MESSAGE_TYPE_PRESENCE);
  lm_message_node_set_attribute(m->node, "type", "probe");
  lm_connection_send(lconnection, m, NULL);
  lm_message_unref(m);
  scr_log_print(LPRINT_LOGNORM, "Presence probe sent to <%s>.", targetjid);
  g_free(jid_utf8);
}


/* Initialization */
static void killpresence_init(void)
{
  /* Add command */
#ifdef MCABBER_API_HAVE_CMD_ID
  killpresence_cmdid = cmd_add("killpresence", "Ignore presence",
                               COMPL_JID, 0, do_killpresence, NULL);
  killchatstates_cmdid = cmd_add("killchatstates", "Reset chatstates",
                                 COMPL_JID, 0, do_killchatstates, NULL);
  probe_cmdid = cmd_add("probe", "Send a presence probe",
                                 COMPL_JID, 0, do_probe, NULL);
#else
  cmd_add("killpresence", "Ignore presence", COMPL_JID, 0,
          do_killpresence, NULL);
  cmd_add("killchatstates", "Reset chatstates", COMPL_JID, 0,
          do_killchatstates, NULL);
  cmd_add("probe", "Send a presence probe", COMPL_JID, 0,
          do_probe, NULL);
#endif
}

/* Uninitialization */
static void killpresence_uninit(void)
{
  /* Unregister command */
#ifdef MCABBER_API_HAVE_CMD_ID
  cmd_del(killpresence_cmdid);
  cmd_del(killchatstates_cmdid);
  cmd_del(probe_cmdid);
#else
  cmd_del("killpresence");
  cmd_del("killchatstates");
  cmd_del("probe");
#endif
}

/* vim: set expandtab cindent cinoptions=>2\:2(0 sw=2 ts=2:  For Vim users... */
