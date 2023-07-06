#ifndef HAMMING_H
#define HAMMING_H

#include "chunk.h"

#define CHUNKS_IN_BUFFER 176
#define INPUT_BUFFER_LENGTH 128

#define BITS_PER_BYTE 8

int encode( const char *fileName );
int decode( const char *fileName );
void populateChunkArray( chunk *chunks, const int chunkCount, const char *buffer, const int bytesRead );
int checkFileName( const char *fileName );
int readFromFile( const int fd, char *buffer, int maxNumBytes );
int writeToFile( const int fd, const char *buffer, int numBytes );

#endif