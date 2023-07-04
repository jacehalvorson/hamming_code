#ifndef HAMMING_H
#define HAMMING_H

#define RAW_CHUNK_SIZE_BITS 11
#define RAW_CHUNK_BIT_MASK 0x7FF
#define CHUNK_SIZE_BITS 16
#define PARITY_BIT_COUNT 4
#define CHUNKS_IN_BUFFER 16
#define READ_SIZE 512

#define BITS_PER_BYTE 8

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

chunk populateChunk( unsigned int rawDataPtr );

int encode( char *fileName );

#endif