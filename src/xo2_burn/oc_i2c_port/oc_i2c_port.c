

#include "oc_i2c_port.h"
#include "delay.h"
#include "../XO2_ECA/XO2_api.h"


int oc_i2c_start(void *pDrvrParams)
{
    return(OK);
}




/**
 * Call OpenCores routine to issue an I2C Stop bus action.
 * @param pDrvrParams pointer that holds specific information for the I2C driver routine, previously stored in XO2Handle_t.
 * @return always returns OK
 */
int oc_i2c_stop(void *pDrvrParams)
{
	return(OK);  
}


int oc_i2c_read(void *pDrvrParams, unsigned int len, unsigned char *pBuf)
{
	int status;
	OC_I2C_t *pI2C;

	pI2C = (OC_I2C_t *)pDrvrParams;
	unsigned int baseaddr = pI2C->i2c_ctx->baseAddr;

	status = I2CMasterRead(baseaddr, pI2C->slaveAddr,
					pBuf,len);
					
	if (status == FAIL)
		return(ERROR);
	else
		return(OK);
}


int oc_i2c_write(void *pDrvrParams, unsigned int len, unsigned char *pBuf)
{
	int status;
	OC_I2C_t *pI2C;

	pI2C = (OC_I2C_t  *)pDrvrParams;
    unsigned int baseaddr = pI2C->i2c_ctx->baseAddr;
	status = I2CMasterWrite(baseaddr, 
					pI2C->slaveAddr, 
				    pBuf,
					len
					);
	if (status == FAIL)
		return(ERROR);
	else
		return(OK);
}


const ECADrvrCalls_t i2c_drvr = {
   oc_i2c_start,
   oc_i2c_stop,
   oc_i2c_read,
   oc_i2c_write,
   delay,
   delayus,
} ;
