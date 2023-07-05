#include <stdio.h>
#include <string.h>
#include "chunk.h"

chunk populateChunk( unsigned short rawData )
{
   chunk newChunk;

   // Initialize all bits to 0
   memset( &newChunk, 0, sizeof( chunk ) );

   // Fill in the data bits
   newChunk.dataBitsThree = ( rawData >> ( RAW_CHUNK_SIZE_BITS - 1 ) ) & 0x1;
   newChunk.dataBitsFiveToSeven = ( rawData >> ( RAW_CHUNK_SIZE_BITS - 4 ) ) & 0x7;
   newChunk.dataBitsNineToFifteen = rawData & 0x7F;

   unsigned short xorResult = xorChunk( newChunk );

   // Set parity bits
   newChunk.parityBitsOneToTwo |= ( xorResult & 1 ) << 1;
   newChunk.parityBitsOneToTwo |= ( xorResult >> 1 ) & 1;
   newChunk.parityBitsFour = ( xorResult >> 2 ) & 1;
   newChunk.parityBitsEight = ( xorResult >> 3 ) & 1;

   // Save the parity of the entire chunk into bit 0
   unsigned short overallParity = 0;
   unsigned short chunkData = chunkToUnsignedShort( newChunk );
   for ( int i = 0; i < CHUNK_SIZE_BITS; i++ )
   {
      if ( chunkData & ( 0x8000 >> i ) )
      {
         overallParity ^= 1;
      }
   }

   newChunk.parityBitsZero = overallParity ? 1 : 0;

   return newChunk;
}

unsigned short chunkToUnsignedShort( const chunk c )
{
   unsigned short result = 0;

   // Copy parity bits
   result |= c.parityBitsZero << ( CHUNK_SIZE_BITS - 1 );
   result |= c.parityBitsOneToTwo << ( CHUNK_SIZE_BITS - 3 );
   result |= c.parityBitsFour << ( CHUNK_SIZE_BITS - 5 );
   result |= c.parityBitsEight << ( CHUNK_SIZE_BITS - 9 );

   // Copy data bits
   result |= c.dataBitsThree << ( CHUNK_SIZE_BITS - 4 );
   result |= c.dataBitsFiveToSeven << ( CHUNK_SIZE_BITS - 8 );
   result |= c.dataBitsNineToFifteen;

   return result;
}

unsigned int xorChunk( const chunk chunk )
{
   // Copy the chunk with data bits into an unsigned short so it can be used with bitwise operators
   unsigned short chunkData = chunkToUnsignedShort( chunk );

   // Iterate through bits, XORing every activated bit position
   unsigned int xorResult = 0;
   for ( int bitIndex = 0; bitIndex < CHUNK_SIZE_BITS; bitIndex++ )
   {
      if ( chunkData & ( 0x8000 >> bitIndex ) )
      {
         xorResult ^= bitIndex;
      }
   }
   return xorResult;
}

void printBinary( unsigned int num, int bits )
{
    if ( bits > 0 )
    {
        printBinary( num >> 1, bits - 1 );
        printf( "%d", num & 1 );
    }
}

void printChunk( const chunk c )
{
   printBinary( c.parityBitsZero, 1 );

   printBinary( c.parityBitsOneToTwo, 2 );

   printBinary( c.dataBitsThree, 1 );
   printf( "\n" );

   printBinary( c.parityBitsFour, 1 );

   printBinary( c.dataBitsFiveToSeven, 3 );
   printf( "\n" );

   printBinary( c.parityBitsEight, 1 );

   printBinary( ( c.dataBitsNineToFifteen & 0x70 ) >> 4, 3 );
   printf( "\n" );

   printBinary( c.dataBitsNineToFifteen & 0xF, 4 );
   printf( "\n" );
}