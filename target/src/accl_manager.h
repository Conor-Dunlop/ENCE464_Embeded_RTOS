/*
 * accl_manager.h
 *
 *  Created on: 23/03/2022
 *      Authors: Matthew Suter
 *
 */

#ifndef ACCL_MANAGER_H_
#define ACCL_MANAGER_H_

#define BUF_SIZE 20 // WARNING: If this is set too high, we run out of heap space and the z-buffer gets garbled data

void acclInit(void);        // set up library
void acclProcess(void);     // Run periodically
uint16_t acclMean(void);   // Return mean acceleration, calculated using circular buffers


#endif /* ACCL_MANAGER_H_ */
