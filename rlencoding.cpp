

#include "rlencoding.h"


int rlencode(uint8_t *inBuffer, int inLen, uint8_t *outBuffer, int outLen, uint8_t esc)
{
    int i, outIdx = 0;

    uint8_t current = inBuffer[0];
    uint8_t count = 0;

    for(i = 0; i < inLen; ++i){

        // count the number of the current character
        if(current == inBuffer[i] || count == 0xFF){
            count++;
        }
        else{

            // greater than zero encoding
            if(count > 1){
                if(current != esc){
                    outBuffer[outIdx++] = esc;
                    outBuffer[outIdx++] = count;
                    outBuffer[outIdx++] = current;
                }
                else{
                    if(count == 2){
                        outBuffer[outIdx++] = esc;
                        outBuffer[outIdx++] = 0x01;
                    }
                }
            }
            // 1 length encoding
            else{
                if(current == esc){
                    outBuffer[outIdx++] = esc;
                    outBuffer[outIdx++] = 0;
                }
                else{
                    outBuffer[outIdx++] = current;
                }
            }
            // change to the different character
            current = inBuffer[i];
            count = 1;
        }

    }
}

void rldecode()
{

}
