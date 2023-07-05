#ifndef TEST_H
#define TEST_H

#include "hamming.h"

typedef struct testData {
   unsigned short rawData;
   chunk expectedChunk;
} testData;

int testPopulateChunk( );
int testEncode( );

#endif