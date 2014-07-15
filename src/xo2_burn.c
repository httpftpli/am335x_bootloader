
#include "xo2_burn/XO2_ECA/XO2_api.h"
#include "xo2_burn\oc_i2c_port\oc_i2c_port.h"

void xo2BurnInit(){
	int designNum;
	int status;
	char selection;
	int display;
	int done;
	XO2RegInfo_t regInfo;
	unsigned int statusReg;


	XO2_JEDEC_t *pCfgBits;  // pointer to the JEDEC confgiruation data to load into XO2

	// Declare the handle to the XO2 device that we will be accessing
	XO2Handle_t xo2_dev;

	// Declare holder for the OpenCores I2C driver parameters needed by the OpenCores drivers
	// that need to be passed along with each call from an XO2 command routine.
	OC_I2C_t oc_i2c;

	//--------------------------------------------------------------
	// First: 
	// Initialize  oc_i2c
	//--------------------------------------------------------------
	oc_i2c.slaveAddr =  0x40;   // blank XO2 default I2C address (7 bit world), becomes 0x80
	oc_i2c.i2c_ctx = modulelist+MODULE_ID_I2C0;   // save I2C controller context for use in driver
    
	//--------------------------------------------------------------
	// Third: 
	// Initialize the XO2 ECA handle with the I2C driver fucntions
	// and other settings for the specific XO2 to access.
	//--------------------------------------------------------------
	xo2_dev.devType = MachXO2_1200;   // must match the real device
	xo2_dev.cfgEn = 0;  // initial value - config not enabled at start (could also be done with an API init call)
	xo2_dev.pDrvrCalls = (ECADrvrCalls_t*)&i2c_drvr;   // driver routines to start/stop read/write over I2C
	xo2_dev.pDrvrParams = (void *)&oc_i2c;   // extra info to pass onto the driver routines
	
	//--------------------------------------------------------------
	// Fourth: 
	// Invoke XO2 ECA APIs via menu selections.
	//--------------------------------------------------------------
	status = XO2ECA_apiGetHdwInfo(&xo2_dev, &regInfo);
    if (status==OK) {
       regInfo.devID++;
    }
}
