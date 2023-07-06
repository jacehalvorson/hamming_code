#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hamming.h"
#include "chunk.h"

int main( int argc, char **argv )
{
   if ( argc != 3 )
   {
      printf( "Usage: %s [encode|decode] <filename>\n", argv[0] );
      return 1;
   }

   if ( strcmp( argv[1], "encode" ) == 0 ||
        strcmp( argv[1], "e" ) == 0 )
   {
      encode( argv[ 2 ] );
   }
   else if ( strcmp( argv[1], "decode" ) == 0 ||
             strcmp( argv[1], "d" ) == 0 )
   {
      decode( argv[ 2 ] );
   }

   return 0;
}