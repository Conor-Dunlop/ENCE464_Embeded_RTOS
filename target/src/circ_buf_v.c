// *******************************************************
// 
// circ_buf_v.c
//
// Support for a circular buffer of vector3 (3*int16_t) values on the
//  Tiva processor.
// P.J. Bones UCECE, Tim Preston-Marshall
// Last modified:  3.5.2022
// 
// FitnessThur9-1
//
// *******************************************************

#include <stdint.h>
#include "stdlib.h"
#include "circ_buf_v.h"

// *******************************************************
// Buffer structure
struct circ_buf_vec_t {
	uint32_t size;		// Number of entries in buffer
	uint32_t windex;	// index for writing, mod(size)
	uint32_t rindex;	// index for reading, mod(size)
	vector3_t *data;	// pointer to the data
};

// *******************************************************
// initCircBuf: Initialise the circBuf instance. Reset both indices to
// the start of the buffer.  Dynamically allocate and clear the the 
// memory and return a pointer for the data.  Return NULL if 
// allocation fails.
circ_buf_vec_t* initVecCircBuf (uint32_t size)
{
	circ_buf_vec_t* buffer = (circ_buf_vec_t*)malloc(sizeof(circ_buf_vec_t));
	buffer->windex = 0;
	buffer->rindex = 0;
	buffer->size = size;
	buffer->data = (vector3_t*) calloc (size, sizeof(vector3_t));

	if 	(buffer->data == NULL) {
		free(buffer);
		buffer = NULL;
	}

	return buffer;
}
   // Note use of calloc() to clear contents.

// *******************************************************
// writeCircBuf: insert entry at the current windex location,
// advance windex, modulo (buffer size).
void writeVecCircBuf (circ_buf_vec_t* buffer, vector3_t entry)
{
	buffer->data[buffer->windex] = entry;
	buffer->windex++;
	if (buffer->windex >= buffer->size)
	   buffer->windex = 0;
}

// *******************************************************
// readCircBuf: return entry at the current rindex location,
// advance rindex, modulo (buffer size). The function deos not check
// if reading has advanced ahead of writing.
vector3_t readVecCircBuf (circ_buf_vec_t* buffer)
{
    vector3_t entry;
	
	entry = buffer->data[buffer->rindex];
	buffer->rindex++;
	if (buffer->rindex >= buffer->size)
	   buffer->rindex = 0;
    return entry;
}

// *******************************************************
// freeCircBuf: Releases the memory allocated to the buffer data,
// sets pointer to NULL and ohter fields to 0. The buffer can
// re-initialised by another call to initCircBuf().
void freeVecCircBuf (circ_buf_vec_t* buffer)
{
	buffer->windex = 0;
	buffer->rindex = 0;
	buffer->size = 0;
	free (buffer->data);
	buffer->data = NULL;
	free (buffer);
}

