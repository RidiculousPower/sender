#ifndef RP_SENDER_RUBY_SOURCE_SUPPORT
	#define RP_SENDER_RUBY_SOURCE_SUPPORT

	#include "ruby.h"
	#include "eval_intern.h"
	#include "version.h"
	#if RUBY_PATCHLEVEL == -1
		#include "vm_core.h"
	#endif
	#include "method.h"

	#define MAX_POSBUF 128

	enum context_type {
		CONTINUATION_CONTEXT = 0,
		FIBER_CONTEXT = 1,
		ROOT_FIBER_CONTEXT = 2
	};

	typedef struct rb_context_struct {
		enum context_type type;
		VALUE self;
		int argc;
		VALUE value;
		VALUE *vm_stack;
	#ifdef CAPTURE_JUST_VALID_VM_STACK
		int vm_stack_slen;  /* length of stack (head of th->stack) */
		int vm_stack_clen;  /* length of control frames (tail of th->stack) */
	#endif
		VALUE *machine_stack;
		VALUE *machine_stack_src;
	#ifdef __ia64
		VALUE *machine_register_stack;
		VALUE *machine_register_stack_src;
		int machine_register_stack_size;
	#endif
		rb_thread_t saved_thread;
		rb_jmpbuf_t jmpbuf;
		int machine_stack_size;
	} rb_context_t;

	enum fiber_status {
		CREATED,
		RUNNING,
		TERMINATED
	};

	typedef struct rb_fiber_struct {
		rb_context_t cont;
		VALUE prev;
		enum fiber_status status;
		struct rb_fiber_struct *prev_fiber;
		struct rb_fiber_struct *next_fiber;
	} rb_fiber_t;

	struct enumerator {
		VALUE obj;
		ID    meth;
		VALUE args;
		VALUE fib;
		VALUE dst;
		VALUE no_next;
	};

	#define GetFiberPtr(obj, ptr)  do {\
				ptr = (rb_fiber_t*)DATA_PTR(obj);\
				if (!ptr) rb_raise(rb_eFiberError, "uninitialized fiber");\
			} while(0)


	ID frame_func_id( rb_control_frame_t *cfp );
	ID rb_frame_caller(void);
	int rb_vm_get_sourceline(const rb_control_frame_t *cfp);
	void control_frame_dump(rb_thread_t *th, rb_control_frame_t *cfp);
	struct enumerator* enumerator_ptr(VALUE obj);
	
#endif