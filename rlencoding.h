#ifndef RLENCODING_H
#define RLENCODING_H

#include <stdint.h>

#define DEFAULT_ESC 0x1B

#ifdef __cplusplus
extern "C"{
#endif

void rlencode(uint8_t *inBuffer, int inLen, uint8_t *outBuffer, int outLen, uint8_t esc);
void rldecode();

#ifdef __cplusplus
}
#endif

#endif // RLENCODING_H
