
#include "RPSender_internal.h"
#include "RubySourceSupport.h"

/******************
 *  framePriorTo  *
 *****************/

rb_control_frame_t* RPRuby_internal_framePriorTo( rb_control_frame_t* c_control_frame )  {
  
  rb_thread_t*      c_thread          = (rb_thread_t *)RTYPEDDATA_DATA(rb_thread_current());
  rb_control_frame_t*    c_prior_control_frame    = NULL;
  //  get the current frame pointer
  if ( c_control_frame == NULL )  {
    c_control_frame  = c_thread->cfp;
  }
  
    if ( ( c_prior_control_frame = rb_vm_get_ruby_level_next_cfp( c_thread, c_control_frame ) ) != 0) {
    
    //  not sure why we have to call this a second time after it was called at the end of rb_vm_get_ruby_level_next_cfp,
    //  but for some reason it seems to be necessary
    c_prior_control_frame = RUBY_VM_PREVIOUS_CONTROL_FRAME( c_prior_control_frame );

    }
  else {
    c_prior_control_frame = NULL;
  }
  
  //  if we have a nil object we've passed main, we're done
  if ( c_prior_control_frame->self == Qnil )  {
    return NULL;
  }

  return c_prior_control_frame;
  
}

