/*
 *  Module "show_mdr"       -- Show Message Delivery Receipts
 *
 *  This module displays the message "delivery receipts" (XEP 184)
 *  received from contacts to the log window.
 *
 * Copyright (C) 2013,2014 Mikael Berthe <mikael@lilotux.net>
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
#include <mcabber/logprint.h>
#include <mcabber/hooks.h>
#include <mcabber/roster.h>
#include <mcabber/utils.h>

static void show_mdr_init(void);
static void show_mdr_uninit(void);

/* Module description */
module_info_t info_show_mdr = {
  /*
        .branch         = MCABBER_BRANCH,
        .api            = MCABBER_API_VERSION,
  */
        .branch         = "dev",
        .api            = 41,     // HOOK_MDR_RECEIVED was included in dev-33
        .version        = "0.01",
        .description    = "Show delivery receipts in the log window.",
        .requires       = NULL,
        .init           = show_mdr_init,
        .uninit         = show_mdr_uninit,
        .next           = NULL,
};

// Hook handler id
static guint mdr_hid;

static int number_of_resources(const char *fjid)
{
  GSList *sl_user;
  gpointer bud;
  GSList *resources, *p_res;
  char *bjid;
  int n = 0;

  if (!fjid) return -1;

  bjid = jidtodisp(fjid);
  sl_user = roster_find(bjid, jidsearch, ROSTER_TYPE_USER);
  g_free(bjid);

  if (!sl_user) return -1;

  bud = sl_user->data;
  resources = buddy_getresources(bud);

  for (p_res = resources ; p_res ; p_res = g_slist_next(p_res)) {
    n++;
    g_free(p_res->data);
  }
  g_slist_free(resources);

  return n;
}

// Event handler for delivery receipts events
static guint mdr_hh(const gchar *hookname, hk_arg_t *args,
                    gpointer userdata)
{
  for ( ; args->name; args++) {
    if (!g_strcmp0(args->name, "jid")) {
      int nres;
      // Note: we could use a whitelist...

      scr_log_print(LPRINT_DEBUG, "Received MDR from %s", args->value);

      /* What we do: we check the number N of resources from the contact and
         display the MDR sender only if N > 1
       */
      nres = number_of_resources(args->value);
      if (nres > 1)
        scr_log_print(LPRINT_NORMAL, "Received MDR from %s", args->value);
    }
  }

  return HOOK_HANDLER_RESULT_ALLOW_MORE_HANDLERS;
}

// Initialization
static void show_mdr_init(void)
{
  // Add hook handler for delivery receipts
  mdr_hid = hk_add_handler(mdr_hh, HOOK_MDR_RECEIVED,
                           G_PRIORITY_DEFAULT_IDLE, NULL);
}

// Uninitialization
static void show_mdr_uninit(void)
{
  // Unregister handler
  hk_del_handler(HOOK_MDR_RECEIVED, mdr_hid);
}

/* vim: set et cindent cinoptions=>2\:2(0 ts=2 sw=2:  For Vim users... */
