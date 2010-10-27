
#ifndef RP_SENDER_KERNEL
	#define RP_SENDER_KERNEL

	#include "ruby.h"

	#define RPRUBY_SENDER_ERROR_STACK_LEVEL_LESS_THAN_ZERO					"Maximum level for stack trace specified was less than zero."

	void Init_senderKernel();

	VALUE rb_RPRuby_Sender_Kernel_backtrace(	int		argc,
											VALUE*	args,
											VALUE	rb_self );
	VALUE rb_RPRuby_Sender_Kernel_each_backtrace_frame(	int		argc,
													   VALUE*	args,
													   VALUE	rb_self );
	VALUE rb_RPRuby_Sender_Kernel_backtrace_includes(	int	argc,
														VALUE*	args,
														VALUE	rb_self );
	VALUE rb_RPRuby_Sender_Kernel_backtrace_includes_one_of(	int		argc,
																VALUE*	args,
																VALUE	rb_self );
	VALUE rb_RPRuby_Sender_Kernel_backtrace_frame_with(	int		argc,
														VALUE*	args,
														VALUE	rb_self );
	VALUE rb_RPRuby_Sender_Kernel_backtrace_frames_with(	int		argc,
															VALUE*	args,
															VALUE	rb_self );
	
	
#endif
