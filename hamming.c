#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "hamming.h"
#include "chunk.h"
#include "file.h"

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

   // Get the size of the file by seeking to end
   int fileSize = lseek( inputFile, 0, SEEK_END );
   // Error check the seek and then seek back to the beginning of file
   if ( fileSize == -1 ||
        lseek( inputFile, 0, SEEK_SET ) == -1 )
   {
      perror( "Error getting file size" );
      free( buffer );
      free( chunks );
      close( inputFile );
      close( outputFile );
      return 1;
   }

   // Write the header of the new encoded file with chunk count 
   header fileHeader = { fileSize };
   int bytesWritten = writeToFile( outputFile, (char *)&fileHeader, sizeof( header ) );
   if ( bytesWritten == -1 )
   {
      free( chunks );
      close( inputFile );
      close( outputFile );
      return 1;
   }

   int bytesRead;
   unsigned int chunkCountInBuffer = 0;
   // Read / Process / Write loop
   do
   {
      // Read the file into the buffer
      bytesRead = readFromFile( inputFile, buffer, BYTES_IN_BUFFER );
      if ( bytesRead == -1 )
      {
         break;
      }

      chunkCountInBuffer = ( bytesRead * BITS_PER_BYTE / RAW_CHUNK_SIZE_BITS );
      if ( ( bytesRead * BITS_PER_BYTE ) % RAW_CHUNK_SIZE_BITS != 0 )
      {
         // Leave room for an extra chunk if the file size isn't a multiple of 11
         chunkCountInBuffer++;
      }

      populateChunkArray( chunks, chunkCountInBuffer, buffer, bytesRead );

      // Write the chunks to the output file
      bytesWritten = writeToFile( outputFile, (char *)chunks, chunkCountInBuffer * sizeof( chunk ) );
      if ( bytesWritten == -1 )
      {
         break;
      }
   }
   while ( bytesWritten == CHUNKS_IN_BUFFER * sizeof( chunk ) );

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

   // Read the header of the file to get the chunk count
   header fileHeader;
   int bytesRead = readFromFile( inputFile, (char *)&fileHeader, sizeof( header ) );
   if ( bytesRead == -1 )
   {
      free( chunks );
      close( inputFile );
      close( outputFile );
      return 1;
   }

   int bytesToWrite;
   int totalBytesWritten = 0;
   int bytesWritten;
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

      // Write decoded data to new file as long as the chunk count is not surpassed
      bytesToWrite = chunkCount * RAW_CHUNK_SIZE_BITS / BITS_PER_BYTE;
      if ( totalBytesWritten + bytesToWrite > fileHeader.originalFileSize )
      {
         printf( "Decode: Overflow detected, cutting off at %d bytes\n", fileHeader.originalFileSize );
         bytesToWrite = fileHeader.originalFileSize - totalBytesWritten;
      }

      bytesWritten = writeToFile( outputFile, (char *)decodedBuffer, bytesToWrite );
      if  ( bytesWritten == -1 )
      {
         break;
      }
      totalBytesWritten += bytesWritten;
   }
   while ( bytesWritten == BYTES_IN_BUFFER );

   if ( totalBytesWritten != fileHeader.originalFileSize )
   {
      printf( "Decode: %d bytes written, original file was %d bytes.\n", totalBytesWritten, fileHeader.originalFileSize );
   }

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