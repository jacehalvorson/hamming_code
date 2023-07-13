#ifndef FILE_H
#define FILE_H

int checkFileName( const char *fileName );
int readFromFile( const int fd, char *buffer, int maxNumBytes );
int writeToFile( const int fd, const char *buffer, int numBytes );

#endif