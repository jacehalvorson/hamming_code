#ifndef CHUNK_H
#define CHUNK_H

// Constants
#define RAW_CHUNK_SIZE_BITS 11
#define CHUNK_SIZE_BITS 16

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

// Takes in 11 bits of data and returns a chunk with the parity bits set.
chunk populateChunk( unsigned short rawData );
unsigned short decodeChunk( const chunk chunk );

// Helper functions
unsigned short chunkToUnsignedShort( const chunk chunk );
unsigned int xorChunk( const chunk chunk );
unsigned int getOverallParity( const chunk chunk );

// Print utility functions
void printBinary( unsigned int num, int bits );
void printChunk( const chunk c );

#endif