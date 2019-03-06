#include <stdio.h>
#include <string.h>  
#include <assert.h>
#include <vector>
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
int compress(vector<unsigned char>& a, vector<unsigned char>& b) {

    // printf("sizeof(a): %lu\n", sizeof(a));
    // printf("sizeof(*a): %lu\n", sizeof(*a));

    printf("size: %lu\n", a->size());


    int originalLen = sizeof(a)/sizeof(*a);
    // printf("Original size is: %u\n", originalLen);
    // printf("Original array of unsigned char is: \n");

    // for (int i = 0; i < originalLen; i++) {
    //     printf("%u ", a[i]);
    // }
    printf("\n");

    printf("\n----------\n\n");
                    
    // zlib struct
    z_stream defstream;
    defstream.zalloc = Z_NULL;
    defstream.zfree = Z_NULL;
    defstream.opaque = Z_NULL;

    // setup "a" as the input and "b" as the compressed output
    defstream.avail_in = originalLen; // size of input
    defstream.next_in = (Bytef*)a; // input pointer
    printf("sizeof(b): %lu\n", sizeof(b));
    defstream.avail_out = sizeof(b); // size of output
    defstream.next_out = (Bytef*)b; // output pointer
    
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
    // int outputLen = sizeof(b)/sizeof(*b);
    // printf("defstream.next_out - b: %lu\n", defstream.next_out - b);
     
    // printf("Compressed size is: %u\n", outputLen);
    // printf("Compressed array of unsigned char is: \n");
    // for (int i = 0; i < outputLen; i++) {
    //     printf("%u ", b[i]);
    // }
    printf("\n");
}

// decompress b into c
int decompress(vector<unsigned char>& b, vector<unsigned char>& c) {

    // zlib struct
    z_stream infstream;
    infstream.zalloc = Z_NULL;
    infstream.zfree = Z_NULL;
    infstream.opaque = Z_NULL;

    int inputLen = sizeof(b)/sizeof(*b);
    

    // setup "b" as the input and "c" as the compressed output
    infstream.avail_in = inputLen; // size of input
    infstream.next_in = (Bytef*)b; // input pointer    
    infstream.avail_out = sizeof(c); // size of output
    infstream.next_out = (Bytef*)c; // output pointer
     
    // the actual decompression work.
    printf("inflateInit() start. \n");
    int ret = inflateInit(&infstream);
    if (ret != Z_OK) {
        zerr(ret); 
    }
    printf("inflate() start. \n");
    // ret = inflate(&infstream, Z_NO_FLUSH);
    ret = inflate(&infstream, Z_FINISH); // => buffer error
    if (ret != Z_OK) {
        zerr(ret); 
    }
    printf("inflateEnd() start. \n");
    ret = inflateEnd(&infstream);
    if (ret != Z_OK) {
        zerr(ret); 
    }

    int outputLen = sizeof(c)/sizeof(*c);                           
    // printf("Decompressed size is: %u\n", outputLen);
    // printf("Decompressed array of unsigned char is: \n");
    // for (int i = 0; i < outputLen; i++) {
    //     printf("%u ", c[i]);
    // }
    printf("\n");

}


int main(int argc, char* argv[])
{   
    // original array of unsigned char. 
    vector<unsigned char> a{124, 76, 220, 45}; 
    // unsigned char a[4]; 

    // placeholder for the compressed version of "a" 
    vector<unsigned char> b;

    // placeholder for the decompressed version of "b"
    vector<unsigned char> c;

    // Compress a into b. 
    compress(a, b);
     
    printf("\n----------\n\n");

    // Decompress b into c. 
    decompress(b, c);

    return 0;
}























