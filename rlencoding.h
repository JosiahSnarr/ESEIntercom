

#ifndef RLENCODING_H
#define RLENCODING_H

#include <stdint.h>

#define DEFAULT_ESC 0x1B

#ifdef __cplusplus
extern "C"{
#endif

/**
    Run Length Encoding

    @param inBuffer
        The input buffer to decode

    @param inLen
        Length of the input buffer

    @param outBuffer
        Buffer to put encoded data

    @param outLen
        Max output buffer length

    @param esc
        the escape code
*/
int rlencode(uint8_t *inBuffer, int inLen, uint8_t *outBuffer, int outLen, uint8_t esc);

/**
    Run Length Decoding

    @param inBuffer

    @param iLen

    @param outBuffer

    @param max

    @param esc
*/
int rldecode(uint8_t* inBuffer, int iLen, uint8_t* outBuffer, int max, uint8_t esc);

#ifdef __cplusplus
}
#endif

#endif // RLENCODING_H
