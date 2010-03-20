/*
   Copyright 2010 Mikael Berthe

  Module "lastmsg"    -- adds a /lastmsg command
                         Displays last personal messages

  This modules stores messages received in a MUC room while
  you are away (or not available) if they contain your nickname.
  When you're back, you can display them in the status window with
  the /lastmsg command.

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

#include <mcabber/modules.h>
#include <mcabber/commands.h>
#include <mcabber/hooks.h>
#include <mcabber/logprint.h>

static void lastmsg_init(void);
static void lastmsg_uninit(void);

/* Module description */
module_info_t info_lastmsg = {
        .branch         = MCABBER_BRANCH,
        .api            = MCABBER_API_VERSION,
        .version        = "0.02",
        .description    = "Add a command /lastmsg",
        .requires       = NULL,
        .init           = lastmsg_init,
        .uninit         = lastmsg_uninit,
        .next           = NULL,
};

static GSList *lastmsg_list;

static guint last_message_hid, last_status_hid;

struct lastm_T {
    gchar *mucname;
    gchar *nickname;
    gchar *msg;
};

static void do_lastmsg(char *args)
{
  if (!lastmsg_list) {
    scr_log_print(LPRINT_NORMAL, "You have no new message.");
    return;
  }

  for (GSList *li = lastmsg_list; li ; li = g_slist_next(li)) {
    struct lastm_T *lastm_item = li->data;
    scr_LogPrint(LPRINT_NORMAL, "In <#%s>, \"%s\" said:\n%s",
                 lastm_item->mucname, lastm_item->nickname,
                 lastm_item->msg);
    g_free(lastm_item->mucname);
    g_free(lastm_item->nickname);
    g_free(lastm_item->msg);
  }
  g_slist_free(lastmsg_list);
  lastmsg_list = NULL;
}

static guint last_message_hh(const gchar *hookname, hk_arg_t *args,
                             gpointer userdata)
{
  enum imstatus status;
  const gchar *bjid, *res, *msg;
  gboolean muc = FALSE, urgent = FALSE;

  status = xmpp_getstatus();

  if (status != notavail && status != away)
    return HOOK_HANDLER_RESULT_ALLOW_MORE_HOOKS;

  bjid = res = NULL;
  msg = NULL;

  for ( ; args->name; args++) {
    if (!g_strcmp0(args->name, "jid"))
      bjid = args->value;
    else if (!g_strcmp0(args->name, "resource"))
      res = args->value;
    else if (!g_strcmp0(args->name, "message"))
      msg = args->value;
    else if (!g_strcmp0(args->name, "groupchat")) {
      if (!g_strcmp0(args->value, "true"))
        muc = TRUE;
    } else if (!g_strcmp0(args->name, "urgent")) {
      if (!g_strcmp0(args->value, "true"))
        urgent = TRUE;
    }
  }

  if (muc && urgent && bjid && res && msg) {
    struct lastm_T *lastm_item;

    lastm_item = g_new(struct lastm_T, 1);
    lastm_item->mucname  = g_strdup(bjid);
    lastm_item->nickname = g_strdup(res);
    lastm_item->msg      = g_strdup(msg);
    lastmsg_list = g_slist_append(lastmsg_list, lastm_item);
  }
  return HOOK_HANDLER_RESULT_ALLOW_MORE_HOOKS;
}

static guint last_status_hh(const gchar *hookname, hk_arg_t *args,
                            gpointer userdata)
{
  gboolean not_away = FALSE;

  for ( ; args->name; args++) {
    if (!g_strcmp0(args->name, "new_status")) {
      if (args->value &&
          (args->value[0] != imstatus2char[away]) &&
          (args->value[0] != imstatus2char[notavail])) {
        not_away = TRUE;
        break;
      }
    }
  }
  if (!not_away || !lastmsg_list)
    return HOOK_HANDLER_RESULT_ALLOW_MORE_HOOKS;

  scr_log_print(LPRINT_NORMAL, "Looks like you're back...");
  scr_log_print(LPRINT_NORMAL, "I've got news for you, use /lastmsg to "
                "read your messages!");
  return HOOK_HANDLER_RESULT_ALLOW_MORE_HOOKS;
}

/* Initialization */
static void lastmsg_init(void)
{
  /* Add command */
  cmd_add("lastmsg", "Display last missed messages", 0, 0, do_lastmsg, NULL);

  last_message_hid = hk_add_handler(last_message_hh, HOOK_POST_MESSAGE_IN,
                                    G_PRIORITY_DEFAULT_IDLE, NULL);
  last_status_hid  = hk_add_handler(last_status_hh, HOOK_MY_STATUS_CHANGE,
                                    G_PRIORITY_DEFAULT_IDLE, NULL);
}

/* Uninitialization */
static void lastmsg_uninit(void)
{
  /* Unregister command */
  cmd_del("lastmsg");
  hk_del_handler(HOOK_POST_MESSAGE_IN, last_message_hid);
  hk_del_handler(HOOK_MY_STATUS_CHANGE, last_status_hid);

  for (GSList *li = lastmsg_list; li ; li = g_slist_next(li)) {
    struct lastm_T *lastm_item = li->data;
    g_free(lastm_item->mucname);
    g_free(lastm_item->nickname);
    g_free(lastm_item->msg);
  }
  g_slist_free(lastmsg_list);
  lastmsg_list = NULL;
}

/* vim: set expandtab cindent cinoptions=>2\:2(0:  For Vim users... */
