#include <stdio.h>
#include <assert.h>
// #include "zlib.h"
#include <zlib.h>
#include "cs621-compression-util.cc"

using namespace std;

int main(int argc, char* argv[])
{   

    CS621CompressionUtil compUtil;


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
    compUtil.compress(a, inputSize, b, outputSize);

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
    compUtil.decompress(b, inputSize, c, outputSize);

    outputLen = sizeof(c)/sizeof(*c);
    printf("Decompressed array of unsigned char is: \n");
    for (int i = 0; i < outputLen; i++) {
        printf("%u ", c[i]);
    }
    printf("\n");



    return 0;
}




