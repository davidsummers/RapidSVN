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

// wxwindows
#include "wx/wx.h"

// svncpp
#include "svncpp/exception.hpp"
#include "svncpp/client.hpp"
#include "svncpp/pool.hpp"
#include "svncpp/targets.hpp"

// app
#include "commit_action.hpp"
#include "commit_dlg.hpp"
#include "ids.hpp"
#include "svn_notify.hpp"
#include "tracer.hpp"
#include "utils.hpp"

CommitAction::CommitAction (wxWindow * parent)
  : Action (parent, actionWithTargets)
{
}

bool
CommitAction::Prepare ()
{
  if (!Action::Prepare ())
  {
    return false;
  }

  CommitDlg dlg(GetParent ());

  if (dlg.ShowModal () != wxID_OK)
  {
    return false;
  }

  m_data = dlg.GetData ();
  return true;
}

bool
CommitAction::Perform ()
{
  SvnNotify notify (GetTracer ());
  svn::Context context (m_data.User.c_str(), m_data.Password.c_str());
  svn::Client client (&context);
  client.notification (&notify);

  const svn::Targets & targets = GetTargets ();

  bool result = false;
  try
  {
    svn::Pool pool;
    long revision = 
      client.commit (targets.array (pool), m_data.LogMessage.c_str (), 
                     m_data.Recursive);
    wxString str;

    str = wxString::Format ("Committed revision %" SVN_REVNUM_T_FMT ".",
                               revision);
    GetTracer ()->Trace (str);
    result = true;
  }
  catch (svn::ClientException &e)
  {
    PostStringEvent (TOKEN_SVN_INTERNAL_ERROR, _(e.description ()), 
                     ACTION_EVENT);
    GetTracer ()->Trace ("The commit action failed:");
    GetTracer ()->Trace (e.description ());
  }

  PostDataEvent (TOKEN_ACTION_END, NULL, ACTION_EVENT);
  return result;
}
/* -----------------------------------------------------------------
 * local variables:
 * eval: (load-file "../rapidsvn-dev.el")
 * end:
 */
