// Compression utility class. 
// Hold compress and decompress methods.

#include <stdio.h>
#include <assert.h>
// #include "zlib.h"
#include <zlib.h>

using namespace std;

class CS621CompressionUtil {

public: 

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
void
compress(unsigned char* a, int inputSize, unsigned char* b, int outputSize) {

    // std::cout << "compress() start." << std::endl;                                             

                    
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
void
decompress(unsigned char* b, int inputSize, unsigned char* c, int outputSize) {

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





};

