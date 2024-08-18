/*
 * circ_buf_v.h
 *
 *  Created on: 03/05/2022
 *      Authors: P.J. Bones UCECE, Tim Preston-Marshall
 *
 *  Support for a circular buffer of vector3 (3*int16_t) values on the
 *  Tiva processor.
 *
 */

#ifndef circ_buf_t_H_
#define circ_buf_t_H_

#include <stdint.h>
#include "stdlib.h"


typedef struct{
    int16_t x;
    int16_t y;
    int16_t z;
} vector3_t;

typedef struct circ_buf_vec_t circ_buf_vec_t;

// *******************************************************
// initCircBuf: Initialise the circBuf instance. Reset both indices to
// the start of the buffer.  Dynamically allocate and clear the the 
// memory and return a pointer for the data.  Return NULL if 
// allocation fails.
circ_buf_vec_t* initVecCircBuf (uint32_t size);

// *******************************************************
// writeCircBuf: insert entry at the current windex location,
// advance windex, modulo (buffer size).
void writeVecCircBuf (circ_buf_vec_t *buffer, vector3_t entry);

// *******************************************************
// readCircBuf: return entry at the current rindex location,
// advance rindex, modulo (buffer size). The function deos not check
// if reading has advanced ahead of writing.
vector3_t readVecCircBuf (circ_buf_vec_t *buffer);

// *******************************************************
// freeCircBuf: Releases the memory allocated to the buffer data,
// sets pointer to NULL and other fields to 0. The buffer can
// re initialised by another call to initCircBuf().
void freeVecCircBuf (circ_buf_vec_t *buffer);

#endif /*circ_buf_t_H_*/
