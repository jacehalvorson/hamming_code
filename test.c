#include <stdio.h>
#include <string.h>
#include "hamming.h"
#include "print.h"
#include "test.h"

testData populateChunkTestArray[ ] = {
   { 0b00000000000, { 0b0, 0b00, 0b0, 0b0, 0b000, 0b0, 0b0000000 } },
   { 0b01010101010, { 0b1, 0b01, 0b0, 0b0, 0b101, 0b1, 0b0101010 } },
   { 0b11001100110, { 0b0, 0b01, 0b1, 0b1, 0b100, 0b0, 0b1100110 } },
   { 0b11111111111, { 0b1, 0b11, 0b1, 0b1, 0b111, 0b1, 0b1111111 } },
};

int populateChunkTestCaseCount = sizeof( populateChunkTestArray ) / sizeof( testData );

int testPopulateChunk( )
{
   int successCount = 0;
   int totalCount = 0;

   // Test case loop
   for ( int i = 0; i < populateChunkTestCaseCount; i++ )
   {
      unsigned int rawData = populateChunkTestArray[ i ].rawData;
      chunk expected = populateChunkTestArray[ i ].expectedChunk;
      chunk actual = populateChunk( rawData );

      printf( "Test case %d: ", i );
      if ( memcmp( &actual, &expected, sizeof( chunk ) ) == 0 )
      {
         printf( "Passed\n" );
         successCount++;
      }
      else
      {
         printf( "Failed\n\nActual:\n" );
         printChunk( &actual );
         printf( "Expected:\n" );
         printChunk( &expected );
      }
      totalCount++;
      printf( "\n" );
   }

   printf( "Test cases passed: %d/%d\n", successCount, totalCount );
}

int testEncode( )
{

}