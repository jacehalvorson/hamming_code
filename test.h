#ifndef TEST_H
#define TEST_H

#include "chunk.h"

typedef void ( *test )( unsigned int );

typedef struct test_data {
   unsigned short rawData;
   chunk expectedChunk;
} test_data;

typedef struct test_list {
   test tests[ 2 ];
   unsigned int length;
} test_list;

void runTest( unsigned int testIndex );
void runTests( void );

void testPopulateChunk( unsigned int testIndex );
void testEncode( unsigned int testIndex );

#endif