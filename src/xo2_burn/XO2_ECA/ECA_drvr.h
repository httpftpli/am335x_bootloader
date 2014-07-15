
#ifndef LATTICE_ECA_DRVR_H
#define LATTICE_ECA_DRVR_H



typedef struct
{
	/** Pointer to function to start the transfer of a byte(s) on the interface. */
	int     (*pfCmdStart)(void *pDrvrParams); 
	
	/** Pointer to function to end the transfer of a byte(s) on the interface. */
	int     (*pfCmdStop)(void *pDrvrParams); 
	

	/** Pointer to function to Read a number of bytes, on the interface, from the XO2. */
	int     (*pfCmdRead)(void *pDrvrParams,
			 	unsigned int len,
			 	unsigned char *buf
				);
       
	/** Pointer to function to Write a number of bytes, on the interface, to the XO2. */
	int     (*pfCmdWrite)(void *pDrvrParams, 
				unsigned int len, 
				unsigned char *buf); 

	/** Pointer to function to implement a milliSec delay function on this platform. */
	void	(*pfmSecDelay)(unsigned int msec); 

	/** Pointer to function to implement a microSec delay function on this platform. */
	void	(*pfuSecDelay)(unsigned int usec);
} ECADrvrCalls_t;





#endif

