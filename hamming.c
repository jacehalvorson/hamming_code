#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "hamming.h"
#include "chunk.h"

int readFromFile( const char *fileName, char *buffer, int maxNumBytes )
{
   int fd = open( fileName, O_RDONLY );
   if ( fd == -1 )
   {
      perror( "Error opening file" );
      close( fd );
      return -1;
   }

   int bytesRead;
   int bufferOffset = 0;
   do
   {
      bytesRead = read( fd, buffer + bufferOffset, maxNumBytes - bufferOffset );
      if ( bytesRead == -1 )
      {
         perror( "Error reading from file" );
         close( fd );
         return -1;
      }

      bufferOffset += bytesRead;
   }
   while ( bytesRead == READ_SIZE && bufferOffset < maxNumBytes );

   close( fd );
   return bufferOffset;
}

int writeToFile( const char *fileName, const char *buffer, int numBytes )
{
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
            return -1;
         }
      }
      else
      {
         perror( "Error reading input" );
         return -1;
      }
   }

   // File doesn't exist, so we can use this name
   int fd = open( fileName, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
   if ( fd == -1 )
   {
      perror( "Error opening file" );
      close( fd );
      return -1;
   }

   while ( numBytes > 0 )
   {
      int bytesWritten = write( fd, buffer, numBytes );
      if ( bytesWritten == -1 )
      {
         perror( "Error writing to file" );
         close( fd );
         return -1;
      }

      numBytes -= bytesWritten;
      buffer += bytesWritten;
   }

   int fileSize = lseek( fd, 0, SEEK_END );
   close( fd );
   return fileSize;
}

int encode( const char *fileName )
{
   chunk *chunks = malloc( sizeof( chunk ) * CHUNKS_IN_BUFFER );
   char *buffer = malloc( RAW_CHUNK_SIZE_BITS * CHUNKS_IN_BUFFER / BITS_PER_BYTE );
   if ( buffer == NULL )
   {
      perror( "Error allocating memory" );
      free( buffer );
      free( chunks );
      return 1; 
   }

   // Read the file into the buffer
   int fileSize = readFromFile( fileName, buffer, CHUNKS_IN_BUFFER * RAW_CHUNK_SIZE_BITS / BITS_PER_BYTE );
   if ( fileSize == -1 )
   {
      free( buffer );
      free( chunks );
      return 1;
   }

   int bitOffset;
   int byteIndex;
   unsigned int bitIndex;
   // Leave room for an extra chunk if the file size isn't a multiple of 11
   unsigned int fileChunkCount = ( fileSize * BITS_PER_BYTE / RAW_CHUNK_SIZE_BITS ) + 1;
   // Take the minimum of the chunks in the file and what can fit in the buffer
   unsigned int chunkCount = ( fileChunkCount < CHUNKS_IN_BUFFER ) ? fileChunkCount : CHUNKS_IN_BUFFER;
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
         if ( byteIndex < fileSize )
         {
            // Copy the bit at the current offset to the current 11-bit group
            rawData |= ( buffer[ byteIndex ] >> ( 7 - bitIndex ) & 1 ) << ( 10 - chunkOffset );
         }
         else
         {
            printf( "All file contents have been read. %d bits of padding added\n", RAW_CHUNK_SIZE_BITS - chunkOffset );
            break;
         }
      }

      // Add this chunk to the array of chunks
      chunks[ chunkIndex ] = populateChunk( rawData );
   }

   char encodedFileName[ strlen( fileName ) + 1 ];
   strcpy( encodedFileName, fileName );
   strcpy( encodedFileName + strlen( fileName ) - 4, ".ham" );
   writeToFile( encodedFileName, (char *)chunks, chunkCount * sizeof( chunk ) );

   free( buffer );
   free( chunks );
   return 0;
}

int decode( const char *fileName )
{
   chunk *chunks = malloc( sizeof( chunk ) * CHUNKS_IN_BUFFER );
   
   // Read the file into the buffer
   int fileSize = readFromFile( fileName, (char *)chunks, sizeof( chunk ) * CHUNKS_IN_BUFFER );
   if ( fileSize == -1 )
   {
      free( chunks );
      return 1;
   }
   printf( "File size: %d\n", fileSize );

   int chunkCount = fileSize / sizeof( chunk );
   if ( fileSize % sizeof( chunk ) != 0 )
   {
      printf( "File size is not a multiple of %ld\n", sizeof( chunk ) );
      free( chunks );
      return 1;
   }

   // Convert the chunks to raw data (unsigned short array)
   unsigned short rawDataBuffer[ chunkCount ];
   for ( int chunkIndex = 0; chunkIndex < chunkCount; chunkIndex++ )
   {
      rawDataBuffer[ chunkIndex ] = decodeChunk( chunks[ chunkIndex ] );
   }
   // Data exists as unsigned shorts on the stack, free memory used for chunk buffer
   free( chunks );

   char decodedBuffer[ ( chunkCount * RAW_CHUNK_SIZE_BITS + 1 ) / BITS_PER_BYTE ];
   memset( decodedBuffer, 0, sizeof( decodedBuffer ) );
   int byteIndex;
   int bitIndex;

   // Iterate through unsigned shorts in the raw data buffer, saving 11 bits at a time
   for ( int chunkIndex = 0; chunkIndex < chunkCount; chunkIndex++ )
   {
      // For this chunk, check each of the 11 bits and copy them to decodedBuffer
      for ( int chunkOffset = 0; chunkOffset < RAW_CHUNK_SIZE_BITS; chunkOffset++ )
      {
         byteIndex = ( chunkIndex * RAW_CHUNK_SIZE_BITS + chunkOffset ) / BITS_PER_BYTE;
         bitIndex = ( chunkIndex * RAW_CHUNK_SIZE_BITS + chunkOffset ) % BITS_PER_BYTE;

         // Don't read past the end of what was read from the file
         if ( byteIndex < fileSize )
         {
            decodedBuffer[ byteIndex ] |= ( ( rawDataBuffer[ chunkIndex ] >> ( 10 - chunkOffset ) ) & 1 ) << ( 7 - bitIndex );
         }
         else
         {
            printf( "All file contents have been read. %d bits of padding added\n", RAW_CHUNK_SIZE_BITS - chunkOffset );
            break;
         }
      }
   }
   for ( int i = 0; i <= byteIndex; i++ )
   {
      printBinary( decodedBuffer[ i ], 8 );
      printf( "\n" );
   }

   // Write decoded data to new file
   char decodedFileName[ strlen( fileName ) + 1 ];
   strcpy( decodedFileName, fileName );
   strcpy( decodedFileName + strlen( fileName ) - 4, ".dec" );
   writeToFile( decodedFileName, (char *)decodedBuffer, chunkCount * RAW_CHUNK_SIZE_BITS / BITS_PER_BYTE );

   return 0;
}