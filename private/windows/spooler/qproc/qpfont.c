/********************************************************************/
/**                     Microsoft LAN Manager                      **/
/**               Copyright(c) Microsoft Corp., 1987-1990          **/
/********************************************************************/

/* static char *SCCSID = "@(#)font.c	9.1 86/10/12"; */
/* SCCSID = @(#)font.c	9.1 86/10/12 */

/*
 * Font table.
 * (defines the bits in the character font)
 *
 * This is based mostly on the Windows CGAOEM font, with appropriate
 * changes for codepage 850.
 *
 * NOTE:  The characters you see in the comments mMAY NOT BE THE
 * CHARACTER REPRESENTED.  This is a representation of the codepage
 * 850 character set, and if you are not using this codepage, you may 
 * see a different set of characters in the comment fields below.
 */

unsigned char Font_Bits[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,		/* 00  */
0x7e, 0x81, 0xa5, 0x81, 0xbd, 0x99, 0x81, 0x7e,		/* 01  */
0x7e, 0xff, 0xdb, 0xff, 0xc3, 0xe7, 0xff, 0x7e,		/* 02  */
0x6c, 0xfe, 0xfe, 0xfe, 0x7c, 0x38, 0x10, 0x00,		/* 03  */
0x10, 0x38, 0x7c, 0xfe, 0x7c, 0x38, 0x10, 0x00,		/* 04  */
0x38, 0x7c, 0x38, 0xfe, 0xfe, 0x7c, 0x38, 0x7c,		/* 05  */
0x10, 0x10, 0x38, 0x7c, 0xfe, 0x7c, 0x38, 0x7c,		/* 06  */
0x00, 0x00, 0x18, 0x3c, 0x3c, 0x18, 0x00, 0x00,		/* 07  */
0xff, 0xff, 0xe7, 0xc3, 0xc3, 0xe7, 0xff, 0xff,		/* 08  */
0x00, 0x3c, 0x66, 0x42, 0x42, 0x66, 0x3c, 0x00,		/* 09  */
0xff, 0xc3, 0x99, 0xbd, 0xbd, 0x99, 0xc3, 0xff,		/* 0a  */
0x0f, 0x07, 0x0f, 0x7d, 0xcc, 0xcc, 0xcc, 0x78,		/* 0b  */
0x3c, 0x66, 0x66, 0x66, 0x3c, 0x18, 0x7e, 0x18,		/* 0c  */
0x3f, 0x33, 0x3f, 0x30, 0x30, 0x70, 0xf0, 0xe0,		/* 0d  */
0x7f, 0x63, 0x7f, 0x63, 0x63, 0x67, 0xe6, 0xc0,		/* 0e  */
0x99, 0x5a, 0x3c, 0xe7, 0xe7, 0x3c, 0x5a, 0x99,		/* 0f  */
0x80, 0xe0, 0xf8, 0xfe, 0xf8, 0xe0, 0x80, 0x00,		/* 10  */
0x02, 0x0e, 0x3e, 0xfe, 0x3e, 0x0e, 0x02, 0x00,		/* 11  */
0x18, 0x3c, 0x7e, 0x18, 0x18, 0x7e, 0x3c, 0x18,		/* 12  */
0x66, 0x66, 0x66, 0x66, 0x66, 0x00, 0x66, 0x00,		/* 13  */
0x7f, 0xdb, 0xdb, 0x7b, 0x1b, 0x1b, 0x1b, 0x00,		/* 14  */
0x3e, 0x63, 0x38, 0x6c, 0x6c, 0x38, 0xcc, 0x78,		/* 15  */
0x00, 0x00, 0x00, 0x00, 0x7e, 0x7e, 0x7e, 0x00,		/* 16  */
0x18, 0x3c, 0x7e, 0x18, 0x7e, 0x3c, 0x18, 0xff,		/* 17  */
0x18, 0x3c, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x00,		/* 18  */
0x18, 0x18, 0x18, 0x18, 0x7e, 0x3c, 0x18, 0x00,		/* 19  */
0x00, 0x18, 0x0c, 0xfe, 0x0c, 0x18, 0x00, 0x00,		/* 1a  */
0x00, 0x30, 0x60, 0xfe, 0x60, 0x30, 0x00, 0x00,		/* 1b  */
0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xfe, 0x00, 0x00,		/* 1c  */
0x00, 0x24, 0x66, 0xff, 0x66, 0x24, 0x00, 0x00,		/* 1d  */
0x00, 0x18, 0x3c, 0x7e, 0xff, 0xff, 0x00, 0x00,		/* 1e  */
0x00, 0xff, 0xff, 0x7e, 0x3c, 0x18, 0x00, 0x00,		/* 1f  */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,		/* 20  */
0x30, 0x78, 0x78, 0x30, 0x30, 0x00, 0x30, 0x00,		/* 21 ! */
0x6c, 0x6c, 0x6c, 0x00, 0x00, 0x00, 0x00, 0x00,		/* 22 " */
0x6c, 0x6c, 0xfe, 0x6c, 0xfe, 0x6c, 0x6c, 0x00,		/* 23 # */
0x30, 0x7c, 0xc0, 0x78, 0x0c, 0xf8, 0x30, 0x00,		/* 24 $ */
0x00, 0xc6, 0xcc, 0x18, 0x30, 0x66, 0xc6, 0x00,		/* 25 % */
0x38, 0x6c, 0x38, 0x76, 0xdc, 0xcc, 0x76, 0x00,		/* 26 & */
0x60, 0x60, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00,		/* 27 ' */
0x18, 0x30, 0x60, 0x60, 0x60, 0x30, 0x18, 0x00,		/* 28 ( */
0x60, 0x30, 0x18, 0x18, 0x18, 0x30, 0x60, 0x00,		/* 29 ) */
0x00, 0x66, 0x3c, 0xff, 0x3c, 0x66, 0x00, 0x00,		/* 2a * */
0x00, 0x30, 0x30, 0xfc, 0x30, 0x30, 0x00, 0x00,		/* 2b + */
0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x60,		/* 2c , */
0x00, 0x00, 0x00, 0xfc, 0x00, 0x00, 0x00, 0x00,		/* 2d - */
0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00,		/* 2e . */
0x06, 0x0c, 0x18, 0x30, 0x60, 0xc0, 0x80, 0x00,		/* 2f / */
0x7c, 0xc6, 0xce, 0xde, 0xf6, 0xe6, 0x7c, 0x00,		/* 30 0 */
0x30, 0x70, 0x30, 0x30, 0x30, 0x30, 0xfc, 0x00,		/* 31 1 */
0x78, 0xcc, 0x0c, 0x38, 0x60, 0xcc, 0xfc, 0x00,		/* 32 2 */
0x78, 0xcc, 0x0c, 0x38, 0x0c, 0xcc, 0x78, 0x00,		/* 33 3 */
0x1c, 0x3c, 0x6c, 0xcc, 0xfe, 0x0c, 0x1e, 0x00,		/* 34 4 */
0xfc, 0xc0, 0xf8, 0x0c, 0x0c, 0xcc, 0x78, 0x00,		/* 35 5 */
0x38, 0x60, 0xc0, 0xf8, 0xcc, 0xcc, 0x78, 0x00,		/* 36 6 */
0xfc, 0xcc, 0x0c, 0x18, 0x30, 0x30, 0x30, 0x00,		/* 37 7 */
0x78, 0xcc, 0xcc, 0x78, 0xcc, 0xcc, 0x78, 0x00,		/* 38 8 */
0x78, 0xcc, 0xcc, 0x7c, 0x0c, 0x18, 0x70, 0x00,		/* 39 9 */
0x00, 0x30, 0x30, 0x00, 0x00, 0x30, 0x30, 0x00,		/* 3a : */
0x00, 0x30, 0x30, 0x00, 0x00, 0x30, 0x30, 0x60,		/* 3b ; */
0x18, 0x30, 0x60, 0xc0, 0x60, 0x30, 0x18, 0x00,		/* 3c < */
0x00, 0x00, 0xfc, 0x00, 0x00, 0xfc, 0x00, 0x00,		/* 3d = */
0x60, 0x30, 0x18, 0x0c, 0x18, 0x30, 0x60, 0x00,		/* 3e > */
0x78, 0xcc, 0x0c, 0x18, 0x30, 0x00, 0x30, 0x00,		/* 3f ? */
0x7c, 0xc6, 0xde, 0xde, 0xde, 0xc0, 0x78, 0x00,		/* 40 @ */
0x30, 0x78, 0xcc, 0xcc, 0xfc, 0xcc, 0xcc, 0x00,		/* 41 A */
0xfc, 0x66, 0x66, 0x7c, 0x66, 0x66, 0xfc, 0x00,		/* 42 B */
0x3c, 0x66, 0xc0, 0xc0, 0xc0, 0x66, 0x3c, 0x00,		/* 43 C */
0xf8, 0x6c, 0x66, 0x66, 0x66, 0x6c, 0xf8, 0x00,		/* 44 D */
0xfe, 0x62, 0x68, 0x78, 0x68, 0x62, 0xfe, 0x00,		/* 45 E */
0xfe, 0x62, 0x68, 0x78, 0x68, 0x60, 0xf0, 0x00,		/* 46 F */
0x3c, 0x66, 0xc0, 0xc0, 0xce, 0x66, 0x3e, 0x00,		/* 47 G */
0xcc, 0xcc, 0xcc, 0xfc, 0xcc, 0xcc, 0xcc, 0x00,		/* 48 H */
0x78, 0x30, 0x30, 0x30, 0x30, 0x30, 0x78, 0x00,		/* 49 I */
0x1e, 0x0c, 0x0c, 0x0c, 0xcc, 0xcc, 0x78, 0x00,		/* 4a J */
0xe6, 0x66, 0x6c, 0x78, 0x6c, 0x66, 0xe6, 0x00,		/* 4b K */
0xf0, 0x60, 0x60, 0x60, 0x62, 0x66, 0xfe, 0x00,		/* 4c L */
0xc6, 0xee, 0xfe, 0xfe, 0xd6, 0xc6, 0xc6, 0x00,		/* 4d M */
0xc6, 0xe6, 0xf6, 0xde, 0xce, 0xc6, 0xc6, 0x00,		/* 4e N */
0x38, 0x6c, 0xc6, 0xc6, 0xc6, 0x6c, 0x38, 0x00,		/* 4f O */
0xfc, 0x66, 0x66, 0x7c, 0x60, 0x60, 0xf0, 0x00,		/* 50 P */
0x78, 0xcc, 0xcc, 0xcc, 0xdc, 0x78, 0x1c, 0x00,		/* 51 Q */
0xfc, 0x66, 0x66, 0x7c, 0x6c, 0x66, 0xe6, 0x00,		/* 52 R */
0x78, 0xcc, 0xe0, 0x70, 0x1c, 0xcc, 0x78, 0x00,		/* 53 S */
0xfc, 0xb4, 0x30, 0x30, 0x30, 0x30, 0x78, 0x00,		/* 54 T */
0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xfc, 0x00,		/* 55 U */
0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0x78, 0x30, 0x00,		/* 56 V */
0xc6, 0xc6, 0xc6, 0xd6, 0xfe, 0xee, 0xc6, 0x00,		/* 57 W */
0xc6, 0xc6, 0x6c, 0x38, 0x38, 0x6c, 0xc6, 0x00,		/* 58 X */
0xcc, 0xcc, 0xcc, 0x78, 0x30, 0x30, 0x78, 0x00,		/* 59 Y */
0xfe, 0xc6, 0x8c, 0x18, 0x32, 0x66, 0xfe, 0x00,		/* 5a Z */
0x78, 0x60, 0x60, 0x60, 0x60, 0x60, 0x78, 0x00,		/* 5b [ */
0xc0, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x02, 0x00,		/* 5c \ */
0x78, 0x18, 0x18, 0x18, 0x18, 0x18, 0x78, 0x00,		/* 5d ] */
0x10, 0x38, 0x6c, 0xc6, 0x00, 0x00, 0x00, 0x00,		/* 5e ^ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff,		/* 5f _ */
0x30, 0x30, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00,		/* 60 ` */
0x00, 0x00, 0x78, 0x0c, 0x7c, 0xcc, 0x76, 0x00,		/* 61 a */
0xe0, 0x60, 0x60, 0x7c, 0x66, 0x66, 0xdc, 0x00,		/* 62 b */
0x00, 0x00, 0x78, 0xcc, 0xc0, 0xcc, 0x78, 0x00,		/* 63 c */
0x1c, 0x0c, 0x0c, 0x7c, 0xcc, 0xcc, 0x76, 0x00,		/* 64 d */
0x00, 0x00, 0x78, 0xcc, 0xfc, 0xc0, 0x78, 0x00,		/* 65 e */
0x38, 0x6c, 0x60, 0xf0, 0x60, 0x60, 0xf0, 0x00,		/* 66 f */
0x00, 0x00, 0x76, 0xcc, 0xcc, 0x7c, 0x0c, 0xf8,		/* 67 g */
0xe0, 0x60, 0x6c, 0x76, 0x66, 0x66, 0xe6, 0x00,		/* 68 h */
0x30, 0x00, 0x70, 0x30, 0x30, 0x30, 0x78, 0x00,		/* 69 i */
0x0c, 0x00, 0x0c, 0x0c, 0x0c, 0xcc, 0xcc, 0x78,		/* 6a j */
0xe0, 0x60, 0x66, 0x6c, 0x78, 0x6c, 0xe6, 0x00,		/* 6b k */
0x70, 0x30, 0x30, 0x30, 0x30, 0x30, 0x78, 0x00,		/* 6c l */
0x00, 0x00, 0xcc, 0xfe, 0xfe, 0xd6, 0xc6, 0x00,		/* 6d m */
0x00, 0x00, 0xf8, 0xcc, 0xcc, 0xcc, 0xcc, 0x00,		/* 6e n */
0x00, 0x00, 0x78, 0xcc, 0xcc, 0xcc, 0x78, 0x00,		/* 6f o */
0x00, 0x00, 0xdc, 0x66, 0x66, 0x7c, 0x60, 0xf0,		/* 70 p */
0x00, 0x00, 0x76, 0xcc, 0xcc, 0x7c, 0x0c, 0x1e,		/* 71 q */
0x00, 0x00, 0xdc, 0x76, 0x66, 0x60, 0xf0, 0x00,		/* 72 r */
0x00, 0x00, 0x7c, 0xc0, 0x78, 0x0c, 0xf8, 0x00,		/* 73 s */
0x10, 0x30, 0x7c, 0x30, 0x30, 0x34, 0x18, 0x00,		/* 74 t */
0x00, 0x00, 0xcc, 0xcc, 0xcc, 0xcc, 0x76, 0x00,		/* 75 u */
0x00, 0x00, 0xcc, 0xcc, 0xcc, 0x78, 0x30, 0x00,		/* 76 v */
0x00, 0x00, 0xc6, 0xd6, 0xfe, 0xfe, 0x6c, 0x00,		/* 77 w */
0x00, 0x00, 0xc6, 0x6c, 0x38, 0x6c, 0xc6, 0x00,		/* 78 x */
0x00, 0x00, 0xcc, 0xcc, 0xcc, 0x7c, 0x0c, 0xf8,		/* 79 y */
0x00, 0x00, 0xfc, 0x98, 0x30, 0x64, 0xfc, 0x00,		/* 7a z */
0x1c, 0x30, 0x30, 0xe0, 0x30, 0x30, 0x1c, 0x00,		/* 7b { */
0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00,		/* 7c | */
0xe0, 0x30, 0x30, 0x1c, 0x30, 0x30, 0xe0, 0x00,		/* 7d } */
0x76, 0xdc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,		/* 7e ~ */
0x00, 0x10, 0x38, 0x6c, 0xc6, 0xc6, 0xfe, 0x00,         /* 7f   */
0x3c, 0x66, 0xc0, 0xc0, 0xc0, 0x66, 0x3c, 0xf0,		/* 80 � */
0xcc, 0x00, 0xcc, 0xcc, 0xcc, 0xcc, 0x76, 0x00,		/* 81 � */
0x1c, 0x00, 0x78, 0xcc, 0xfc, 0xc0, 0x78, 0x00,		/* 82 � */
0x7e, 0xc3, 0x78, 0x0c, 0x7c, 0xcc, 0x76, 0x00,		/* 83 � */
0xcc, 0x00, 0x78, 0x0c, 0x7c, 0xcc, 0x76, 0x00,		/* 84 � */
0xe0, 0x00, 0x78, 0x0c, 0x7c, 0xcc, 0x76, 0x00,		/* 85 � */
0x30, 0x30, 0x78, 0x0c, 0x7c, 0xcc, 0x76, 0x00,		/* 86 � */
0x00, 0x00, 0x78, 0xcc, 0xc0, 0xcc, 0x78, 0xf0,		/* 87 � */
0x7e, 0xc3, 0x78, 0xcc, 0xfc, 0xc0, 0x78, 0x00,		/* 88 � */
0xcc, 0x00, 0x78, 0xcc, 0xfc, 0xc0, 0x78, 0x00,		/* 89 � */
0xe0, 0x00, 0x78, 0xcc, 0xfc, 0xc0, 0x78, 0x00,		/* 8a � */
0xcc, 0x00, 0x70, 0x30, 0x30, 0x30, 0x78, 0x00,		/* 8b � */
0x7e, 0xc3, 0x70, 0x30, 0x30, 0x30, 0x78, 0x00,		/* 8c � */
0xe0, 0x00, 0x70, 0x30, 0x30, 0x30, 0x78, 0x00,		/* 8d � */
0xcc, 0x30, 0x78, 0xcc, 0xcc, 0xfc, 0xcc, 0x00,		/* 8e � */
0xcc, 0xcc, 0x30, 0x78, 0xcc, 0xfc, 0xcc, 0x00,		/* 8f � */
0x1c, 0x00, 0xfc, 0x60, 0x78, 0x60, 0xfc, 0x00,         /* 90 � */
0x00, 0x00, 0x7f, 0x0c, 0x7f, 0xcc, 0x7f, 0x00,         /* 91 � */
0x3e, 0x6c, 0xcc, 0xfe, 0xcc, 0xcc, 0xce, 0x00,         /* 92 � */
0x78, 0xcc, 0x00, 0x78, 0xcc, 0xcc, 0x78, 0x00,         /* 93 � */
0x00, 0xcc, 0x00, 0x78, 0xcc, 0xcc, 0x78, 0x00,         /* 94 � */
0x00, 0xe0, 0x00, 0x78, 0xcc, 0xcc, 0x78, 0x00,         /* 95 � */
0x78, 0xcc, 0x00, 0xcc, 0xcc, 0xcc, 0x7e, 0x00,         /* 96 � */
0x00, 0xc0, 0x00, 0xcc, 0xcc, 0xcc, 0x7e, 0x00,         /* 97 � */
0x00, 0xcc, 0x00, 0xcc, 0xcc, 0xfc, 0x0c, 0xf8,         /* 98 � */
0xc3, 0x18, 0x3c, 0x66, 0x66, 0x3c, 0x18, 0x00,         /* 99 � */
0xcc, 0x00, 0xcc, 0xcc, 0xcc, 0xcc, 0x78, 0x00,         /* 9a � */
0x00, 0x00, 0x34, 0x48, 0x54, 0x24, 0x58, 0x00,         /* 9b � */
0x38, 0x6c, 0x64, 0xf0, 0x60, 0xe6, 0xfc, 0x00,         /* 9c � */
0x00, 0x3a, 0x44, 0x4a, 0x62, 0x22, 0x5c, 0x00,         /* 9d � */
0x00, 0x00, 0x44, 0x28, 0x10, 0x28, 0x44, 0x00,         /* 9e � */
0x0c, 0x1b, 0x18, 0x7c, 0x18, 0x18, 0xd8, 0x70,         /* 9f � */
0x1c, 0x00, 0x78, 0x0c, 0x7c, 0xcc, 0x76, 0x00,		/* a0 � */
0x38, 0x00, 0x70, 0x30, 0x30, 0x30, 0x78, 0x00,         /* a1 � */
0x00, 0x1c, 0x00, 0x78, 0xcc, 0xcc, 0x78, 0x00,         /* a2 � */
0x00, 0x1c, 0x00, 0xcc, 0xcc, 0xcc, 0x7e, 0x00,         /* a3 � */
0x00, 0xf8, 0x00, 0xf8, 0xcc, 0xcc, 0xcc, 0x00,         /* a4 � */
0xfc, 0x00, 0xcc, 0xec, 0xfc, 0xdc, 0xcc, 0x00,         /* a5 � */
0x3c, 0x6c, 0x6c, 0x3e, 0x00, 0x7e, 0x00, 0x00,         /* a6 � */
0x3c, 0x66, 0x66, 0x3c, 0x00, 0x7e, 0x00, 0x00,         /* a7 � */
0x0c, 0x00, 0x0c, 0x18, 0x30, 0x33, 0x1e, 0x00,		/* a8 � */
0x00, 0x00, 0x00, 0xfc, 0xc0, 0xc0, 0x00, 0x00,         /* a9 � */
0x00, 0x00, 0x00, 0xfc, 0x0c, 0x0c, 0x00, 0x00,         /* aa � */
0xc6, 0xcc, 0xd8, 0x3e, 0x63, 0xce, 0x98, 0x1f,         /* ab � */
0xc6, 0xcc, 0xd8, 0xf3, 0x67, 0xcf, 0x9f, 0x03,         /* ac � */
0x30, 0x00, 0x30, 0x30, 0x78, 0x78, 0x30, 0x00,		/* ad � */
0x00, 0x12, 0x24, 0x48, 0x24, 0x12, 0x00, 0x00,         /* ae � */
0x00, 0x48, 0x24, 0x12, 0x24, 0x48, 0x00, 0x00,         /* af � */
0x22, 0x88, 0x22, 0x88, 0x22, 0x88, 0x22, 0x88,         /* b0 � */
0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa,         /* b1 � */
0xdd, 0x77, 0xdd, 0x77, 0xdd, 0x77, 0xdd, 0x77,         /* b2 � */
0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00,         /* b3 � */
0x08, 0x08, 0x08, 0xf8, 0x08, 0x08, 0x08, 0x00,         /* b4 � */
0x1e, 0x00, 0x30, 0x78, 0xcc, 0xfc, 0xcc, 0x00,		/* b5 � */
0x7c, 0x82, 0x30, 0x78, 0xcc, 0xfc, 0xcc, 0x00,		/* b6 � */
0xf0, 0x00, 0x30, 0x78, 0xcc, 0xfc, 0xcc, 0x00,		/* b7 � */
0x3c, 0x42, 0x99, 0xa1, 0xa1, 0x99, 0x42, 0x3c,         /* b8 � */
0x36, 0x36, 0xf6, 0x00, 0xf6, 0x36, 0x36, 0x00,         /* b9 � */
0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x00,         /* ba � */
0x00, 0x00, 0xfe, 0x06, 0xf6, 0x36, 0x36, 0x00,         /* bb � */
0x36, 0x36, 0xf6, 0x06, 0xfe, 0x00, 0x00, 0x00,         /* bc � */
0x18, 0x18, 0x7e, 0xd8, 0xd8, 0x7e, 0x18, 0x18,         /* bd � */
0xcc, 0xcc, 0x78, 0xfc, 0x30, 0xfc, 0x30, 0x30,         /* be � */
0x00, 0x00, 0x00, 0xf8, 0x08, 0x08, 0x08, 0x00,         /* bf � */
0x08, 0x08, 0x08, 0x0f, 0x00, 0x00, 0x00, 0x00,         /* c0 � */
0x08, 0x08, 0x08, 0xff, 0x00, 0x00, 0x00, 0x00,         /* c1 � */
0x00, 0x00, 0x00, 0xff, 0x08, 0x08, 0x08, 0x00,         /* c2 � */
0x08, 0x08, 0x08, 0x0f, 0x08, 0x08, 0x08, 0x00,         /* c3 � */
0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00,         /* c4 � */
0x08, 0x08, 0x08, 0xff, 0x08, 0x08, 0x08, 0x00,         /* c5 � */
0x36, 0x4c, 0x00, 0x38, 0x04, 0x3c, 0x44, 0x3a,         /* c6 � */
0x36, 0x4c, 0x00, 0x3c, 0x66, 0x7e, 0x66, 0x00,         /* c7 � */
0x36, 0x36, 0x37, 0x30, 0x3f, 0x00, 0x00, 0x00,         /* c8 � */
0x00, 0x00, 0x3f, 0x30, 0x37, 0x36, 0x36, 0x00,         /* c9 � */
0x36, 0x36, 0xf7, 0x00, 0xff, 0x00, 0x00, 0x00,         /* ca � */
0x00, 0x00, 0xff, 0x00, 0xf7, 0x36, 0x36, 0x00,         /* cb � */
0x36, 0x36, 0x37, 0x30, 0x37, 0x36, 0x36, 0x00,         /* cc � */
0x00, 0x00, 0xff, 0x00, 0xff, 0x00, 0x00, 0x00,         /* cd � */
0x36, 0x36, 0xf7, 0x00, 0xf7, 0x36, 0x36, 0x00,         /* ce � */
0x40, 0x3a, 0x44, 0x44, 0x44, 0xb8, 0x04, 0x00,         /* cf � */
0x36, 0x4c, 0x00, 0x38, 0x44, 0x44, 0x38, 0x00,         /* d0 � */
0xf8, 0x4c, 0x44, 0xf4, 0x44, 0x4c, 0xf8, 0x00,         /* d1 � */
0x7e, 0x81, 0x3c, 0x20, 0x38, 0x20, 0x3c, 0x00,         /* d2 � */
0x66, 0x00, 0x3c, 0x20, 0x38, 0x20, 0x3c, 0x00,         /* d3 � */
0x70, 0x00, 0x3c, 0x20, 0x38, 0x20, 0x3c, 0x00,         /* d4 � */
0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,         /* d5 � */
0x0e, 0x00, 0x3c, 0x18, 0x18, 0x18, 0x3c, 0x00,         /* d6 � */
0x7e, 0x81, 0x3c, 0x18, 0x18, 0x18, 0x3c, 0x00,         /* d7 � */
0x66, 0x00, 0x3c, 0x18, 0x18, 0x18, 0x3c, 0x00,         /* d8 � */
0x08, 0x08, 0x08, 0xf8, 0x00, 0x00, 0x00, 0x00,         /* d9 � */
0x00, 0x00, 0x00, 0x0f, 0x08, 0x08, 0x08, 0x00,         /* da � */
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,         /* db � */
0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,         /* dc � */
0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0,         /* dd � */
0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,         /* de � */
0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,         /* df � */
0x0e, 0x00, 0x38, 0x44, 0x44, 0x44, 0x38, 0x00,         /* e0 � */
0x78, 0xcc, 0xcc, 0xf8, 0xcc, 0xcc, 0xf8, 0xc0,         /* e1 � */
0x7c, 0x82, 0x38, 0x44, 0x44, 0x44, 0x38, 0x00,         /* e2 � */
0x70, 0x00, 0x38, 0x44, 0x44, 0x44, 0x38, 0x00,         /* e3 � */
0x32, 0x4c, 0x00, 0x30, 0x48, 0x48, 0x30, 0x00,         /* e4 � */
0x32, 0x4c, 0x38, 0x44, 0x44, 0x44, 0x38, 0x00,         /* e5 � */
0x00, 0x66, 0x66, 0x66, 0x7e, 0x3c, 0x26, 0x20,         /* e6 � */
0x60, 0x38, 0x24, 0x24, 0x38, 0x20, 0x70, 0x00,         /* e7 � */
0xf0, 0x60, 0x7c, 0x66, 0x66, 0x7c, 0x60, 0xf0,         /* e8 � */
0x1c, 0x44, 0x00, 0x44, 0x44, 0x44, 0x38, 0x00,         /* e9 � */
0x38, 0x44, 0x00, 0x44, 0x44, 0x44, 0x38, 0x00,         /* ea � */
0x70, 0x44, 0x00, 0x44, 0x44, 0x44, 0x38, 0x00,         /* eb � */
0x0e, 0x00, 0x22, 0x22, 0x1c, 0x04, 0x38, 0x00,         /* ec � */
0x0e, 0x00, 0x6c, 0x28, 0x38, 0x10, 0x38, 0x00,         /* ed � */
0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,         /* ee � */
0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,         /* ef � */
0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00,         /* f0 � */
0x00, 0x30, 0x30, 0xfc, 0x30, 0x30, 0xfc, 0x00,		/* f1 � */
0x00, 0x00, 0xfc, 0x00, 0x00, 0xfc, 0x00, 0x00,		/* f2 = */
0xf1, 0x12, 0xf4, 0x1e, 0xf1, 0x27, 0x48, 0x8f,         /* f3 � */
0x00, 0x00, 0x3c, 0x60, 0x60, 0x20, 0x00, 0x00,         /* f4 � */
0x1c, 0x22, 0x18, 0x24, 0x24, 0x18, 0x44, 0x38,         /* f5 � */
0x30, 0x30, 0x00, 0xfc, 0x00, 0x30, 0x30, 0x00,         /* f6 � */
0x00, 0x00, 0x00, 0x00, 0x4c, 0x3c, 0x00, 0x00,         /* f7 � */
0x38, 0x6c, 0x6c, 0x38, 0x00, 0x00, 0x00, 0x00,         /* f8 � */
0x00, 0x00, 0x00, 0x4c, 0x4c, 0x00, 0x00, 0x00,         /* f9 � */
0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00,         /* fa � */
0x10, 0x30, 0x10, 0x10, 0x38, 0x00, 0x00, 0x00,         /* fb � */
0x30, 0x48, 0x18, 0x48, 0x30, 0x00, 0x00, 0x00,         /* fc � */
0x78, 0x0c, 0x38, 0x60, 0x7c, 0x00, 0x00, 0x00,         /* fd � */
0x00, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x00,         /* fe � */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00          /* ff   */
};
