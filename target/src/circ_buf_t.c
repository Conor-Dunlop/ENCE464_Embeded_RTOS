/*
 * circ_buf_t.c
 *
 *  Created on: 08/03/2017
 *      Authors: P.J. Bones UCECE
 *
 *  Support for a circular buffer of uint32_t values on the 
 *  Tiva processor.
 *
 */

#include "circ_buf_t.h"

// *******************************************************
// Buffer structure

struct circBuf_t {
	uint32_t size;		// Number of entries in buffer
	uint32_t windex;	// index for writing, mod(size)
	uint32_t rindex;	// index for reading, mod(size)
	uint32_t *data;		// pointer to the data
};

// *******************************************************
// initCircBuf: Initialise the circBuf instance. Reset both indices to
// the start of the buffer.  Dynamically allocate and clear the the 
// memory and return a pointer for the data.  Return NULL if 
// allocation fails.
circBuf_t* initCircBuf (uint32_t size)
{
	circBuf_t* buffer = (circBuf_t*)malloc(sizeof(circBuf_t));
	buffer->windex = 0;
	buffer->rindex = 0;
	buffer->size = size;
	buffer->data = (uint32_t*) calloc (size, sizeof(uint32_t));

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
void writeCircBuf (circBuf_t *buffer, uint32_t entry)
{
	buffer->data[buffer->windex] = entry;
	buffer->windex++;
	if (buffer->windex >= buffer->size)
	   buffer->windex = 0;
}

// *******************************************************
// readCircBuf: return entry at the current rindex location,
// advance rindex, modulo (buffer size). The function checks for 
// unwritten data to error if rindex has not overtaken windex.
uint32_t readCircBuf (circBuf_t *buffer)
{
	uint32_t entry;
	if (buffer->data != 0) {
		entry = buffer->data[buffer->rindex];
		buffer->rindex++;
		if (buffer->rindex >= buffer->size)
			buffer->rindex = 0;
		return entry;
	} else {
		return 0;
	}
	
}

// *******************************************************
// freeCircBuf: Releases the memory allocated to the buffer data,
// sets pointer to NULL and other fields to 0. The buffer can
// re-initialised by another call to initCircBuf().
void freeCircBuf (circBuf_t * buffer)
{
	buffer->windex = 0;
	buffer->rindex = 0;
	buffer->size = 0;
	free (buffer->data);
	buffer->data = NULL;
	//free(buffer); memory leak go brrrrrr
}

