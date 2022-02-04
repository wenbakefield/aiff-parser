//
//  helpers.h
//
//  Created by Mike Frengel on 2/1/21.
//

#include <math.h>

short swap2Bytes(unsigned char* bytes);
int swap4Bytes(unsigned char* bytes);
double convertFromIeeeExtended(unsigned char* bytes);


//....................................................................................................
int swap4Bytes(unsigned char* bytes)
{
    return ((int)bytes[3]<<0) | ((int)bytes[2]<<8) | ((int)bytes[1]<<16) | ((int)bytes[0]<<24);
}

//....................................................................................................
short swap2Bytes(unsigned char* bytes)
{
    return (int)bytes[1] | (int)bytes[0]<<8;
}

//....................................................................................................
# define UnsignedToFloat(u)         (((double)((long)(u - 2147483647L - 1))) + 2147483648.0)
double convertFromIeeeExtended(unsigned char* bytes)
{
    double    f;
    int    expon;
    unsigned long hiMant, loMant;
    
    expon = ((bytes[0] & 0x7F) << 8) | (bytes[1] & 0xFF);
    hiMant    =    ((unsigned long)(bytes[2] & 0xFF) << 24)
            |    ((unsigned long)(bytes[3] & 0xFF) << 16)
            |    ((unsigned long)(bytes[4] & 0xFF) << 8)
            |    ((unsigned long)(bytes[5] & 0xFF));
    loMant    =    ((unsigned long)(bytes[6] & 0xFF) << 24)
            |    ((unsigned long)(bytes[7] & 0xFF) << 16)
            |    ((unsigned long)(bytes[8] & 0xFF) << 8)
            |    ((unsigned long)(bytes[9] & 0xFF));

    if (expon == 0 && hiMant == 0 && loMant == 0)
    {
        f = 0;
    }
    else
    {
        if (expon == 0x7FFF)
        {    /* Infinity or NaN */
            f = HUGE_VAL;
        }
        else {
            expon -= 16383;
            f  = ldexp(UnsignedToFloat(hiMant), expon-=31);
            f += ldexp(UnsignedToFloat(loMant), expon-=32);
        }
    }

    if (bytes[0] & 0x80)
        return -f;
    else
        return f;
}
