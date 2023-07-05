#include <stdio.h>
#include "hamming.h"
#include "print.h"

void printBinary( unsigned int num, int bits )
{
    if ( bits > 0 )
    {
        printBinary( num >> 1, bits - 1 );
        printf( "%d", num & 1 );
    }
}

void printChunk( const chunk* c )
{
   printBinary( c->parityBitsZero, 1 );

   printBinary( c->parityBitsOneToTwo, 2 );

   printBinary( c->dataBitsThree, 1 );
   printf( "\n" );

   printBinary( c->parityBitsFour, 1 );

   printBinary( c->dataBitsFiveToSeven, 3 );
   printf( "\n" );

   printBinary( c->parityBitsEight, 1 );

   printBinary( ( c->dataBitsNineToFifteen & 0x70 ) >> 4, 3 );
   printf( "\n" );

   printBinary( c->dataBitsNineToFifteen & 0xF, 4 );
   printf( "\n" );
}