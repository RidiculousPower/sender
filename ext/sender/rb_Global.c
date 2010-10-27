
#include "rb_Global.h"
#include "rb_Kernel.h"

//	Internals from ruby that aren't included in the ruby lib
#include "RubySourceSupport.h"

#include "eval_intern.h"

/***********
*  Global  *
***********/

void Init_senderGlobal()	{
	
	rb_define_global_function( "__sender__",		rb_RPRuby_Sender___sender__,			0 );
	rb_define_global_function( "__caller__",		rb_RPRuby_Sender___caller__,			0 );

}

/***************************************************************************************************************************************************************
****************************************************************************************************************************************************************
													Ruby Global Methods
****************************************************************************************************************************************************************
***************************************************************************************************************************************************************/

/***************
*  __sender__  *
***************/

/*
 * call-seq:
 *   __sender__ -> object
 *
 * Return object sending message to receiver.
 */
VALUE rb_RPRuby_Sender___sender__()	{
	
	//	we want 3 levels of backtrace:
	//	1: current call to __method__ (__method__ in context)
	//	2: the frame we want, unless it is :new (call to context: __sender__)
	//	3: the frame we want in the case #2 is :new
	VALUE	rb_backtrace_limit		=	INT2FIX( 3 );

	VALUE	rb_backtrace_array		=	rb_RPRuby_Sender_Kernel_backtrace(	1,
																			& rb_backtrace_limit,
																			rb_mKernel );
	
	int		c_backtrace_index		=	1;
	
	VALUE	rb_backtrace_frame_hash	=	rb_ary_entry( rb_backtrace_array, c_backtrace_index );
													
	VALUE	rb_caller	=	rb_hash_aref(	rb_backtrace_frame_hash,
											ID2SYM( rb_intern( "method" ) ) );
	
	//	if we get :initialize as our caller and our __method__ is :initialize, we need to go up the chain
	//	until our caller is no longer :initialize or :new
	while ( rb_caller == ID2SYM( rb_intern( "initialize" ) ) )	{	
		c_backtrace_index++;
		rb_backtrace_frame_hash	=	rb_ary_entry( rb_backtrace_array, c_backtrace_index );
		rb_caller	=	rb_hash_aref(	rb_backtrace_frame_hash,
										ID2SYM( rb_intern( "method" ) ) );
										
		//	we have one parent past our current method; if that is also :initialize, get the whole backtrace
		if ( c_backtrace_index == 2 )	{
			rb_backtrace_array		=	rb_RPRuby_Sender_Kernel_backtrace( 0, NULL, rb_mKernel );
		}
	}	

	//	if we get "new" as our caller we need to get the next level, as we are in :initialize
	//	and want to know what called :new
	//
	//	since we might have had to go up the chain from :initialize through parents before getting here we
	//	want to use a separate if statement
	if ( rb_caller == ID2SYM( rb_intern( "new" ) ) )	{	
		c_backtrace_index++;
		rb_backtrace_frame_hash	=	rb_ary_entry( rb_backtrace_array, c_backtrace_index );
	}
	
	VALUE	rb_sender	=	rb_hash_aref(	rb_backtrace_frame_hash,
											ID2SYM( rb_intern( "object" ) ) );

	//	assuming we have a previous frame, return its rb_self (our current receiver's sender)
	return rb_sender;
}

/***************
*  __caller__  *
***************/

/*
 * call-seq:
 *   __caller__ -> object
 *
 * Return method sending message to receiver.
 */
VALUE rb_RPRuby_Sender___caller__()	{


	//	we want 3 levels of backtrace:
	//	1: current call to __method__ (__method__ in context)
	//	2: the frame we want, unless it is :new (call to context: __sender__)
	//	3: the frame we want in the case #2 is :new
	VALUE	rb_backtrace_limit		=	INT2FIX( 3 );
	
	VALUE	rb_backtrace_array		=	rb_RPRuby_Sender_Kernel_backtrace(	1,
																			& rb_backtrace_limit,
																			rb_mKernel );
	
	int		c_backtrace_index		=	1;
	
	VALUE	rb_backtrace_frame_hash	=	rb_ary_entry( rb_backtrace_array, c_backtrace_index );
	
	VALUE	rb_caller	=	rb_hash_aref(	rb_backtrace_frame_hash,
											ID2SYM( rb_intern( "method" ) ) );
	
	//	we want to compare our caller as we go up the chain to our first caller to deal with super methods
	//	we have a symbol so no cloning is necessary (or appropriate)
	VALUE	rb_first_caller	=	rb_hash_aref(	rb_ary_entry( rb_backtrace_array, 0 ),
												ID2SYM( rb_intern( "method" ) ) );
	
	//	if we get :initialize as our caller and our __method__ is :initialize, we need to go up the chain
	//	until our caller is no longer :initialize or :new
	while ( rb_caller == rb_first_caller )	{	
		c_backtrace_index++;
		rb_backtrace_frame_hash	=	rb_ary_entry( rb_backtrace_array, c_backtrace_index );
		rb_caller	=	rb_hash_aref(	rb_backtrace_frame_hash,
										ID2SYM( rb_intern( "method" ) ) );
		
		//	we have one parent past our current method; if that is also :initialize, get the whole backtrace
		if ( c_backtrace_index == 2 )	{
			rb_backtrace_array		=	rb_RPRuby_Sender_Kernel_backtrace( 0, NULL, rb_mKernel );
		}
	}	
	
	//	if we get "new" as our caller we need to get the next level, as we are in :initialize
	//	and want to know what called :new
	//
	//	since we might have had to go up the chain from :initialize through parents before getting here we
	//	want to use a separate if statement	
	if ( rb_caller == ID2SYM( rb_intern( "new" ) ) )	{	
		c_backtrace_index++;
		rb_backtrace_frame_hash	=	rb_ary_entry( rb_backtrace_array, c_backtrace_index );		
		rb_caller	=	rb_hash_aref(	rb_backtrace_frame_hash,
										ID2SYM( rb_intern( "method" ) ) );
	}
	
	//	assuming we have a previous frame, return its rb_self (our current receiver's sender)
	return rb_caller;
}

