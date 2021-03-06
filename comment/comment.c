/*
 *  Module "comment"    -- adds a dummy /# command
 *                        (ignore the rest of the command line)
 *
 * Copyright (C) 2010 Mikael Berthe <mikael@lilotux.net>
 *
 * This module is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <mcabber/modules.h>
#include <mcabber/commands.h>

static void comment_init(void);
static void comment_uninit(void);

/* Module description */
module_info_t info_comment = {
        .branch         = MCABBER_BRANCH,
        .api            = MCABBER_API_VERSION,
        .version        = "1.00",
        .description    = "Comment (no-op) pseudo-command\n"
                          "(Pretty useless!)",
        .requires       = NULL,
        .init           = comment_init,
        .uninit         = comment_uninit,
        .next           = NULL,
};

#ifdef MCABBER_API_HAVE_CMD_ID
static gpointer comment_cmdid;
#endif

static void do_comment(char *args)
{
  /* This is actually a no-op! */
}

/* Initialization */
static void comment_init(void)
{
  /* Add command */
#ifdef MCABBER_API_HAVE_CMD_ID
  comment_cmdid = cmd_add("#", "Ignore", 0, 0, do_comment, NULL);
#else
  cmd_add("#", "Ignore", 0, 0, do_comment, NULL);
#endif
}

/* Deinitialization */
static void comment_uninit(void)
{
  /* Unregister command */
#ifdef MCABBER_API_HAVE_CMD_ID
  cmd_del(comment_cmdid);
#else
  cmd_del("comment");
#endif
}

/* vim: set expandtab cindent cinoptions=>2\:2(0:  For Vim users... */
