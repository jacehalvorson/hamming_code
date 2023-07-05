#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "hamming.h"
#include "chunk.h"

int writeToFile( const char *fileName, const char *buffer, size_t numBytes )
{
   printf( "Checking %s\n", fileName );
   if ( access( fileName, F_OK ) != -1 )
   {
      printf( "File %s already exists, want to override? [y/n] ", fileName );

      int input;
      int readResult = fread( &input, 1, 1, stdin );
      if ( readResult > 0 )
      {
         if ( (char)input == 'y' || (char)input == 'Y' )
         {
            printf( "\nOverwriting %s\n", fileName );
         }
         else
         {
            printf( "\nExiting\n" );
            return 1;
         }
      }
      else
      {
         perror( "Error reading input" );
         return 1;
      }
   }

   // File doesn't exist, so we can use this name
   printf( "Using %s\n", fileName );
   int fd = open( fileName, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
   if ( fd == -1 )
   {
      perror( "Error opening file" );
      close( fd );
      return 1;
   }

   while ( numBytes > 0 )
   {
      int bytesWritten = write( fd, buffer, numBytes );
      if ( bytesWritten == -1 )
      {
         perror( "Error writing to file" );
         close( fd );
         return 1;
      }

      numBytes -= bytesWritten;
      buffer += bytesWritten;
   }

   close( fd );
   return 0;
}

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
   while ( bytesRead == READ_SIZE && bufferOffset < ( ( RAW_CHUNK_SIZE_BITS * CHUNKS_IN_BUFFER ) / BITS_PER_BYTE ) );
   if ( close( fd ) == -1 )
   {
      perror( "Error closing file" );
      free( buffer );
      free( chunks );
      return 1;
   }

   if ( bufferOffset >= ( ( RAW_CHUNK_SIZE_BITS * CHUNKS_IN_BUFFER ) / BITS_PER_BYTE ) )
   {
      // File is smaller than the buffer, so we can just encode it all at once
      printf( "File is too big for buffer size %d\n", ( CHUNKS_IN_BUFFER * RAW_CHUNK_SIZE_BITS ) / BITS_PER_BYTE );
      free( buffer );
      free( chunks );
      return 1;
   }

   unsigned int bitOffset;
   unsigned int byteIndex;
   unsigned int bitIndex;
   // Leave room for an extra chunk if the file size isn't a multiple of 11
   unsigned int fileChunkCount = ( fileSize * BITS_PER_BYTE / RAW_CHUNK_SIZE_BITS ) + 1;
   // Take the minimum of the chunks in the file and what can fit in the buffer
   unsigned int chunkCount = ( fileChunkCount < CHUNKS_IN_BUFFER ) ? fileChunkCount : CHUNKS_IN_BUFFER;
   unsigned int chunkLeftOver = ( fileSize * BITS_PER_BYTE ) % RAW_CHUNK_SIZE_BITS;
   // Used to copy 11 bytes from the buffer and pass them to populateChunk
   unsigned short rawData;
   
   // Iterate through the buffer, passing 11 bits at a time to populateChunk
   for ( unsigned int chunkIndex = 0; chunkIndex < chunkCount; chunkIndex++ )
   {
      rawData = 0;
      bitOffset = chunkIndex * RAW_CHUNK_SIZE_BITS;

      // For this chunk, check each of the 11 bits and copy them to rawData
      for ( int chunkOffset = 0; chunkOffset < RAW_CHUNK_SIZE_BITS; chunkOffset++ )
      {
         byteIndex = ( bitOffset + chunkOffset ) / BITS_PER_BYTE;
         bitIndex = ( bitOffset + chunkOffset ) % BITS_PER_BYTE;

         // Don't read past the end of what was read from the file
         if ( byteIndex < bufferOffset )
         {
            rawData |= ( buffer[ byteIndex ] >> ( 7 - bitIndex ) & 1 ) << ( 10 - chunkOffset );
         }
         else
         {
            printf( "All file contents have been read. %d bytes of padding added\n", RAW_CHUNK_SIZE_BITS - chunkOffset );
            break;
         }
      }

      // Add this chunk to the array of chunks
      chunks[ chunkIndex ] = populateChunk( rawData );
   }

   writeToFile( "encoded.ham", (char *)chunks, chunkCount * sizeof( chunk ) );

   free( buffer );
   free( chunks );
   return 0;
}

int decode( const char *fileName )
{
   return 0;
}