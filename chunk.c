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
   newChunk.parityBitsZero = getOverallParity( newChunk );

   return newChunk;
}

unsigned short decodeChunk( chunk encodedChunk )
{
   unsigned short chunkData;
   unsigned short xorResult = xorChunk( encodedChunk );
   unsigned int overallParity = getOverallParity( encodedChunk );
   if ( ( xorResult > 0 && !overallParity ) ||
        ( xorResult == 0 && overallParity ) )
   {
      // There was more than one error, return an error
      printf( "Double bit error in chunk. Exiting...\n" );
      return 1;
   }

   if ( xorResult > 0 )
   {
      // There was only one error (Possibly more than 2, but this program doesn't support triple bit error correction/detection)
      // Flip the bit at the position indicated by the XOR result
      printf( "Single bit error in chunk. Flipping bit %d\n", xorResult );
      printChunk( encodedChunk );
      chunkData = chunkToUnsignedShort( encodedChunk );
      chunkData ^= ( 0x8000 >> xorResult );
      encodedChunk = unsignedShortToChunk( chunkData );
      printChunk( encodedChunk );
   }
   else
   {
      // There were no errors
   }

   // Extract data bits into the last 11 bits of the unsigned short
   unsigned short dataBits = 0;
   dataBits |= encodedChunk.dataBitsThree << ( RAW_CHUNK_SIZE_BITS - 1 );
   dataBits |= encodedChunk.dataBitsFiveToSeven << ( RAW_CHUNK_SIZE_BITS - 4 );
   dataBits |= encodedChunk.dataBitsNineToFifteen;

   // Return the chunk in unsigned short form
   return dataBits & 0x7FF;
}

// Only copies 16 bits into a chunk, does not calculate parity bits
chunk unsignedShortToChunk( const unsigned short chunkData )
{
   chunk newChunk;

   // Initialize all bits to 0
   memset( &newChunk, 0, sizeof( chunk ) );

   // Copy parity bits
   newChunk.parityBitsZero = ( chunkData >> ( CHUNK_SIZE_BITS - 1 ) ) & 0x1;
   newChunk.parityBitsOneToTwo = ( chunkData >> ( CHUNK_SIZE_BITS - 3 ) ) & 0x3;
   newChunk.parityBitsFour = ( chunkData >> ( CHUNK_SIZE_BITS - 5 ) ) & 0x1;
   newChunk.parityBitsEight = ( chunkData >> ( CHUNK_SIZE_BITS - 9 ) ) & 0x1;

   // Copy data bits
   newChunk.dataBitsThree = ( chunkData >> ( CHUNK_SIZE_BITS - 4 ) ) & 0x1;
   newChunk.dataBitsFiveToSeven = ( chunkData >> ( CHUNK_SIZE_BITS - 8 ) ) & 0x7;
   newChunk.dataBitsNineToFifteen = chunkData & 0x7F;

   return newChunk;
}

// Copies 16 bits from a chunk into an unsigned short for use with bitwise operators
unsigned short chunkToUnsignedShort( const chunk chunk )
{
   unsigned short chunkData = 0;

   // Copy parity bits
   chunkData |= chunk.parityBitsZero << ( CHUNK_SIZE_BITS - 1 );
   chunkData |= chunk.parityBitsOneToTwo << ( CHUNK_SIZE_BITS - 3 );
   chunkData |= chunk.parityBitsFour << ( CHUNK_SIZE_BITS - 5 );
   chunkData |= chunk.parityBitsEight << ( CHUNK_SIZE_BITS - 9 );

   // Copy data bits
   chunkData |= chunk.dataBitsThree << ( CHUNK_SIZE_BITS - 4 );
   chunkData |= chunk.dataBitsFiveToSeven << ( CHUNK_SIZE_BITS - 8 );
   chunkData |= chunk.dataBitsNineToFifteen;

   return chunkData;
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

unsigned int getOverallParity( const chunk chunk )
{
   // Copy the chunk with data bits into an unsigned short so it can be used with bitwise operators
   unsigned short chunkData = chunkToUnsignedShort( chunk );

   // Iterate through bits, Toggling parity every activated bit
   unsigned int overallParity = 0;
   for ( int bitIndex = 0; bitIndex < CHUNK_SIZE_BITS; bitIndex++ )
   {
      if ( chunkData & ( 0x8000 >> bitIndex ) )
      {
         overallParity ^= 1;
      }
   }

   return overallParity;
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