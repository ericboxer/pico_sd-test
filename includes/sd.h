#ifndef SD_CONFIG
#define SD_CONFIG

#include <stdint.h>

void printTest(void);

void sdInit(void);
void sdCheckForFile(char *filename);

void sdUnmount(void);

void sdWriteString(char *filename, char *data);
void sdWriteData(char *filename, uint8_t data[], int dataSize);
void sdRead(char *filename);
void sdClose(void);

#endif