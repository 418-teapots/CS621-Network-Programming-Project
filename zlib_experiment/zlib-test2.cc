#include <stdio.h>
#include <assert.h>
#include "zlib.h"

using namespace std;

int main(int argc, char* argv[])
{   

    // original array of unsigned char. 
    // unsigned char a[20] = {124, 76, 220, 45}; 
    // unsigned char a[20] = {1, 1, 1, 1}; 
    int n = 1024;
    unsigned char dataBeforeCompress[n];
    for (int i = 0; i < n; i++) {
        dataBeforeCompress[i] = 1;
    } 


    int outputLen = sizeof(dataBeforeCompress)/sizeof(*dataBeforeCompress);
    printf("outputLen: %u\n", outputLen);
    printf("dataBeforeCompress: \n");
    for (int i = 0; i < outputLen; i++) {
        printf("%u ", dataBeforeCompress[i]);
    }
    printf("\n");

    unsigned long src_size, dest_size;
    src_size = outputLen;
    dest_size = src_size + 100;
    unsigned char dataAfterCompression[dest_size];

    int result = compress((unsigned char*)dataAfterCompression, &dest_size, (unsigned char*)dataBeforeCompress, src_size);

    if (result == Z_OK) {
        printf("Compressed.\nbefore compression= %lu byte、after compression= %lu byte\n", src_size, dest_size);
      } else if (result == Z_MEM_ERROR) {
        printf("failed because of lack of memory.\n");
      } else if (result == Z_BUF_ERROR) {
        printf("failed because of lack of output buffer.\n");
      } else {
        printf("error.\n");
      }

    // outputLen = sizeof(dataAfterCompression)/sizeof(*dataAfterCompression);
    // printf("dataAfterCompression: \n");
    // for (int i = 0; i < outputLen; i++) {
    //   printf("%u ", dataAfterCompression[i]);
    // }
    // printf("\n\n");
     
    printf("\n----------\n\n");


    // outputLen = sizeof(dataAfterCompression)/sizeof(*dataAfterCompression);
    // printf("outputLen: %u\n", outputLen);
    // printf("dataAfterCompression: \n");
    // for (int i = 0; i < outputLen; i++) {
    //     printf("%u ", dataAfterCompression[i]);
    // }
    // printf("\n");

    unsigned long src_size2, dest_size2;
    // src_size2 = outputLen;
    src_size2 = dest_size;
    dest_size2 = src_size;
    unsigned char dataAfterDecompression[dest_size2];

    result = uncompress((unsigned char*)dataAfterDecompression, &dest_size2, (unsigned char*)dataAfterCompression, src_size2);


    if (result == Z_OK) {
      printf("Decompressed.\nbefore decompression= %lu byte、after decompression= %lu byte\n", src_size2, dest_size2);
    } else if (result == Z_MEM_ERROR) {
      printf("failed because of lack of memory.\n");
    } else if (result == Z_BUF_ERROR) {
      printf("failed because of lack of output buffer.\n");
    } else {
      printf("error.\n");
    }

    outputLen = sizeof(dataAfterDecompression)/sizeof(*dataAfterDecompression);
    printf("outputLen: %u\n", outputLen);
    printf("dataAfterDecompression: \n");
    for (int i = 0; i < outputLen; i++) {
      printf("%u ", dataAfterDecompression[i]);
    }
    printf("\n\n");



    return 0;
}




