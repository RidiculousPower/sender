#ifndef RP_SENDER_INTERNAL
  #define RP_SENDER_INTERNAL

  #include "ruby.h"
  #include "eval_intern.h"

  typedef enum BOOL_e  {
    FALSE,
    TRUE
  } BOOL;

  rb_control_frame_t* RPRuby_internal_framePriorTo( rb_control_frame_t* control_frame );
  VALUE RPSender_internal_backtraceHashForControlFrame( const rb_control_frame_t* c_top_of_control_frame );

#endif
