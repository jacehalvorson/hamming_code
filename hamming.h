#ifndef HAMMING_H
#define HAMMING_H

#define RAW_CHUNK_SIZE_BITS 11
#define CHUNK_SIZE_BITS 16
#define CHUNKS_IN_BUFFER 64
#define READ_SIZE 512

typedef struct chunk {
   unsigned int parityZeroToTwo : 3;
   unsigned int dataBitsThree : 1;
   unsigned int parityFour : 1;
   unsigned int dataBitsFiveToSeven : 3;
   unsigned int parityEight : 1;
   unsigned int dataBitsNineToFifteen : 7;
} chunk;

typedef struct rawChunkArray {
   unsigned int rawChunk : 11;
} rawChunkArray;

chunk *populateChunk( unsigned int *rawDataPtr );

int encode( char *fileName );

#endif