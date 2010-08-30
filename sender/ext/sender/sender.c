
#include "rb_Global.h"
#include "rb_Kernel.h"

/*************
*	 Init	 *
*************/

//	Called from ruby when RPBDB module is initialized
void Init_sender()	{

	Init_senderGlobal();
	Init_senderKernel();
	
}

