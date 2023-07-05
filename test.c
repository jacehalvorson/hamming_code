#include <stdio.h>
#include <string.h>
#include "hamming.h"
#include "test.h"

test_data populateChunkTestArray[ ] = {
   { 0b00000000000, { 0b0, 0b00, 0b0, 0b0, 0b000, 0b0, 0b0000000 } },
   { 0b01010101010, { 0b1, 0b01, 0b0, 0b0, 0b101, 0b1, 0b0101010 } },
   { 0b11001100110, { 0b0, 0b01, 0b1, 0b1, 0b100, 0b0, 0b1100110 } },
   { 0b10100111101, { 0b1, 0b01, 0b1, 0b0, 0b010, 0b1, 0b0111101 } },
   { 0b11111111111, { 0b1, 0b11, 0b1, 0b1, 0b111, 0b1, 0b1111111 } },
};

int populateChunkTestCaseCount = sizeof( populateChunkTestArray ) / sizeof( test_data );

void runTest( unsigned int testIndex )
{
   static test_list testList = {
      {
         testPopulateChunk,
         testEncode,
      },
      2
   };

   // Unsigned integer can't be negative, but make sure it's in range
   if ( testIndex < testList.length )
   {
      testList.tests[ testIndex ]( testIndex );
   }
   else
   {
      printf( "Invalid test index: %d\n", testIndex );
   }
}

void runTests( void )
{
   // Tets list composed of an array of function pointers and a length
   static test_list testList = {
      {
         testPopulateChunk,
         testEncode,
      },
      2
   };

   // Run tests
   for ( unsigned int i = 0; i < testList.length; i++ )
   {
      // Call the ith function in the test list
      testList.tests[ i ]( i );
   }
}

void testPopulateChunk( unsigned int testIndex )
{
   int successCount = 0;
   int totalCount = 0;

   printf( "Test %u\nRunning testPopulateChunk...\n", testIndex );

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
         printChunk( actual );
         printf( "XOR Result: %u\n", xorChunk( actual ) );
         printf( "Expected:\n" );
         printChunk( expected );
         printf( "XOR Result: %u\n", xorChunk( expected ) );
      }
      totalCount++;
      printf( "\n" );
   }

   printf( "Test cases passed: %d/%d\n", successCount, totalCount );
}

void testEncode( unsigned int testIndex )
{
   printf( "Test %u\nRunning testEncode...\n", testIndex );

   encode( "sample.txt" );
}

int main( int argc, char **argv )
{
   if ( argc != 1 )
   {
      printf( "Usage: %s\n", argv[ 0 ] );
      return 1;
   }

   // Run tests
   runTests( );

   return 0;
}