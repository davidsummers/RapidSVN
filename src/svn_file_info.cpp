/*
 * ====================================================================
 * Copyright (c) 2002, 2003 The RapidSvn Group.  All rights reserved.
 *
 * This software is licensed as described in the file LICENSE.txt,
 * which you should have received as part of this distribution.
 *
 * This software consists of voluntary contributions made by many
 * individuals.  For exact contribution history, see the revision
 * history and logs, available at http://rapidsvn.tigris.org/.
 * ====================================================================
 */

// subversion
#include "svn_path.h"
#include "svn_wc.h"

// wxwindows
#include "wx/wx.h"

// app
#include "svn_file_info.hpp"


static void
info_print_time (apr_time_t atime, const wxChar * desc, wxString & str)
{
  apr_time_exp_t extime;
  apr_status_t apr_err;

  /* if this returns an error, just don't print anything out */
  apr_err = apr_time_exp_tz (&extime, atime, 0);
  if (!apr_err)
    str.Printf (_("%s: %04lu-%02lu-%02lu %02lu:%02lu GMT\n"), desc,
                (unsigned long) (extime.tm_year + 1900),
                (unsigned long) (extime.tm_mon + 1),
                (unsigned long) (extime.tm_mday),
                (unsigned long) (extime.tm_hour),
                (unsigned long) (extime.tm_min));
}


svn_error_t *
svn_get_file_info (const wxString & path, apr_pool_t * pool, wxString & info)
{
  const svn_wc_entry_t *entry;
  wxString str;
  svn_boolean_t text_conflict = FALSE;
  svn_boolean_t props_conflict = FALSE;
  svn_wc_adm_access_t *adm_access;

  SVN_ERR (svn_wc_adm_probe_open (&adm_access, NULL, path.c_str (), FALSE,
                                  FALSE, pool));
  info = path + _("\n");

  SVN_ERR (svn_wc_entry (&entry, path.c_str (), adm_access, FALSE, pool));
  if (!entry)
  {
    info += _("(Not a versioned resource)\n");
    return SVN_NO_ERROR;
  }

  if ((entry->name) && strcmp (entry->name, SVN_WC_ENTRY_THIS_DIR))
  {
    str.Printf (_("Name: %s\n"), entry->name);
    info += str;
  }

  if (entry->url)
  {
    str.Printf ("Url: %s\n", entry->url);
    info += str;
  }

  if (entry->repos)
  {
    str.Printf ("Repository: %s\n", entry->repos);
    info += str;
  }

  if (SVN_IS_VALID_REVNUM (entry->revision))
  {
    str.Printf ("Revision: %" SVN_REVNUM_T_FMT "\n", entry->revision);
    info += str;
  }

  switch (entry->kind)
  {
  case svn_node_file:
    info += _("Node Kind: file\n");
    {
      const char *dir_name;
      svn_path_split (path.c_str (), &dir_name, NULL, pool);
      SVN_ERR (svn_wc_conflicted_p (&text_conflict, &props_conflict,
                                    dir_name, entry, pool));
    }
    break;

  case svn_node_dir:
    info += _("Node Kind: directory\n");
    SVN_ERR (svn_wc_conflicted_p (&text_conflict, &props_conflict,
                                  path.c_str (), entry, pool));
    break;

  case svn_node_none:
    info += _("Node Kind: none\n");
    break;

  case svn_node_unknown:
  default:
    info += _("Node Kind: unknown\n");
    break;
  }

  switch (entry->schedule)
  {
  case svn_wc_schedule_normal:
    info += _("Schedule: normal\n");
    break;

  case svn_wc_schedule_add:
    info += _("Schedule: add\n");
    break;

  case svn_wc_schedule_delete:
    info += _("Schedule: delete\n");
    break;

  case svn_wc_schedule_replace:
    info += _("Schedule: replace\n");
    break;

  default:
    break;
  }

  if (entry->copied)
  {
    if (entry->copyfrom_url)
    {
      str.Printf (_("Copied From Url: %s\n"), entry->copyfrom_url);
      info += str;
    }

    if (SVN_IS_VALID_REVNUM (entry->copyfrom_rev))
    {
      str.Printf (_("Copied From Rev: %" SVN_REVNUM_T_FMT "\n"),
                  entry->copyfrom_rev);
      info += str;
    }
  }

  if (entry->cmt_author)
  {
    str.Printf (_("Last Changed Author: %s\n"), entry->cmt_author);
    info += str;
  }


  if (SVN_IS_VALID_REVNUM (entry->cmt_rev))
  {
    str.Printf (_("Last Changed Rev: %" SVN_REVNUM_T_FMT "\n"),
                entry->cmt_rev);
    info += str;
  }


  if (entry->cmt_date)
  {
    info_print_time (entry->cmt_date, _("Last Changed Date"), str);
    info += str;
  }

  if (entry->text_time)
  {
    info_print_time (entry->text_time, _("Text Last Updated"), str);
    info += str;
  }

  if (entry->prop_time)
  {
    info_print_time (entry->prop_time, _("Properties Last Updated"), str);
    info += str;
  }

  if (entry->checksum)
  {
    str.Printf (_("Checksum: %s\n"), entry->checksum);
    info += str;
  }

  if (text_conflict && entry->conflict_old)
  {
    str.Printf (_("Conflict Previous Base File: %s\n"),
                entry->conflict_old);
    info += str;
  }

  if (text_conflict && entry->conflict_wrk)
  {
    str.Printf (_("Conflict Previous Working File: %s\n"),
                entry->conflict_wrk);
    info += str;
  }

  if (text_conflict && entry->conflict_new)
  {
    str.Printf (_("Conflict Current Base File: %s\n"), entry->conflict_new);
    info += str;
  }


  if (props_conflict && entry->prejfile)
  {
    str.Printf (_("Conflict Properties File: %s\n"), entry->prejfile);
    info += str;
  }

  return SVN_NO_ERROR;
}
/* -----------------------------------------------------------------
 * local variables:
 * eval: (load-file "../rapidsvn-dev.el")
 * end:
 */
