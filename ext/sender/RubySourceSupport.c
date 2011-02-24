
#include "RubySourceSupport.h"

#include "rb_Kernel.h"
#include "RPSender_internal.h"

#include "iseq.h"

//  Taken from eval.c in Ruby source
//  No header, so easiest way to integrate was to copy the code and make my own header.
//  Previously declared static; otherwise unchanged

int rb_vm_get_sourceline(const rb_control_frame_t *cfp)
{
    int line_no = 0;
    const rb_iseq_t *iseq = cfp->iseq;
  
    if (RUBY_VM_NORMAL_ISEQ_P(iseq)) {
    rb_num_t i;
    size_t pos = cfp->pc - cfp->iseq->iseq_encoded;
    
    for (i = 0; i < iseq->insn_info_size; i++) {
      if (iseq->insn_info_table[i].position == pos) {
        if (i == 0) goto found;
        line_no = iseq->insn_info_table[i - 1].line_no;
        goto found;
      }
    }
    line_no = iseq->insn_info_table[i - 1].line_no;
    }
found:
    return line_no;
}

struct enumerator* enumerator_ptr(VALUE obj)
{
    struct enumerator *ptr;
  
    Data_Get_Struct(obj, struct enumerator, ptr);
  
  //  modified because a call to a local version of enumerator_mark won't verify
  
    if (!ptr || ptr->obj == Qundef) {
    rb_raise(rb_eArgError, "uninitialized enumerator");
    }
    return ptr;
}
