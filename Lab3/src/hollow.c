#include <stdio.h>
#include "platform.h"
#include "xil_io.h"
#include "xil_types.h"
#include "xparameters.h"
#include "xil_exception.h"
#include "xscugic.h"
#include "myip.h"
#include "sleep.h"


XScuGic InterruptController; /* Instance of the Interrupt Controller */
static XScuGic_Config *GicConfig;/* The configuration parameters of the controller */

//int XPAR_FABRIC_MYIP_0_INTR_INTR = 61;//######interrupt number####################(vivado2017.4此行可註解)

void INTERRUPT_Handler0(void *baseaddr_p){
	int i;
	print("Push Button was pressed\r\n");
	//xil_printf("Push Button was pressed\r\n");
	for(i=0;i<2;i++){
	  MYIP_mWriteReg(0x43C00000, MYIP_S00_AXI_SLV_REG0_OFFSET, 0xf0);
	  usleep(500000);
	  MYIP_mWriteReg(0x43C00000, MYIP_S00_AXI_SLV_REG0_OFFSET, 0x0f);
	  usleep(500000);
	}
}


int ScuGicInterrupt_Init(void){
	int Status;
	/*
	 * Initialize the interrupt controller driver so that it is ready to
	 * use.
	 * */
	Xil_ExceptionInit();

	GicConfig = XScuGic_LookupConfig(XPAR_PS7_SCUGIC_0_DEVICE_ID);
	if (NULL == GicConfig) {
	  return XST_FAILURE;
	}

	Status = XScuGic_CfgInitialize(&InterruptController, GicConfig,
	GicConfig->CpuBaseAddress);

	if (Status != XST_SUCCESS) {
	  return XST_FAILURE;
}

/*
 * Setup the Interrupt System
 * */

/*
 * Connect the interrupt controller interrupt handler to the hardware
 * interrupt handling logic in the ARM processor.
 */
Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT,
(Xil_ExceptionHandler) XScuGic_InterruptHandler,
(void *) &InterruptController);

/*
 * Connect a device driver handler that will be called when an
 * interrupt for the device occurs, the device driver handler performs
 * the specific interrupt processing for the device
 * since the xparameters.h file doesnt detect the external interrupts, we have to manually
 * use the IRQ_F2P port numbers ; 91, 90, ect..
 */

Status = XScuGic_Connect(&InterruptController,XPAR_FABRIC_MYIP_0_INTR_INTR,
(Xil_ExceptionHandler)INTERRUPT_Handler0,
(void *)&InterruptController);

XScuGic_Enable(&InterruptController, XPAR_FABRIC_MYIP_0_INTR_INTR);
/*
 * Enable interrupts in the ARM
 */
Xil_ExceptionEnable();
/*
 * Set interrupts PriorityTriggerType
 */
XScuGic_SetPriorityTriggerType(&InterruptController, XPAR_FABRIC_MYIP_0_INTR_INTR,
0xa0, 3);

if (Status != XST_SUCCESS)
return XST_FAILURE;
}

int main(){
	int xstatus;

	// Interrupt Test

	xstatus = ScuGicInterrupt_Init();

	if (xstatus != XST_SUCCESS)
	  return XST_FAILURE;

	// Wait For interrupt;

	print("Wait for the interrupt to trigger \r\n");
	print("########################################\r\n");
	print(" \r\n");

	int i=0;

	while(1){
	  for(i=0;i<=255;i++){
	    MYIP_mWriteReg(0x43C00000, MYIP_S00_AXI_SLV_REG0_OFFSET, i);
	    printf("i = %d \r\n",i);
	    sleep(1);
	  }
	}
	return 0;
}
