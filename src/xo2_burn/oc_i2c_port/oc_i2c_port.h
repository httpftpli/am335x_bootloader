/*
 *  COPYRIGHT (c) 2012 by Lattice Semiconductor Corporation
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

/** @file oc_i2c_port.h 
 * OpenCores I2C Master Controller Access Routines.
 * This file provides function prototypes for the OpenCores I2C wrapper routines,
 * and a definition of the structure to be passed to the routines that holds
 * specific parameters needed by the OpenCores I2C routines, such as
 * device context and slave address.  @see OC_I2C_t
 */
 
#ifndef LATTICE_OC_I2C_PORT_H
#define LATTICE_OC_I2C_PORT_H

#include "type.h"
#include "pf_hsi2c.h"
#include "module.h"
#include "../XO2_ECA/ECA_drvr.h"

/**
 * Structure to hold device access specific information when using
 * the MSB OpenCores I2C drivers.  An instance of this structure needs
 * to exist for each XO2 I2C device that will be accessed.  The 7 bit I2C address
 * of the specific XO2 is stored here, along with the reference to the specific OpenCores
 * I2C controller to use for the access.
 * The reference to this structure is held in the pDrvrParams in XO2Handle_t.
 * 
 */
typedef struct
{
	unsigned int		slaveAddr;   /**< 7 bit I2C address of XO2 device */
	MODULE *i2c_ctx;  /**< specific I2C controller instance */
} OC_I2C_t;


////////////////////////////////////////////////////////////////////////////////
// FUNCTION PROTOTYPES
////////////////////////////////////////////////////////////////////////////////
int oc_i2c_start(void *pDrvrParams);
int oc_i2c_stop(void *pDrvrParams);
int oc_i2c_read(void *pDrvrParams, unsigned int len, unsigned char *pBuf);
int oc_i2c_write(void *pDrvrParams, unsigned int len, unsigned char *pBuf);

extern const ECADrvrCalls_t i2c_drvr;

#endif

