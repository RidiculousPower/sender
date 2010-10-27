
#ifndef RP_SENDER_CALLER_GLOBAL_INTERNAL
	#define RP_SENDER_CALLER_GLOBAL_INTERNAL
	
	rb_control_frame_t* RPRuby_internal_framePriorTo();
	rb_control_instruction_t* RPRuby_internal_instructionPriorTo( rb_control_instruction_t* c_control_instruction );

#endif