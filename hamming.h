#ifndef HAMMING_H
#define HAMMING_H

#define CHUNKS_IN_BUFFER 2048
#define READ_SIZE 512
#define INPUT_BUFFER_LENGTH 128

#define BITS_PER_BYTE 8

int writeToFile( const char *fileName, const char *buffer, size_t numBytes );
int encode( const char *fileName );
int decode( const char *fileName );

#endif