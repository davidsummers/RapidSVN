
#ifndef _SVN_NOTIFY_H_INCLUDED_
#define _SVN_NOTIFY_H_INCLUDED_

#include "svncpp/notify.h"

class SvnNotify : public svn::Notify
{
private:
  Tracer * _tracer;

public:
  /**
   * Constructor. Sets the Tracer object.
   */
  SvnNotify (Tracer * tracer);
  ~SvnNotify ();

  /**
   * Overrides the base virtual Notify function to provide its 
   * own native response handling of internal events from the C API.
   */
  void onNotify (const char *path,
    svn_wc_notify_action_t action,
    svn_node_kind_t kind,
    const char *mime_type,
    svn_wc_notify_state_t content_state,
    svn_wc_notify_state_t prop_state,
    long revision); 
};

#endif