#include <stdio.h>
#include <stdlib.h>
#include "hamming.h"

int main( int argc, char **argv )
{
   if ( argc != 2 )
   {
      printf( "Usage: %s <filename>\n", argv[0] );
      return 1;
   }

   encode( argv[1] );

   // unsigned int rawData = 0xeef;
   // printf( "Raw data: %u\n", rawData );
   // chunk encodedChunk = populateChunk( rawData );
   // printf( "Data bits: %u %u %u %u %u %u\n",
   //         encodedChunk.dataBitsThree,
   //         encodedChunk.dataBitsFiveToSeven,
   //         encodedChunk.dataBitsNineToFifteen,
   //         encodedChunk.parityBitsZeroToTwo,
   //         encodedChunk.parityBitsFour,
   //         encodedChunk.parityBitsEight
   // );

   return 0;
}