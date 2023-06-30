#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "hamming.h"

chunk *populateChunk( unsigned int *rawDataPtr )
{
   chunk *newChunk = malloc( sizeof( chunk ) );
   if ( newChunk == NULL )
   {
      perror( "Error allocating memory" );
      free( newChunk );
      return NULL;
   }

   // Initialize all bits to 0
   memset( newChunk, 0, sizeof( chunk ) );

   // Fill in the data bits
   newChunk->dataBitsThree = ( rawDataPtr[0] & 1 );
   newChunk->dataBitsFiveToSeven = ( rawDataPtr[0] >> 1 ) & 7;
   newChunk->dataBitsNineToFifteen = ( rawDataPtr[0] >> 4 ) & 127;

   // XOR every activated bit
   unsigned int xorResult = 0;
   for ( int i = 0; i < RAW_CHUNK_SIZE_BITS; i++ )
   {
      if ( ( rawDataPtr[0] >> i ) & 1 )
      {
         xorResult ^= i;
      }
   }

   printf( "XOR result: %d\n", xorResult );
   return newChunk;
}

int encode( char *fileName )
{
   char *buffer = malloc( RAW_CHUNK_SIZE_BITS * CHUNKS_IN_BUFFER );
   if ( buffer == NULL )
   {
      perror( "Error allocating memory" );
      free( buffer );
      return 1;
   }

   int fd = open( fileName, O_RDONLY );
   if ( fd == -1 )
   {
      perror( "Error opening file" );
      close( fd );
      free( buffer );
      return 1;
   }

   unsigned int bytesRead = read( fd, buffer, READ_SIZE );
   unsigned int bufferOffset = bytesRead;
   if ( bytesRead == -1 )
   {
      perror( "Error reading file" );
      close( fd );
      free( buffer );
      return 1;
   }
   while ( bytesRead == RAW_CHUNK_SIZE_BITS && bufferOffset < ( ( RAW_CHUNK_SIZE_BITS * ( CHUNKS_IN_BUFFER - 1 ) ) ) )
   {
      bytesRead = read( fd, buffer + bufferOffset, READ_SIZE );
      if ( bytesRead == -1 )
      {
         perror( "Error reading file" );
         close( fd );
         free( buffer );
         return 1;
      }

      bufferOffset += bytesRead;
   }

   // Iterate through the buffer, passing 11 bits at a time to populateChunk



   unsigned int *rawData = ( unsigned int * ) buffer + bufferOffset - RAW_CHUNK_SIZE_BITS;
   chunk *c = populateChunk( rawData );

   close( fd );
   free( buffer );
   free( c );
   return 0;
}