#include <windows.h>
#include "rsa.h"

LPBSAFE_PUB_KEY		PUB;
LPBSAFE_PRV_KEY		PRV;

unsigned char pubmodulus[] =
{
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x2b, 0xdf, 0x91, 0x97, 0x6f, 0x5e, 0x6b, 0xdd,
0xd9, 0x32, 0x2b, 0x15, 0x2c, 0xff, 0xf9, 0xed,
0xd4, 0xd6, 0xe0, 0xc1, 0xee, 0x15, 0xcc, 0x67,
0x71, 0xd0, 0xd8, 0x2a, 0xd6, 0x05, 0x49, 0xd8,
0xd2, 0xe7, 0xa5, 0x66, 0xd3, 0x8d, 0x28, 0x32,
0x42, 0x4b, 0xc3, 0x14, 0x97, 0xe2, 0x02, 0xc6,
0x7c, 0x41, 0x3b, 0xef, 0x01, 0xc9, 0xd0, 0x12,
0x79, 0x1b, 0x47, 0xc5, 0xfe, 0xc7, 0x82, 0xe4,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

unsigned char prvmodulus[] =
{
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x2b, 0xdf, 0x91, 0x97, 0x6f, 0x5e, 0x6b, 0xdd,
0xd9, 0x32, 0x2b, 0x15, 0x2c, 0xff, 0xf9, 0xed,
0xd4, 0xd6, 0xe0, 0xc1, 0xee, 0x15, 0xcc, 0x67,
0x71, 0xd0, 0xd8, 0x2a, 0xd6, 0x05, 0x49, 0xd8,
0xd2, 0xe7, 0xa5, 0x66, 0xd3, 0x8d, 0x28, 0x32,
0x42, 0x4b, 0xc3, 0x14, 0x97, 0xe2, 0x02, 0xc6,
0x7c, 0x41, 0x3b, 0xef, 0x01, 0xc9, 0xd0, 0x12,
0x79, 0x1b, 0x47, 0xc5, 0xfe, 0xc7, 0x82, 0xe4,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x23, 0x1a, 0xb6, 0x63, 0x1d, 0x11, 0x0c, 0xd5,
0x23, 0xdc, 0x0a, 0x06, 0x56, 0x0b, 0xf3, 0x7a,
0x2a, 0x42, 0x9c, 0x6e, 0x83, 0x79, 0x28, 0xca,
0x03, 0x09, 0x8d, 0x88, 0xd7, 0x3b, 0xe6, 0xfc,
0x00, 0x00, 0x00, 0x00, 0x59, 0x23, 0x90, 0x9c,
0x7e, 0xc9, 0x5b, 0xec, 0x28, 0x64, 0xf8, 0x2e,
0x1a, 0xf0, 0xd1, 0x11, 0xb9, 0xe9, 0xb0, 0xf6,
0x51, 0x92, 0x24, 0x06, 0x6b, 0x08, 0x35, 0xbd,
0x0c, 0x00, 0x50, 0xe7, 0x00, 0x00, 0x00, 0x00,
0x4b, 0x71, 0xe8, 0x9c, 0x8e, 0xa5, 0xdc, 0x33,
0xf6, 0x20, 0x66, 0xad, 0xb8, 0x43, 0xe6, 0x37,
0xe7, 0xb3, 0x09, 0x4f, 0x1e, 0xec, 0xa5, 0xa2,
0xba, 0x55, 0x6c, 0x5d, 0x58, 0x12, 0x87, 0x41,
0x00, 0x00, 0x00, 0x00, 0x49, 0x48, 0x91, 0x4e,
0x7d, 0x93, 0x61, 0x8c, 0x98, 0xa9, 0x4b, 0x3a,
0x07, 0x8b, 0x60, 0x96, 0x59, 0x0b, 0x9c, 0xe6,
0x32, 0x6e, 0xb0, 0x77, 0xd9, 0x42, 0x88, 0x6f,
0x3f, 0xfa, 0x3b, 0xcf, 0x00, 0x00, 0x00, 0x00,
0xaf, 0xb3, 0x42, 0xea, 0xce, 0x0d, 0x18, 0xb2,
0xe5, 0x4b, 0xe1, 0xc4, 0xc6, 0xa3, 0xc3, 0x11,
0xf6, 0xae, 0x20, 0x26, 0x87, 0x71, 0x81, 0x0a,
0xc8, 0x1d, 0xc6, 0x3a, 0xed, 0x8e, 0x81, 0x60,
0x00, 0x00, 0x00, 0x00, 0xd1, 0x5c, 0x39, 0xd2,
0x9c, 0x17, 0x81, 0x18, 0x1a, 0x1b, 0x2e, 0xb5,
0x87, 0x0c, 0x2a, 0x8e, 0xb0, 0x25, 0x8a, 0xe1,
0x0b, 0xf1, 0x7a, 0x03, 0x94, 0x9f, 0x32, 0xa2,
0x29, 0x0c, 0x83, 0xfc, 0x04, 0xc8, 0xb2, 0xaa,
0x12, 0x47, 0x2c, 0x87, 0x01, 0x7f, 0x66, 0x7f,
0xdf, 0x6d, 0x98, 0xe4, 0x6b, 0x2f, 0xba, 0xc8,
0x56, 0x98, 0x93, 0x4e, 0x0c, 0x52, 0x2f, 0xf3,
0x4e, 0x42, 0x39, 0x8d, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

BOOL initkey(void)
{
    DWORD       bits;

    PUB = (LPBSAFE_PUB_KEY)pubmodulus;
    
    PUB->magic = RSA1;
    PUB->keylen = 0x48;
    PUB->bitlen = 0x0200;
    PUB->datalen = 0x3f;
    PUB->pubexp = 0x10001;

    PRV = (LPBSAFE_PRV_KEY)prvmodulus;
    PRV->magic = RSA2;
    PRV->keylen = 0x48;
    PRV->bitlen = 0x0200;
    PRV->datalen = 0x3f;
    PRV->pubexp = 0x10001;

    bits = PRV->bitlen;

    return TRUE;

}

