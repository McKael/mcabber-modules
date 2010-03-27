/*
 *  Module "info_msgcount"  -- Show number of unread buffers in status bar
 *
 *  This module relies on the "info" option to display the number of
 *  unread messages in the status bar...
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

#include <stdio.h>
#include <stdlib.h>
#include <mcabber/modules.h>
#include <mcabber/settings.h>
#include <mcabber/screen.h>
#include <mcabber/hooks.h>

static void info_msgcount_init(void);
static void info_msgcount_uninit(void);

/* Module description */
module_info_t info_info_msgcount = {
        .branch         = MCABBER_BRANCH,
        .api            = MCABBER_API_VERSION,
        .version        = "0.01",
        .description    = "Show unread message count in the status bar",
        .requires       = NULL,
        .init           = info_msgcount_init,
        .uninit         = info_msgcount_uninit,
        .next           = NULL,
};

// Hook handler id
static guint unread_list_hid;

static gchar *backup_info;

// Event handler for HOOK_UNREAD_LIST_CHANGE events
static guint unread_list_hh(const gchar *hookname, hk_arg_t *args,
                            gpointer userdata)
{
  static gchar buf[128];
  guint all_unread = 0;
  guint muc_unread = 0;
  guint muc_attention = 0;
  guint unread; // private message count

  // Note: We can add "attention" string later, but it isn't used
  // yet in mcabber...
  for ( ; args->name; args++) {
    if (!g_strcmp0(args->name, "unread")) {
      all_unread = atoi(args->value);
    } else if (!g_strcmp0(args->name, "muc_unread")) {
      muc_unread = atoi(args->value);
    } else if (!g_strcmp0(args->name, "muc_attention")) {
      muc_attention = atoi(args->value);
    }
  }

  // Let's not count the MUC unread buffers that don't have the attention
  // flag (that is, MUC buffer that have no highlighted messages).
  unread = all_unread - (muc_unread - muc_attention);

  // Update the status bar
  snprintf(buf, sizeof(buf), "(%d/%d) ", unread, all_unread);
  settings_set(SETTINGS_TYPE_OPTION, "info", buf);
  scr_update_chat_status(TRUE);

  return HOOK_HANDLER_RESULT_ALLOW_MORE_HANDLERS;
}

// Initialization
static void info_msgcount_init(void)
{
  // Backup info option, set default initial string
  backup_info = g_strdup(settings_opt_get("info"));
  settings_set(SETTINGS_TYPE_OPTION, "info", "(...)");
  scr_update_chat_status(TRUE);

  // Add hook handler for unread message data
  unread_list_hid = hk_add_handler(unread_list_hh, HOOK_UNREAD_LIST_CHANGE,
                                   G_PRIORITY_DEFAULT_IDLE, NULL);
}

// Uninitialization
static void info_msgcount_uninit(void)
{
  // Unregister handler
  hk_del_handler(HOOK_UNREAD_LIST_CHANGE, unread_list_hid);

  // Restore initial info option value
  settings_set(SETTINGS_TYPE_OPTION, "info", backup_info);
  g_free(backup_info);
  backup_info = NULL;
  scr_update_chat_status(TRUE);
}

/* vim: set et cindent cinoptions=>2\:2(0 ts=2 sw=2:  For Vim users... */
