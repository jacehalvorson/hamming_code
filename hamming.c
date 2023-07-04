#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "hamming.h"

void printBinary(unsigned int num, int bits) {
    if (bits > 0) {
        printBinary(num >> 1, bits - 1);
        printf("%d", num & 1);
    }
}

void printChunk(const chunk* c) {
   printBinary(c->parityBitsZeroToTwo, 3);

   printBinary(c->dataBitsThree, 1);
   printf("\n");

   printBinary(c->parityBitsFour, 1);

   printBinary(c->dataBitsFiveToSeven, 3);
   printf("\n");

   printBinary(c->parityBitsEight, 1);

   printBinary( ( c->dataBitsNineToFifteen & 0x70 ) >> 4, 3 );
   printf("\n");

   printBinary(c->dataBitsNineToFifteen & 0xF, 4);
   printf("\n");
}

chunk populateChunk( unsigned int rawData )
{
   chunk newChunk;

   // Initialize all bits to 0
   memset( &newChunk, 0, sizeof( chunk ) );

   // Fill in the data bits
   newChunk.dataBitsThree = ( rawData >> ( RAW_CHUNK_SIZE_BITS - 1 ) ) & 0x1;
   newChunk.dataBitsFiveToSeven = ( rawData >> ( RAW_CHUNK_SIZE_BITS - 4 ) ) & 0x7;
   newChunk.dataBitsNineToFifteen = rawData & 0x7F;

   // XOR every activated bit
   unsigned int xorResult = 0;
   for ( int i = 0; i < RAW_CHUNK_SIZE_BITS; i++ )
   {
      if ( ( rawData >> i ) & 1 )
      {
         xorResult ^= i;
      }
   }

   printf( "XOR result: %d\n", xorResult );
   for ( int i = 0; i < 2; i++ )
   {
      printf( "Setting bit %d to %d\n", i, ( xorResult >> i ) & 1 );
      newChunk.parityBitsZeroToTwo |= ( ( xorResult >> i ) & 1 ) << i;
   }
   newChunk.parityBitsFour = ( xorResult >> 2 ) & 1;
   newChunk.parityBitsEight = ( xorResult >> 3 ) & 1;

   return newChunk;
}

int encode( char *fileName )
{
   chunk *chunks = malloc( sizeof( chunk ) * CHUNKS_IN_BUFFER );
   char *buffer = malloc( RAW_CHUNK_SIZE_BITS * CHUNKS_IN_BUFFER );
   if ( buffer == NULL )
   {
      perror( "Error allocating memory" );
      free( buffer );
      free( chunks );
      return 1; 
   }

   int fd = open( fileName, O_RDONLY );
   if ( fd == -1 )
   {
      perror( "Error opening file" );
      close( fd );
      free( buffer );
      free( chunks );
      return 1;
   }

   // Get file size by seeking to end, then seek back to beginning
   unsigned int fileSize = lseek( fd, 0, SEEK_END );
   if ( fileSize == -1 || lseek( fd, 0, SEEK_SET ) != 0 )
   {
      perror( "Error getting file size" );
      close( fd );
      free( buffer );
      free( chunks );
      return 1;
   }

   // Read file into buffer
   unsigned int bytesRead;
   unsigned int bufferOffset = 0;
   do
   {
      bytesRead = read( fd, buffer + bufferOffset, READ_SIZE );
      if ( bytesRead == -1 )
      {
         perror( "Error reading file" );
         close( fd );
         free( buffer );
         free( chunks );
         return 1;
      }

      bufferOffset += bytesRead;
   }
   while ( bytesRead == RAW_CHUNK_SIZE_BITS && bufferOffset < ( ( RAW_CHUNK_SIZE_BITS * ( CHUNKS_IN_BUFFER - 1 ) ) ) );

   // Iterate through the buffer, passing 11 bits at a time to populateChunk
   unsigned int bitOffset;
   unsigned int byteIndex;
   unsigned int bitIndex;
   unsigned int rawData;
   unsigned int chunkCount = ( fileSize * BITS_PER_BYTE ) / RAW_CHUNK_SIZE_BITS;
   for ( int chunkIndex = 0; chunkIndex < chunkCount; chunkIndex++ )
   {
      rawData = 0;
      bitOffset = chunkIndex * RAW_CHUNK_SIZE_BITS;

      // For this chunk, check each of the 11 bits and copy them to rawData
      for ( int chunkOffset = 0; chunkOffset < RAW_CHUNK_SIZE_BITS; chunkOffset++ )
      {
         byteIndex = ( bitOffset + chunkOffset ) / BITS_PER_BYTE;
         bitIndex = ( bitOffset + chunkOffset ) % BITS_PER_BYTE;

         rawData |= ( buffer[ byteIndex ] >> ( 7 - bitIndex ) & 1 ) << ( 10 - chunkOffset );
      }

      printf( "Raw data: " );
      printBinary( rawData, RAW_CHUNK_SIZE_BITS );
      printf( "\n" );

      chunks[ chunkIndex ] = populateChunk( rawData );
      printf( "Chunk %d\n\n", chunkIndex );
      printChunk( &chunks[ chunkIndex ] );
      printf( "\n" );
   }

   close( fd );
   free( buffer );
   free( chunks );
   return 0;
}