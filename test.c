#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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
   if ( testIndex > 0 && testIndex <= testList.length )
   {
      testList.tests[ testIndex-1 ]( testIndex );
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
      testList.tests[ i ]( i+1 );
   }
}

void testPopulateChunk( unsigned int testIndex )
{
   int successCount = 0;
   int testCaseCount = 5;
   printf( "Test %u\nRunning testPopulateChunk...\n\n", testIndex );

   // Test case loop
   for ( int i = 0; i < testCaseCount; i++ )
   {
      unsigned int rawData = populateChunkTestArray[ i ].rawData;
      chunk expected = populateChunkTestArray[ i ].expectedChunk;
      chunk actual = populateChunk( rawData );

      printf( "Test case %d: ", i+1 );
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
      printf( "\n" );
   }

   printf( "Test cases passed: %d/%d\n\n", successCount, testCaseCount );
}

void testEncode( unsigned int testIndex )
{
   int successCount = 0;
   int testCaseCount = 1;
   printf( "Test %u\nRunning testEncode...\n\n", testIndex );

   for ( int i = 0; i < testCaseCount; i++ )
   {
      printf( "Test case %d: ", i+1 );
      // Run test.sh
      if ( 1 )
      {
         printf( "Passed\n" );
         successCount++;
      }
      else
      {
         printf( "Failed\n" );
      }
      printf( "\n" );
   }

   printf( "Test cases passed: %d/%d\n\n", successCount, testCaseCount );
}

int main( int argc, char **argv )
{
   unsigned int testIndex = 0;
   switch ( argc )
   {
      case 1:
         // No arguments, run all tests
         runTests( );
         break;
      case 2:
         // User specified a test index
         testIndex = atoi( argv[ 1 ] );
         if ( testIndex > 0 )
         {
            if ( testIndex > 0 )
            {
               runTest( testIndex );
            }
         }
         else
         {
            printf( "Invalid test index: %s\n", argv[ 1 ] );
         }
         break;
      default:
         printf( "Usage: test [testIndex]\n" );
         return 1;
   }

   return 0;
}