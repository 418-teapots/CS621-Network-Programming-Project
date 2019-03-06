#include <stdio.h>
#include <assert.h>
#include "zlib.h"

using namespace std;
    
void zerr(int ret)
{
    switch (ret) {
    case Z_ERRNO: 
        fputs("error \n", stderr);
        break;
    case Z_STREAM_ERROR:
        fputs("invalid compression level\n", stderr);
        break;
    case Z_BUF_ERROR: 
        fputs("buffer error\n", stderr);
        break;
    case Z_DATA_ERROR:
        fputs("invalid or incomplete deflate data\n", stderr);
        break;
    case Z_MEM_ERROR:
        fputs("out of memory\n", stderr);
        break;
    case Z_VERSION_ERROR:
        fputs("zlib version mismatch!\n", stderr);
    }
}

// compress a into b. 
int compress(unsigned char* a, int inputSize, unsigned char* b, int outputSize) {
                    
    // zlib struct
    z_stream defstream;
    defstream.zalloc = Z_NULL;
    defstream.zfree = Z_NULL;
    defstream.opaque = Z_NULL;

    // setup "a" as the input and "b" as the compressed output
    defstream.avail_in = inputSize; // size of input
    defstream.next_in = a; // input pointer
    defstream.avail_out = outputSize; // size of output
    defstream.next_out = b; // output pointer
    
    // the actual compression work.
    int ret = deflateInit(&defstream, Z_BEST_COMPRESSION);
    if (ret != Z_OK) {
        zerr(ret); 
    }
    ret = deflate(&defstream, Z_FINISH);
    // ret = deflate(&defstream, Z_NO_FLUSH);
    if (ret != Z_OK) {
        zerr(ret); 
    }
    ret = deflateEnd(&defstream);
    if (ret != Z_OK) {
        zerr(ret); 
    }

    // printf("defstream.next_out - b: %lu\n", defstream.next_out - b);

    printf("\n"); 
}

// decompress b into c
int decompress(unsigned char* b, int inputSize, unsigned char* c, int outputSize) {

    // zlib struct
    z_stream infstream;
    infstream.zalloc = Z_NULL;
    infstream.zfree = Z_NULL;
    infstream.opaque = Z_NULL;

    // setup "b" as the input and "c" as the compressed output
    infstream.avail_in = inputSize; // size of input
    infstream.next_in = b; // input pointer    
    infstream.avail_out = outputSize; // size of output
    infstream.next_out = c; // output pointer
     
    // the actual decompression work.
    printf("inflateInit() start. \n");
    int ret = inflateInit(&infstream);
    if (ret != Z_OK) {
        zerr(ret); 
    }
    printf("inflate() start. \n");
    // ret = inflate(&infstream, Z_NO_FLUSH);
    ret = inflate(&infstream, Z_FINISH); 
    if (ret != Z_OK) {
        zerr(ret); 
    }
    printf("inflateEnd() start. \n");
    ret = inflateEnd(&infstream);
    if (ret != Z_OK) {
        zerr(ret); 
    }

    printf("\n");
}


int main(int argc, char* argv[])
{   
    // original array of unsigned char. 
    // unsigned char a[20] = {124, 76, 220, 45}; 
    // unsigned char a[20] = {1, 1, 1, 1}; 
    int n = 1024;
    unsigned char a[n];
    for (int i = 0; i < n; i++) {
        a[i] = 1;
    } 


    printf("sizeof(a): %lu\n", sizeof(a)); // 
    printf("sizeof(*a): %lu\n", sizeof(*a)); // 
    printf("\n");

    printf("Original array of unsigned char is: \n");
    int inputLen = sizeof(a)/sizeof(*a);
    for (int i = 0; i < inputLen; i++) {
        printf("%u ", a[i]);
    }
    printf("\n\n");


    // placeholder for the compressed version of "a" 
    // unsigned char b[20];
    unsigned char b[n];

    int inputSize = sizeof(a);
    int outputSize = sizeof(b);
    printf("inputSize: %u\n", inputSize); // 
    printf("outputSize: %lu\n", sizeof(b)); // 
    printf("\n");

    // Compress a into b. 
    compress(a, inputSize, b, outputSize);

    int outputLen = sizeof(b)/sizeof(*b);
    printf("Compressed array of unsigned char is: \n");
    for (int i = 0; i < outputLen; i++) {
        printf("%u ", b[i]);
    }
    printf("\n");

     
    printf("\n----------\n\n");


    // placeholder for the decompressed version of "b"
    // unsigned char c[20];
    unsigned char c[n];

    inputSize = sizeof(b);
    printf("inputSize: %lu\n", sizeof(b));
    outputSize = sizeof(c);
    printf("outputSize: %lu\n", sizeof(c));  
    printf("\n");

    // Decompress b into c. 
    decompress(b, inputSize, c, outputSize);

    outputLen = sizeof(c)/sizeof(*c);
    printf("Decompressed array of unsigned char is: \n");
    for (int i = 0; i < outputLen; i++) {
        printf("%u ", c[i]);
    }
    printf("\n");



    return 0;
}























