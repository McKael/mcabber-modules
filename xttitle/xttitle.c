/*
   Copyright (C) 2010 Mikael Berthe <mikael@lilotux.net>

  Module "xttitle"    -- Update xterm tittle


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

#include <stdlib.h>
#include <mcabber/modules.h>
#include <mcabber/settings.h>
#include <mcabber/hooks.h>
#include <mcabber/logprint.h>

static void xttitle_init(void);
static void xttitle_uninit(void);

/* Module description */
module_info_t info_xttitle = {
        .branch         = MCABBER_BRANCH,
        .api            = MCABBER_API_VERSION,
        .version        = "0.01",
        .description    = "Show unread message count in X terminal title",
        .requires       = NULL,
        .init           = xttitle_init,
        .uninit         = xttitle_uninit,
        .next           = NULL,
};

static guint unread_list_hid;

static guint unread_list_hh(const gchar *hookname, hk_arg_t *args,
                            gpointer userdata)
{
  guint all_unread = 0;
  guint muc_unread = 0;
  guint muc_attention = 0;
  guint unread;

  for ( ; args->name; args++) {
    if (!g_strcmp0(args->name, "unread")) {
      all_unread = atoi(args->value);
    } else if (!g_strcmp0(args->name, "muc_unread")) {
      muc_unread = atoi(args->value);
    } else if (!g_strcmp0(args->name, "muc_attention")) {
      muc_attention = atoi(args->value);
    }
  }

  unread = all_unread - (muc_unread - muc_attention);

  if (muc_unread)
    printf("\033]0;MCabber  %d message%c (total:%d / MUC:%d)\007",
           unread, (unread > 1 ? 's' : ' '), all_unread, muc_unread);
  else
    printf("\033]0;MCabber  %d message%c\007", unread,
           (unread > 1 ? 's' : ' '));
  return HOOK_HANDLER_RESULT_ALLOW_MORE_HANDLERS;
}

/* Initialization */
static void xttitle_init(void)
{
  /* Add hook handler for unread message data */
  unread_list_hid = hk_add_handler(unread_list_hh, HOOK_UNREAD_LIST_CHANGE,
                                   G_PRIORITY_DEFAULT_IDLE, NULL);
}

/* Uninitialization */
static void xttitle_uninit(void)
{
  /* Unregister handler */
  hk_del_handler(HOOK_UNREAD_LIST_CHANGE, unread_list_hid);
}

/* vim: set expandtab cindent cinoptions=>2\:2(0:  For Vim users... */
