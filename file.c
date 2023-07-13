#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "file.h"

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