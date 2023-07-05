#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "hamming.h"
#include "chunk.h"

int encode( const char *fileName )
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
   if ( fileSize == -1U || lseek( fd, 0, SEEK_SET ) != 0 )
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
      if ( bytesRead == -1U )
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
   unsigned short rawData;
   unsigned int chunkCount = ( fileSize * BITS_PER_BYTE ) / RAW_CHUNK_SIZE_BITS;
   for ( unsigned int chunkIndex = 0; chunkIndex < chunkCount; chunkIndex++ )
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
      printChunk( chunks[ chunkIndex ] );
      printf( "\n" );
   }

   close( fd );
   free( buffer );
   free( chunks );
   return 0;
}