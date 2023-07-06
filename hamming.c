#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "hamming.h"
#include "chunk.h"

int encode( const char *fileName )
{
   char *buffer = malloc( RAW_CHUNK_SIZE_BITS * CHUNKS_IN_BUFFER / BITS_PER_BYTE );
   if ( buffer == NULL )
   {
      perror( "Error allocating memory" );
      free( buffer );
      return 1; 
   }

   chunk *chunks = malloc( sizeof( chunk ) * CHUNKS_IN_BUFFER );
   if ( chunks == NULL )
   {
      perror( "Error allocating memory" );
      free( buffer );
      free( chunks );
      return 1;
   }

   int inputFile = open( fileName, O_RDONLY );
   if ( inputFile == -1 )
   {
      perror( "Error opening file" );
      free( buffer );
      free( chunks );
      close( inputFile );
      return 1;
   }

   char encodedFileName[ strlen( fileName ) + 5 ];
   sprintf( encodedFileName, "%s.ham", fileName );
   int outputFile = checkFileName( encodedFileName );
   if ( outputFile == -1 )
   {
      free( buffer );
      free( chunks );
      close( inputFile );
      close( outputFile );
      return 1;
   }

   int bytesRead;
   // Read / Process / Write loop
   do
   {
      // Read the file into the buffer
      bytesRead = readFromFile( inputFile, buffer, CHUNKS_IN_BUFFER * RAW_CHUNK_SIZE_BITS / BITS_PER_BYTE );
      if ( bytesRead == -1 )
      {
         break;
      }

      unsigned int fileChunkCount = ( bytesRead * BITS_PER_BYTE / RAW_CHUNK_SIZE_BITS );
      if ( ( bytesRead * BITS_PER_BYTE ) % RAW_CHUNK_SIZE_BITS != 0 )
      {
         // Leave room for an extra chunk if the file size isn't a multiple of 11
         fileChunkCount++;
      }
      // Take the minimum of the chunks in the file and what can fit in the buffer
      unsigned int chunkCount = ( fileChunkCount < CHUNKS_IN_BUFFER ) ? fileChunkCount : CHUNKS_IN_BUFFER;

      populateChunkArray( chunks, chunkCount, buffer, bytesRead );

      if ( writeToFile( outputFile, (char *)chunks, chunkCount * sizeof( chunk ) ) == -1 )
      {
         break;
      }
   }
   while ( bytesRead == CHUNKS_IN_BUFFER * RAW_CHUNK_SIZE_BITS / BITS_PER_BYTE );

   free( buffer );
   free( chunks );
   close( inputFile );
   close( outputFile );
   return 0;
}

int decode( const char *fileName )
{
   chunk *chunks = malloc( sizeof( chunk ) * CHUNKS_IN_BUFFER );
   if ( chunks == NULL )
   {
      perror( "Error allocating memory" );
      free( chunks );
      return 1;
   }

   int inputFile = open( fileName, O_RDONLY );
   if ( inputFile == -1 )
   {
      perror( "Error opening file" );
      free( chunks );
      close( inputFile );
      return 1;
   }

   char decodedFileName[ strlen( fileName ) + 5 ];
   sprintf( decodedFileName, "%s.dec", fileName );
   int outputFile = checkFileName( decodedFileName );
   if ( outputFile == -1 )
   {
      free( chunks );
      close ( inputFile );
      close( outputFile );
      return 1;
   }

   int bytesRead;
   do
   {
      // Read the file into the buffer
      bytesRead = readFromFile( inputFile, (char *)chunks, sizeof( chunk ) * CHUNKS_IN_BUFFER );
      if ( bytesRead == -1 )
      {
         break;
      }

      int chunkCount = bytesRead / sizeof( chunk );
      if ( bytesRead % sizeof( chunk ) != 0 )
      {
         printf( "File size is not a multiple of %ld\n", sizeof( chunk ) );
         break;
      }

      unsigned short rawDataBuffer[ chunkCount ];
      // Convert the chunks to raw data (unsigned short array)
      for ( int chunkIndex = 0; chunkIndex < chunkCount; chunkIndex++ )
      {
         rawDataBuffer[ chunkIndex ] = decodeChunk( chunks[ chunkIndex ] );
      }

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
            if ( byteIndex < bytesRead )
            {
               decodedBuffer[ byteIndex ] |= ( ( rawDataBuffer[ chunkIndex ] >> ( 10 - chunkOffset ) ) & 1 ) << ( 7 - bitIndex );
            }
            else
            {
               printf( "Decode: All file contents have been read. %d bits of padding added\n", RAW_CHUNK_SIZE_BITS - chunkOffset );
               break;
            }
         }
      }

      // Write decoded data to new file
      if ( writeToFile( outputFile, (char *)decodedBuffer, chunkCount * RAW_CHUNK_SIZE_BITS / BITS_PER_BYTE ) == -1 )
      {
         break;
      }
   }
   while ( bytesRead == sizeof( chunk ) * CHUNKS_IN_BUFFER );

   free( chunks );
   close( inputFile );
   close( outputFile );
   return 0;
}

void populateChunkArray( chunk *chunks, const int chunkCount, const char *buffer, const int bytesRead )
{
   int bitOffset;
   int byteIndex;
   unsigned int bitIndex;

   // Used to copy 11 bytes from the buffer and pass them to populateChunk
   unsigned short rawData;
   
   // Iterate through the buffer, passing 11 bits at a time to populateChunk
   for ( int chunkIndex = 0; chunkIndex < chunkCount; chunkIndex++ )
   {
      rawData = 0;
      bitOffset = chunkIndex * RAW_CHUNK_SIZE_BITS;

      // For this chunk, check each of the 11 bits and copy them to rawData
      for ( int chunkOffset = 0; chunkOffset < RAW_CHUNK_SIZE_BITS; chunkOffset++ )
      {
         byteIndex = ( bitOffset + chunkOffset ) / BITS_PER_BYTE;
         bitIndex = ( bitOffset + chunkOffset ) % BITS_PER_BYTE;

         // Don't read past the end of what was read from the file
         if ( byteIndex < bytesRead )
         {
            // Copy the bit at the current offset to the current 11-bit group
            rawData |= ( buffer[ byteIndex ] >> ( 7 - bitIndex ) & 1 ) << ( 10 - chunkOffset );
         }
         else
         {
            printf( "Encode: All file contents have been read. %d bits of padding added\n", RAW_CHUNK_SIZE_BITS - chunkOffset );
            break;
         }
      }

      // Add this chunk to the array of chunks
      chunks[ chunkIndex ] = populateChunk( rawData );
   }
}

int checkFileName( const char *fileName )
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

   int fd = open( fileName, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
   if ( fd == -1 )
   {
      perror( "Error opening file" );
      return -1;
   }

   return fd;
}

int readFromFile( const int fd, char *buffer, int maxNumBytes )
{
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
   while ( bytesRead > 0 && bufferOffset < maxNumBytes );

   return bufferOffset;
}


int writeToFile( const int fd, const char *buffer, int numBytes )
{
   int bufferOffset = 0;
   while ( numBytes > 0 )
   {
      int bytesWritten = write( fd, buffer + bufferOffset, numBytes );
      if ( bytesWritten == -1 )
      {
         perror( "Error writing to file" );
         return -1;
      }

      numBytes -= bytesWritten;
      bufferOffset += bytesWritten;
   }

   return bufferOffset;
}