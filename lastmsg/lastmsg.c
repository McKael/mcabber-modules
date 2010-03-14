/*
   Copyright 2010 Mikael Berthe

  Module "lastmsg"    -- adds a /lastmsg command
                         Displays last personal messages

  This modules stores messages received in a MUC room while
  you are away (or not available) if they contain your nickname.
  When you're back, you can display them in the status window with
  the /lastmsg command.
  Note that this module is dumb: if your nick is foo and somebody
  says "foobar" the message will be stored!

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
#include <mcabber/xmpp.h>
#include <mcabber/logprint.h>

static void lastmsg_init(void);
static void lastmsg_uninit(void);

/* Module description */
module_info_t info_lastmsg = {
        .branch         = MCABBER_BRANCH,
        .api            = MCABBER_API_VERSION,
        .version        = "0.01",
        .description    = "Add a command /lastmsg",
        .requires       = NULL,
        .init           = lastmsg_init,
        .uninit         = lastmsg_uninit,
        .next           = NULL,
};

static GSList *lastmsg_list;

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

static void last_message_hh(guint32 hid, hk_arg_t *args, gpointer userdata)
{
  enum imstatus status;
  const gchar *bjid, *res, *msg;
  gboolean muc = FALSE;

  status = xmpp_getstatus();

  if (status != notavail && status != away)
    return;

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
    }
  }

  if (muc && bjid && res && msg) {
    GSList *room_elt;
    struct lastm_T *lastm_item;
    const gchar *mynick = NULL;

    room_elt = roster_find(bjid, jidsearch, ROSTER_TYPE_ROOM);
    if (room_elt)
      mynick = buddy_getnickname(room_elt->data);

    if (!mynick || !g_strcmp0(res, mynick))
      return;

    if (!g_strstr_len(msg, -1, mynick))
      return;

    lastm_item = g_new(struct lastm_T, 1);
    lastm_item->mucname  = g_strdup(bjid);
    lastm_item->nickname = g_strdup(res);
    lastm_item->msg      = g_strdup(msg);
    lastmsg_list = g_slist_append(lastmsg_list, lastm_item);
  }
}

static void last_status_hh(guint32 hid, hk_arg_t *args, gpointer userdata)
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
  if (!not_away || !lastmsg_list) return;

  scr_log_print(LPRINT_NORMAL, "Looks like you're back...");
  scr_log_print(LPRINT_NORMAL, "I've got news for you, use /lastmsg to "
                "read your messages!");
}

/* Initialization */
static void lastmsg_init(void)
{
  /* Add command */
  cmd_add("lastmsg", "Display last missed messages", 0, 0, do_lastmsg, NULL);

  hk_add_handler(last_message_hh, HOOK_MESSAGE_IN, NULL);
  hk_add_handler(last_status_hh, HOOK_MY_STATUS_CHANGE, NULL);
}

/* Deinitialization */
static void lastmsg_uninit(void)
{
  /* Unregister command */
  cmd_del("lastmsg");
  hk_del_handler(last_message_hh, NULL);
  hk_del_handler(last_status_hh, NULL);

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
