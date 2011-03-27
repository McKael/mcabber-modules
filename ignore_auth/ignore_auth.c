/*
 *  Module "ignore_auth" -- ignore subscription requests
 *
 * Copyright 2010 Frank Zschockelt <mcabber@freakysoft.de>
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

#include <mcabber/commands.h>
#include <mcabber/hooks.h>
#include <mcabber/modules.h>
#include <mcabber/screen.h>
#include <mcabber/settings.h>
#include <mcabber/xmpp.h>

static void ignore_auth_init   (void);
static void ignore_auth_uninit (void);

/* Module description */
module_info_t info_ignore_auth = {
        .branch          = MCABBER_BRANCH,
        .api             = MCABBER_API_VERSION,
        .version         = MCABBER_VERSION,
        .description     = "ignore auth requests by specifying a jid regex",
        .requires        = NULL,
        .init            = ignore_auth_init,
        .uninit          = ignore_auth_uninit,
        .next            = NULL,
};

#ifdef MCABBER_API_HAVE_CMD_ID
static gpointer ignoreauth_cmdid;
#endif

GSList *regexlist = NULL;
static guint ignore_auth_hid = 0;  /* Hook handler id */

static guint ignore_hh(const gchar *hookname, hk_arg_t *args, gpointer userdata)
{
  guint subscription;
  const char *bjid = NULL, *type = NULL, *msg = NULL;

  if (settings_opt_get_int("ignore_auth")) {
    int i;
    for (i = 0; args[i].name; i++) {
      if (!strcmp(args[i].name, "type"))
        type = args[i].value;
      if (!strcmp(args[i].name, "message"))
        msg = args[i].value;
      if (!strcmp(args[i].name, "jid"))
        bjid = args[i].value;
    }
    /* shouldn't happen */
    if (!bjid || !type || !msg)
      return HOOK_HANDLER_RESULT_ALLOW_MORE_HANDLERS;

    subscription = roster_getsubscription(bjid);
    /* only ignore subscription requests when no subscription exists */
    if ((subscription == sub_none) && (!strcmp(type, "subscribe"))) {
      GSList *head;
      int ignore_it = FALSE;

      /* try to match against one of our regular expressions */
      for (head = regexlist; head && !ignore_it; head = g_slist_next(head)) {
        GMatchInfo *match_info;
        g_regex_match(head->data, bjid, 0, &match_info);
        if (g_match_info_matches(match_info))
          ignore_it = TRUE;
        g_match_info_free (match_info);
      }
      if(ignore_it) {
        xmpp_send_s10n(bjid, LM_MESSAGE_SUB_TYPE_UNSUBSCRIBED);
        scr_log_print(LPRINT_NORMAL, "Ignored auth request from %s (%s)",
                      bjid, msg);
        return HOOK_HANDLER_RESULT_NO_MORE_HANDLER_DROP_DATA;
      }
    }
  }
  /* we're done, let the other handlers do their job! */
  return HOOK_HANDLER_RESULT_ALLOW_MORE_HANDLERS;
}

/* ignore command handler */
static void do_ignore_auth(char *args)
{
  GRegex *new_regex;
  if (!args || !*args) {
    GSList *head;
    for (head = regexlist; head; head = g_slist_next(head))
      scr_log_print(LPRINT_NORMAL, "Ignoring %s",
                    g_regex_get_pattern(head->data));
    return;
  }
  new_regex = g_regex_new(args, G_REGEX_OPTIMIZE, 0, NULL);
  if (!new_regex) {
    scr_log_print(LPRINT_NORMAL, "That wasn't a glib regex");
    return;
  }
  regexlist = g_slist_append(regexlist, new_regex);
}

/* Initialization */
static void ignore_auth_init(void)
{
  /* Add command */
#ifdef MCABBER_API_HAVE_CMD_ID
  ignoreauth_cmdid = cmd_add("ignore_auth", "", 0, 0, do_ignore_auth, NULL);
#else
  cmd_add("ignore_auth", "", 0, 0, do_ignore_auth, NULL);
#endif
  /* Add handler */
  ignore_auth_hid = hk_add_handler(ignore_hh, HOOK_SUBSCRIPTION,
                                   G_PRIORITY_DEFAULT_IDLE, NULL);
  settings_set(SETTINGS_TYPE_OPTION, "ignore_auth", "1");
}

/* Uninitialization */
static void ignore_auth_uninit(void)
{
  GSList *head;
  /* Unregister command */
#ifdef MCABBER_API_HAVE_CMD_ID
  cmd_del(ignoreauth_cmdid);
#else
  cmd_del("ignore_auth");
#endif
  /* Unregister event handler */
  hk_del_handler(HOOK_SUBSCRIPTION, ignore_auth_hid);
  /* unref every regex */
  for (head = regexlist; head; head = g_slist_next(head))
    g_regex_unref(head->data);
  g_slist_free(regexlist);
  regexlist = NULL;
}

/* vim: set et cindent cinoptions=>2\:2(0 ts=2 sw=2:  For Vim users... */
