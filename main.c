#include <stdio.h>
#include <stdlib.h>
#include "hamming.h"
#include "chunk.h"

int main( int argc, char **argv )
{
   if ( argc != 2 )
   {
      printf( "Usage: %s <filename>\n", argv[0] );
      return 1;
   }

   encode( argv[1] );

   return 0;
}