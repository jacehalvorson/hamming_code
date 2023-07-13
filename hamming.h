#ifndef HAMMING_H
#define HAMMING_H

#include "chunk.h"

#define CHUNKS_IN_BUFFER 176
#define BYTES_IN_BUFFER ( CHUNKS_IN_BUFFER * RAW_CHUNK_SIZE_BITS / BITS_PER_BYTE )
#define INPUT_BUFFER_LENGTH 128

#define BITS_PER_BYTE 8

typedef struct header {
   int originalFileSize;
} header;

int encode( const char *fileName );
int decode( const char *fileName );
void populateChunkArray( chunk *chunks, const int chunkCount, const char *buffer, const int bytesRead );

#endif