#ifndef HAMMING_H
#define HAMMING_H

#define RAW_CHUNK_SIZE_BITS 11
#define RAW_CHUNK_BIT_MASK 0x7FF
#define CHUNK_SIZE_BITS 16
#define PARITY_BIT_COUNT 4
#define CHUNKS_IN_BUFFER 16
#define READ_SIZE 512

#define BITS_PER_BYTE 8

// Bit 0 contains the parity for the entire chunk.
// Bit 1 contains the parity for all odd bit positions (0bxxx1).
// Bit 2 contains the parity for all even bit positions (0bxx1x).
// Bit 4 contains the parity for bit positions with 0bx1xx pattern.
// Bit 8 contains the parity for bit positions with 0b1xxx pattern.
typedef struct chunk {
   unsigned short parityBitsZero : 1;
   unsigned short parityBitsOneToTwo : 2;
   unsigned short dataBitsThree : 1;
   unsigned short parityBitsFour : 1;
   unsigned short dataBitsFiveToSeven : 3;
   unsigned short parityBitsEight : 1;
   unsigned short dataBitsNineToFifteen : 7;
} chunk;

unsigned short chunkToUnsignedShort( const chunk *c );
unsigned int xorChunk( chunk *chunk );
chunk populateChunk( unsigned int rawDataPtr );

int encode( char *fileName );

#endif