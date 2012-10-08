
#include "rb_Kernel.h"
#include "rb_Kernel_internal.h"

#include "RPSender_internal.h"

#include "RubySourceSupport.h"

/***********
*  Kernel  *
***********/

void Init_senderKernel()  {
  
  rb_define_singleton_method( rb_mKernel, "backtrace",            rb_RPRuby_Sender_Kernel_backtrace,            -1 );
  rb_define_singleton_method( rb_mKernel, "each_backtrace_frame",        rb_RPRuby_Sender_Kernel_each_backtrace_frame,      -1 );
  rb_define_singleton_method( rb_mKernel, "backtrace_includes?",        rb_RPRuby_Sender_Kernel_backtrace_includes,        -1 );
  rb_define_singleton_method( rb_mKernel, "backtrace_includes_one_of?",    rb_RPRuby_Sender_Kernel_backtrace_includes_one_of,    -1 );
  rb_define_singleton_method( rb_mKernel, "backtrace_frame_with",        rb_RPRuby_Sender_Kernel_backtrace_frame_with,      -1 );
  rb_define_singleton_method( rb_mKernel, "backtrace_frames_with",      rb_RPRuby_Sender_Kernel_backtrace_frames_with,      -1 );


}

static VALUE rb_eFiberError;

#define RPRUBY_SENDER_ERROR_NO_ENUMERATORS              "Enumerators not currently enabled due to difficulty with Fiber support."

/***************************************************************************************************************************************************************
****************************************************************************************************************************************************************
                          Ruby Kernel Methods
****************************************************************************************************************************************************************
***************************************************************************************************************************************************************/

/**********************
 *  Kernel.backtrace  *
 *********************/

/*
 * call-seq:
 *   Kernel.backtrace( number_of_frames = nil ) -> [ { :object => object, :method => method }, ... ]
 *
 * Return array of hashes with object and method frame information for backtrace.
 * Specifying number_of_frames will cause only the last number_of_frames to be returned.
 * Kernel.backtrace returns all frames including the current context (__method__/__callee__).
 */
VALUE rb_RPRuby_Sender_Kernel_backtrace(  int    argc,
                      VALUE*  args,
                      VALUE  rb_self )  {

  //  Get max stack level from args if it is there
  int    c_max_stack_level  =  0;
  if ( argc )  {
    c_max_stack_level  =  FIX2INT( args[ 0 ] );
    
    //  if max_stack_level is 0 return empty array
    if ( c_max_stack_level == 0 )  {
      return rb_ary_new();
    }
    //  if max_stack_level < 0, throw error
    else if ( c_max_stack_level < 0 ) {
      rb_raise( rb_eArgError, RPRUBY_SENDER_ERROR_STACK_LEVEL_LESS_THAN_ZERO );
    }
    
  }
  
  rb_thread_t*      c_thread          = (rb_thread_t *)RTYPEDDATA_DATA(rb_thread_current());
  //  Get the current frame - we're doing a backtrace, so our current working frame to start is the first previous thread
  rb_control_frame_t*    c_current_context_frame    = RUBY_VM_PREVIOUS_CONTROL_FRAME( c_thread->cfp );
  
  //  c_top_of_control_frame describes the top edge of the stack trace
  //  set c_top_of_control_frame to the first frame in <main>
    rb_control_frame_t*    c_top_of_control_frame  =  RUBY_VM_NEXT_CONTROL_FRAME( RUBY_VM_NEXT_CONTROL_FRAME( (void *)( c_thread->stack + c_thread->stack_size ) ) );
  
  VALUE  rb_return_array  =  rb_ary_new();
  
  int  c_stack_level  =  0;
  //  for each control frame:
    while (    c_current_context_frame < c_top_of_control_frame
       &&  (  argc == 0
         ||  c_stack_level < c_max_stack_level ) ) {
    
    VALUE  rb_frame_hash  =  rb_RPRuby_Sender_Kernel_internal_backtraceHashForControlFrame(  & c_current_context_frame );
     
    if ( rb_frame_hash == Qnil )  {
      break;
    }
    
    //  push hash to array
    rb_ary_push(  rb_return_array,
            rb_frame_hash );
     
     c_current_context_frame = RUBY_VM_PREVIOUS_CONTROL_FRAME( c_current_context_frame );
     c_stack_level++;
   }
   
   return rb_return_array;
}

/*********************************
 *  Kernel.each_backtrace_frame  *
 ********************************/

/*
 * call-seq:
 *   Kernel.each_backtrace_frame( & block )
 *
 * Return array of hashes with object and method frame information for backtrace.
 * Specifying number_of_frames will cause only the last number_of_frames to be returned.
 * Kernel.backtrace returns all frames including the current context (__method__/__callee__).
 */
VALUE rb_RPRuby_Sender_Kernel_each_backtrace_frame(  int    argc,
                          VALUE*  args,
                          VALUE  rb_self )  {

  rb_thread_t*      c_thread          = (rb_thread_t *)RTYPEDDATA_DATA(rb_thread_current());
  //  Get the current frame - we're doing a backtrace, so our current working frame to start is the first previous thread
  rb_control_frame_t*    c_current_context_frame    = RUBY_VM_PREVIOUS_CONTROL_FRAME( RUBY_VM_PREVIOUS_CONTROL_FRAME( c_thread->cfp ) );
  
  //  c_top_of_control_frame describes the top edge of the stack trace
  //  set c_top_of_control_frame to the first frame in <main>
    rb_control_frame_t*    c_top_of_control_frame  =  RUBY_VM_NEXT_CONTROL_FRAME( RUBY_VM_NEXT_CONTROL_FRAME( (void *)( c_thread->stack + c_thread->stack_size ) ) );
  
  VALUE  rb_stored_backtrace_array  =  Qnil;
  
  //  if we were passed a stored backtrace array, use it
  if (  argc == 1
    &&  TYPE( args[ 0 ] ) == T_ARRAY )  {
    rb_stored_backtrace_array  =  args[ 0 ];
  }
        
  //  for each control frame:
    while ( c_current_context_frame < c_top_of_control_frame ) {

    VALUE  rb_frame_hash;
    //  if we are using a stored backtrace we don't need to ask for a new hash
    if ( rb_stored_backtrace_array == Qnil )  {
      rb_frame_hash  =  rb_RPRuby_Sender_Kernel_internal_backtraceHashForControlFrame(  & c_current_context_frame );
    }
    else {
      rb_frame_hash  =  rb_ary_shift( rb_stored_backtrace_array );
    }

    if ( rb_frame_hash == Qnil )  {
      break;
    }
    
    //  if we try to iterate using an Enumerator we will lose our context
    if ( ! rb_block_given_p() )  {
      
      //  we solve this by assuming that the desired context is the moment when each_backtrace_frame is called
      //  this allows us to store the backtrace and iterate it as we want
      //  the only downside is that we have to get the entire backtrace first in order to store it
      rb_stored_backtrace_array  =  rb_RPRuby_Sender_Kernel_backtrace(  0,
                                        NULL,
                                        rb_self );
    
      RETURN_ENUMERATOR( rb_self, 1, & rb_stored_backtrace_array );
    }
    
    //  otherwise, yield the block
    rb_yield( rb_frame_hash );
    
    //  only move the frame if we are not using a stored backtrace
    if ( rb_stored_backtrace_array == Qnil )  {
      c_current_context_frame = RUBY_VM_PREVIOUS_CONTROL_FRAME( c_current_context_frame );    
    }
  }
  
  return Qnil;
}
  
/********************************
 *  Kernel.backtrace_includes?  *
 *******************************/

/*
 * call-seq:
 *   Kernel.backtrace_includes?( method_or_object, ... ) -> true or false
 *   Kernel.backtrace_includes?( number_of_frames, method_or_object, ... ) -> true or false
 *
 * Returns whether specified methods or objects or classes are in the current backtrace context.
 * Kernel.backtrace_includes? begins with the prior frame, so asking if the backtrace includes the current method
 * will only report true if the current method is part of the earlier call chain.
 */
VALUE rb_RPRuby_Sender_Kernel_backtrace_includes(  int    argc,
                          VALUE*  args,
                          VALUE  rb_self )  {
  
  //  this function is also used for 
  //  * backtrace_includes_one_of?
  //  * backtrace_includes_frame?
  //  * backtrace_includes_one_of_frames?
  
  //  create tracking array
  VALUE  rb_tracking_array  =  rb_ary_new();
  
  //  populate tracking array with methods/objects
  //  optional - if first arg is Qtrue, we are looking for one of the args instead of all of the args
  int    c_which_arg        =  0;
  BOOL  c_requires_all_items  = TRUE;
  if (  args[ 0 ] == Qnil
    ||  (  argc > 1
       &&  args[ 1 ] == Qnil ) )  {
    c_which_arg++;
    c_requires_all_items = FALSE;
  }
  BOOL  c_return_frame  =  FALSE;
  if (  args[ 0 ] == Qfalse
    ||  (  argc > 1
       &&  args[ 1 ] == Qfalse ) )  {
    c_which_arg++;
    c_return_frame = TRUE;
  }
  BOOL  c_return_all_frames  =  FALSE;
  if (  args[ 0 ] == Qtrue
    ||  (  argc > 1
       &&  args[ 1 ] == Qtrue ) )  {
    c_which_arg++;
    c_return_all_frames = TRUE;
  }
  int  c_args_offset  =  c_which_arg;
  for ( ; c_which_arg < argc ; c_which_arg++ )  {
    rb_ary_push(  rb_tracking_array,
            args[ c_which_arg ] );
  }
    
  rb_thread_t*      c_thread          = (rb_thread_t *)RTYPEDDATA_DATA(rb_thread_current());
  //  Get the current frame - we're doing a backtrace, so our current working frame to start is the first previous thread
  rb_control_frame_t*    c_current_context_frame    = RUBY_VM_PREVIOUS_CONTROL_FRAME( RUBY_VM_PREVIOUS_CONTROL_FRAME( c_thread->cfp ) );
  
  //  c_top_of_control_frame describes the top edge of the stack trace
  //  set c_top_of_control_frame to the first frame in <main>
    rb_control_frame_t*    c_top_of_control_frame  =  RUBY_VM_NEXT_CONTROL_FRAME( RUBY_VM_NEXT_CONTROL_FRAME( (void *)( c_thread->stack + c_thread->stack_size ) ) );
  
  VALUE  rb_test_index_array  =  rb_ary_new();
  //  :object
  //  instance or class
  rb_ary_push(  rb_test_index_array,
          ID2SYM( rb_intern( "object" ) ) );
  //  :method        
  rb_ary_push(  rb_test_index_array,
          ID2SYM( rb_intern( "method" ) ) );
  //  :file
  rb_ary_push(  rb_test_index_array,
          ID2SYM( rb_intern( "file" ) ) );
  //  :line
  rb_ary_push(  rb_test_index_array,
          ID2SYM( rb_intern( "line" ) ) );
  
  //  only used if c_return_all_frames == TRUE
  VALUE  rb_frame_hashes_array  =  Qnil;
  if ( c_return_all_frames == TRUE )  {
    rb_frame_hashes_array    =  rb_ary_new();
  }
  
  VALUE  rb_frame_hash  =  Qnil;
  
  //  for each control frame:
    while ( c_current_context_frame < c_top_of_control_frame ) {
     
    //  iterate each array member 
    int  c_which_member;
    for ( c_which_member = 0 ; c_which_member < RARRAY_LEN( rb_tracking_array ) ; c_which_member++ )  {
       
      VALUE  rb_this_arg  =  args[ c_which_member + c_args_offset ];
       
      BOOL    matched  =  FALSE;
       
      rb_frame_hash  =  rb_RPRuby_Sender_Kernel_internal_backtraceHashForControlFrame(  & c_current_context_frame );
      
      //  if we have a hash we are testing multiple items in a frame
      if ( TYPE( rb_this_arg ) == T_HASH )  {

        VALUE  rb_frame_test_array  =  rb_obj_clone( rb_test_index_array );
      
        //  for each element that we could test for
        int  c_which_index;
        int  c_skipped_index_count  =  0;
        for ( c_which_index = 0 ; c_which_index < RARRAY_LEN( rb_frame_test_array ) ; c_which_index++ )  {
          
          VALUE  rb_this_index  =  RARRAY_PTR( rb_frame_test_array )[ c_which_index ];
          
          //  see if our requested test hash includes the potential test element
          if ( rb_hash_lookup(  rb_this_arg,
                      rb_this_index ) != Qnil )  {
            
            VALUE  rb_required_element  =  rb_hash_aref(  rb_this_arg,
                                    rb_this_index );
            VALUE  rb_frame_element  =  rb_hash_aref(  rb_frame_hash,
                                    rb_this_index  );
                               
            //  if it does, we need to see if the current frame's element matches this element
            VALUE  rb_required_element_klass;
            if ( rb_required_element == rb_frame_element
              //  if we have a string, which is a filename
              ||  (  TYPE( rb_required_element ) == T_STRING
                 &&  rb_funcall( rb_frame_element, rb_intern( "==" ), 1, rb_required_element ) == Qtrue )
              //  if we have a class, which is a special case for :object
              ||  (  rb_this_index == ID2SYM( rb_intern( "class" ) ) 
                 &&  ( rb_required_element_klass = ( ( TYPE( rb_required_element ) == T_CLASS ) ? rb_required_element : rb_funcall( rb_required_element, rb_intern( "class" ), 0 ) ) )
                 &&  rb_required_element_klass == rb_required_element ) )  {

              rb_ary_delete_at(  rb_frame_test_array,
                        c_which_index );
              c_which_index--;
            }
          }
          else {
            c_skipped_index_count++;
          }

          if ( RARRAY_LEN( rb_frame_test_array ) == c_skipped_index_count )  {
            if ( c_return_frame == TRUE )  {
              return rb_frame_hash;
            }
            else if ( c_return_all_frames == TRUE )  {
              rb_ary_push(  rb_frame_hashes_array,
                      rb_frame_hash );
            }
            else {
              return Qtrue;              
            }
          }          
        }
      }
      else {

        //  :object => <class:instance>
        if  (  TYPE( rb_this_arg ) == T_OBJECT )  {

          if ( rb_hash_aref(  rb_frame_hash,
                    ID2SYM( rb_intern( "object" ) ) ) == rb_this_arg )  {
            matched = TRUE;
          }
        }
        //  :object => <class>
        else if  ( TYPE( rb_this_arg ) == T_CLASS )  {
          
          VALUE  rb_frame_object      =  rb_hash_aref(  rb_frame_hash,
                                    ID2SYM( rb_intern( "object" ) ) );
          VALUE  rb_frame_object_klass  =  TYPE( rb_frame_object ) == T_CLASS ? rb_frame_object : rb_funcall( rb_frame_object, rb_intern( "class" ), 0 );
          if ( rb_frame_object_klass == rb_this_arg )  {
            matched = TRUE;
          }
        }
        //  :method => :method
        else if  ( TYPE( rb_this_arg ) == T_SYMBOL )  {
           
          if ( rb_hash_aref(  rb_frame_hash,
                    ID2SYM( rb_intern( "method" ) ) ) == rb_this_arg )  {
            matched = TRUE;
          }
        }
        //  :file => "filename"
        else if ( TYPE( rb_this_arg ) == T_STRING )  {
          VALUE  rb_filename  =  rb_hash_aref(  rb_frame_hash,
                              ID2SYM( rb_intern( "file" ) ) );
          VALUE  rb_comparison  =  rb_funcall( rb_filename, rb_intern( "==" ), 1, rb_this_arg );
          if ( rb_comparison == Qtrue )  {
            matched = TRUE;
          }
        }
        //  :line => number
        else if ( TYPE( rb_this_arg ) == T_FIXNUM )  {
          if ( rb_hash_aref(  rb_frame_hash,
                    ID2SYM( rb_intern( "line" ) ) ) == rb_this_arg )  {
            matched = TRUE;
          }
        }
         
        //  if array member exists in frame, remove from array
        if ( matched )  {
          if ( c_requires_all_items == FALSE )  {
            if ( c_return_frame == TRUE )  {
              return rb_frame_hash;
            }
            else {
              return Qtrue;              
            }

          }
          else {
            
            //  delete this index
            rb_ary_delete_at(  rb_tracking_array,
                      c_which_member );
            
            //  decrement the loop iterator so that the increase is offset
            //  this is necessary since we just removed an index and are iterating vs. the length of the array
            c_which_member--;
          }
        }
      }
    }
     
    //  if array is empty, return true
    //  we check here as well as at the end so we can stop iterating the backtrace if we find all our items
    if ( RARRAY_LEN( rb_tracking_array ) == 0 )  {
      if ( c_return_frame == TRUE )  {
        return rb_frame_hash;
      }
      else if ( c_return_all_frames == TRUE )  {
        rb_ary_push(  rb_frame_hashes_array,
                rb_frame_hash );
        return rb_frame_hashes_array;
      }
      else {
        return Qtrue;              
      }
    }
    c_current_context_frame = RUBY_VM_PREVIOUS_CONTROL_FRAME( c_current_context_frame );    
  }
  
  if (  c_return_all_frames == TRUE
    &&  RARRAY_LEN( rb_frame_hashes_array ) > 0 ) {
    return rb_frame_hashes_array;
  }
  //  if we finish iterating frames and still have items in the array, return false
  else if ( RARRAY_LEN( rb_tracking_array ) > 0 )  {
    if ( c_return_frame == TRUE )  {
      return Qnil;
    }
    else {
      return Qfalse;
    }
  }
  //  otherwise, return true
  else if ( c_return_frame == TRUE )  {
    return rb_frame_hash;
  }
  else {
    return Qtrue;              
  }
  //  we don't get here
  return Qnil;
}

/***************************************
 *  Kernel.backtrace_includes_one_of?  *
 **************************************/

/*
 * call-seq:
 *   Kernel.backtrace_includes_one_of?( method_or_object, ... ) -> true or false
 *   Kernel.backtrace_includes_one_of?( number_of_frames, method_or_object, ... ) -> true or false
 *
 * Returns whether one of the specified methods or objects or classes are in the current backtrace context.
 * Kernel.backtrace_includes_one_of? begins with the prior frame, so asking if the backtrace includes the current method
 * will only report true if the current method is part of the earlier call chain.
 */
VALUE rb_RPRuby_Sender_Kernel_backtrace_includes_one_of(  int    argc,
                              VALUE*  args,
                              VALUE  rb_self )  {

  //  method implemented as special case of backtrace_includes?
  
  //  create a new array starting with Qtrue and then followed by args
  VALUE    c_methods_array[ argc + 1 ];
  c_methods_array[ 0 ] = Qnil;
  int  c_which_arg;
  for ( c_which_arg = 0 ; c_which_arg < argc ; c_which_arg++ )  {
    c_methods_array[ c_which_arg + 1 ] = args[ c_which_arg ];
  }
  return rb_RPRuby_Sender_Kernel_backtrace_includes(  argc + 1,
                            c_methods_array,
                            rb_self );
}

/*********************************
 *  Kernel.backtrace_frame_with  *
 ********************************/

/*
 * call-seq:
 *   Kernel.backtrace_frame_with( method_or_object, ... ) -> true or false
 *   Kernel.backtrace_frame_with( number_of_frames, method_or_object, ... ) -> true or false
 *
 * Returns first frame matching specifications, which work like Kernel.backtrace_includes?. 
 * Kernel.backtrace_includes_one_of? begins with the prior frame, so asking if the backtrace includes the current method
 * will only report true if the current method is part of the earlier call chain.
 */
VALUE rb_RPRuby_Sender_Kernel_backtrace_frame_with(  int    argc,
                          VALUE*  args,
                          VALUE  rb_self )  {
  
  //  create a new array starting with Qtrue and then followed by args
  VALUE    c_methods_array[ argc + 1 ];
  //  Qfalse tells backtrace_includes to return frame hash instead of true/false
  c_methods_array[ 0 ] = Qfalse;
  int  c_which_arg;
  for ( c_which_arg = 0 ; c_which_arg < argc ; c_which_arg++ )  {
    c_methods_array[ c_which_arg + 1 ] = args[ c_which_arg ];
  }
  return rb_RPRuby_Sender_Kernel_backtrace_includes_one_of(  argc + 1,
                                c_methods_array,
                                rb_self );  
}

/**********************************
 *  Kernel.backtrace_frames_with  *
 *********************************/

/*
 * call-seq:
 *   Kernel.backtrace_frames_with( method_or_object, ... ) -> true or false
 *   Kernel.backtrace_frames_with( number_of_frames, method_or_object, ... ) -> true or false
 *
 * Returns all frames matching specifications, which work like Kernel.backtrace_includes?. 
 * Kernel.backtrace_includes_one_of? begins with the prior frame, so asking if the backtrace includes the current method
 * will only report true if the current method is part of the earlier call chain.
 */
VALUE rb_RPRuby_Sender_Kernel_backtrace_frames_with(  int    argc,
                            VALUE*  args,
                            VALUE  rb_self )  {
  
  
  //  create a new array starting with Qtrue and then followed by args
  VALUE    c_methods_array[ argc + 1 ];
  //  Qtrue tells backtrace_includes to return all frame hashes specified instead of the first
  c_methods_array[ 0 ] = Qtrue;
  int  c_which_arg;
  for ( c_which_arg = 0 ; c_which_arg < argc ; c_which_arg++ )  {
    c_methods_array[ c_which_arg + 1 ] = args[ c_which_arg ];
  }
  return rb_RPRuby_Sender_Kernel_backtrace_includes(  argc + 1,
                            c_methods_array,
                            rb_self );  
}

/***************************************************************************************************************************************************************
****************************************************************************************************************************************************************
                          Internal Methods
****************************************************************************************************************************************************************
***************************************************************************************************************************************************************/

/**********************************
 *  backtraceHashForControlFrame  *
 *********************************/

VALUE rb_RPRuby_Sender_Kernel_internal_backtraceHashForControlFrame(  rb_control_frame_t**    c_current_frame )  {

  const char*    c_method_name    =  NULL;
  int        c_sourcefile_line  =  0;
  
  //  create new hash for this frame
  VALUE  rb_frame_hash      =  rb_hash_new();
    
  VALUE  rb_sourcefile_name    =  Qnil;
  VALUE  rb_sourcefile_line    =  Qnil;
  VALUE  rb_method_name      =  Qnil;
  VALUE  rb_object_for_frame    =  Qnil;
  
  if ( ( *c_current_frame )->iseq != 0 ) {
    
    if ( ( *c_current_frame )->pc != 0 ) {
      
      rb_iseq_t *iseq      = ( *c_current_frame )->iseq;
      
      //  get sourcefile name and set in hash
      rb_sourcefile_name    =  iseq->filename;
      
      //  get sourcefile line and set in hash
      c_sourcefile_line    =  rb_vm_get_sourceline( *c_current_frame );
      rb_sourcefile_line    =  INT2FIX( c_sourcefile_line );
      
      //  get name of instruction sequence
      rb_method_name      =  ID2SYM( rb_intern( StringValuePtr( iseq->name ) ) );        
      rb_object_for_frame  =  ( *c_current_frame )->self;
    }
  }
  else if ( RUBYVM_CFUNC_FRAME_P( *c_current_frame ) ) {
    
    //  get name of method

    #if RUBY_PATCHLEVEL >= -1
    //  For 1.9.2:
    const rb_method_entry_t*  c_method_for_frame  =  ( *c_current_frame )->me;
    c_method_name        =  rb_id2name( c_method_for_frame->called_id );
    #else
    //  For 1.9.1:
    c_method_name        =  rb_id2name( ( *c_current_frame )->method_id );
    #endif
    
    rb_method_name        =  ( c_method_name == NULL ? Qnil : ID2SYM( rb_intern( c_method_name ) ) );
    rb_object_for_frame  =  ( *c_current_frame )->self;    
  }
  //  we have to test this case - it works for blocks but there may be other cases too
  else if (  ( *c_current_frame )->block_iseq != 0
       &&  ( *c_current_frame )->pc == 0)  {
  
    //  If we got here we have a fiber
    //  There doesn't seem to be much that we can tell about a fiber's context
      
    VALUE      rb_current_fiber  =  rb_fiber_current();
    rb_fiber_t*    c_current_fiber    =  NULL;

    GetFiberPtr(  rb_current_fiber, 
            c_current_fiber);
            
    rb_context_t*  c_context      =  & c_current_fiber->cont;
    
//    rb_block_t*  c_blockptr = RUBY_VM_GET_BLOCK_PTR_IN_CFP( *c_current_frame );
     
    rb_object_for_frame  =  ( *c_current_frame )->self;
    
    //  get sourcefile name and set in hash
    rb_sourcefile_name    =  Qnil;

    //  get sourcefile line and set in hash
    rb_sourcefile_line    =  Qnil;
    
    //  get name of instruction sequence
    rb_method_name      =  rb_str_new2( "<Fiber>" );    
    
    //  if we have a fiber we also include its ruby reference since we have so little other context
    rb_hash_aset(  rb_frame_hash,
            ID2SYM( rb_intern( "fiber" ) ),
            c_context->self );
    
    //  The one time that we know a fiber is in use in the Ruby base is with Enumerators
    //  For now we will handle that with a special case
    
    VALUE  rb_enumerator_class  =  rb_const_get(  rb_cObject,
                            rb_intern( "Enumerator" ) );
    
    VALUE  rb_object_for_frame_klass  =  ( ( TYPE( rb_object_for_frame ) == T_CLASS ) ? rb_object_for_frame : rb_funcall( rb_object_for_frame, rb_intern( "class" ), 0 ) );

    VALUE  rb_ancestors  =  rb_funcall(  rb_object_for_frame_klass,
                        rb_intern( "ancestors" ),
                        0 );
    
    if ( rb_ary_includes(  rb_ancestors,
                rb_enumerator_class ) )  {
      
      struct enumerator* c_enumerator    =  enumerator_ptr( rb_object_for_frame );
      
      rb_object_for_frame  =  c_enumerator->obj;
      rb_method_name    =  ID2SYM( c_enumerator->meth );      
    }
    
  }
  else if (  ( *c_current_frame )->block_iseq == 0
       &&  ( *c_current_frame )->pc == 0)  {
    //  this happens after we had a fiber and we try to go up - which doesn't make sense with a fiber
    //  not sure what we want to do here, if anything
    return Qnil;
  }
  else {
    //  The third possibility is that we have an iseq frame with nil params for what we want
    //  In that case we can simply return the next frame
    *c_current_frame  =  RUBY_VM_PREVIOUS_CONTROL_FRAME( *c_current_frame );
    
    //  in theory this could crash because we are going forward a frame when we don't know what's there
    //  in practice I think we are ok, since we are only jumping forward from nil frames which should never be at the end
    //  at least - I don't think they should... we shall see.
    //  
    //  a fix would be to check the next frame, but that requires access to the thread or the limit cfp, 
    //  which requires passing more context; so for now, I'm leaving it there
    
    return rb_RPRuby_Sender_Kernel_internal_backtraceHashForControlFrame( c_current_frame );
  }

  //  Push values to return hash

  rb_hash_aset(  rb_frame_hash,
          ID2SYM( rb_intern( "object" ) ),
          rb_object_for_frame );
          
  rb_hash_aset(  rb_frame_hash,
          ID2SYM( rb_intern( "file" ) ),
          rb_sourcefile_name );

  rb_hash_aset(  rb_frame_hash,
          ID2SYM( rb_intern( "line" ) ),
          rb_sourcefile_line );

  rb_hash_aset(  rb_frame_hash,
          ID2SYM( rb_intern( "method" ) ),
          rb_method_name );
  
  return rb_frame_hash;
}
