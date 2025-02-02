/*
 * circ_buf_t.h
 *
 *  Created on: 08/03/2017
 *      Authors: P.J. Bones UCECE
 *
 *  Support for a circular buffer of uint32_t values on the 
 *  Tiva processor.
 *
 */

#ifndef circ_buf_t_H_
#define circ_buf_t_H_

#include <stdint.h>
#include "stdlib.h"

typedef struct circBuf_t circBuf_t;

// *******************************************************
// initCircBuf: Initialise the circBuf instance. Reset both indices to
// the start of the buffer.  Dynamically allocate and clear the the 
// memory and return a pointer for the data.  Return NULL if 
// allocation fails.
circBuf_t *
initCircBuf (uint32_t size);

// *******************************************************
// writeCircBuf: insert entry at the current windex location,
// advance windex, modulo (buffer size).
void
writeCircBuf (circBuf_t *buffer, uint32_t entry);

// *******************************************************
// readCircBuf: return entry at the current rindex location,
// advance rindex, modulo (buffer size). The function deos not check
// if reading has advanced ahead of writing.
uint32_t
readCircBuf (circBuf_t *buffer);

// *******************************************************
// freeCircBuf: Releases the memory allocated to the buffer data,
// sets pointer to NULL and other fields to 0. The buffer can
// re initialised by another call to initCircBuf().
void
freeCircBuf (circBuf_t *buffer);

#endif /*circ_buf_t_H_*/
