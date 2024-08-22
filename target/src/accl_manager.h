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
#define ACCL_SENSITIVITY 5

void acclInit(void);        // set up library
void acclProcess(void);     // Run periodically


#endif /* ACCL_MANAGER_H_ */
