#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define CHUNK_SIZE_BITS 11
#define CHUNKS_IN_BUFFER 16
#define READ_SIZE 512

int main( int argc, char **argv )
{
   if ( argc != 2 )
   {
      printf( "Usage: %s <filename>\n", argv[0] );
      return 1;
   }

   char *buffer = malloc( CHUNK_SIZE_BITS * CHUNKS_IN_BUFFER );
   if ( buffer == NULL )
   {
      perror( "Error allocating memory" );
      free( buffer );
      return 1;
   }

   int fd = open( argv[1], O_RDONLY );
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
   while ( bytesRead == CHUNK_SIZE_BITS && bufferOffset < ( ( CHUNK_SIZE_BITS * ( CHUNKS_IN_BUFFER - 1 ) ) ) )
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

      char *chunk = buffer + bufferOffset - CHUNK_SIZE_BITS;
      unsigned int bitIndex = 0;
      char currentBit = chunk[0];
      // printf( "%s\n", chunk );
      while ( bitIndex < CHUNK_SIZE_BITS )
      {
         printf( "%c", currentBit & 1 ? '1' : '0' );

         bitIndex++;
         currentBit = ( currentBit >> 1 );
      }
      printf( "\n" );
   }
   printf( "%s\n", buffer );

   close( fd );
   free( buffer );
   return 0;
}