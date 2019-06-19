#include <stdio.h>
#include "platform.h"
#include "xil_io.h"
#include "xil_types.h"
#include "xparameters.h"
#include "xil_exception.h"
#include "xscugic.h"
#include "myip.h"
#include "sleep.h"


int main()
{
    init_platform();

    print("Hello World\n\r");
    MYIP_mWriteReg(0x43C00000, MYIP_S00_AXI_SLV_REG0_OFFSET, 63);
    MYIP_mWriteReg(0x43C00000, MYIP_S00_AXI_SLV_REG1_OFFSET, 15);

    cleanup_platform();
    return 0;
}
