/*

$Log:   S:\oiwh\filing\fiopcx.c_v  $
 * 
 *    Rev 1.13   05 Feb 1996 14:38:24   RWR
 * Eliminate static links to OIDIS400 and OIADM400 for NT builds
 * 
 *    Rev 1.12   09 Jan 1996 14:14:14   RWR
 * Add logic to check for empty buffer at start of ExpandPcxLine() routine
 * (if prior call exactly cleared the buffer, this call would read bad memory)
 * 
 *    Rev 1.11   05 Oct 1995 17:18:36   RWR
 * Rewrite PlaneToColor() assembler routine in "C" for NT platforms
 * 
 *    Rev 1.10   28 Sep 1995 10:12:22   JAR
 * fixed rounding error when reading 24-bit pcx images
 * 
 *    Rev 1.9   13 Sep 1995 17:14:54   RWR
 * Preliminary checkin of conditional code supporting new compress/expand calls
 * 
 *    Rev 1.8   13 Jul 1995 10:42:50   RWR
 * Add logic to restore original lpCompressData pointer
 * 
 *    Rev 1.7   12 Jul 1995 16:57:24   RWR
 * Switch from \include\oiutil.h to (private) \filing\fileutil.h
 * 
 *    Rev 1.6   12 Jul 1995 10:24:30   RWR
 * Change display.h header (#include) to engdisp.h
 * 
 *    Rev 1.5   23 Jun 1995 10:39:54   RWR
 * Change "wiisfio2.h" include file to "filing.h"
 * 
 *    Rev 1.4   09 May 1995 13:21:28   RWR
 * #include file modifications to match changes to oiadm.h/admin.h/privapis.h
 * 
 *    Rev 1.3   24 Apr 1995 15:41:30   JCW
 * Removed the oiuidll.h.
 * Rename wiissubs.h to oiutil.h.
 * 
 *    Rev 1.2   20 Apr 1995 12:25:44   RWR
 * Modify PlaneToColor() assembler code to work properly in 32-bit environment
 * 
 *    Rev 1.1   14 Apr 1995 22:14:04   JAR
 * massaged to get compilation under windows 95
 * 
 *    Rev 1.0   06 Apr 1995 13:55:50   JAR
 * Initial entry

*/
 
#include "abridge.h"
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <memory.h>
#include "wic.h"
#include "oifile.h"
#include "oicmp.h"
#include "filing.h"
#include "fileutil.h"
#include "oierror.h"
#include "engdisp.h"

typedef struct
{
  char *pLine;
  char *pOutLine;
  short linbytes;
} PCX_DATA, FAR *LP_PCX_DATA;

typedef struct
{
  unsigned carrycnt;
  char carryval;
  short bpl;
} PCX_EXPAND_DATA, FAR *LP_PCX_EXPAND_DATA;

static void NEAR PASCAL ExpandPcxOpen(LPCXDATA lpCX, short pcxbpl, LP_PCX_DATA lpPcxData,
                                      LP_PCX_EXPAND_DATA lpPcxExpandData);
static int  NEAR PASCAL ExpandPcxData(HWND hWnd, LPCXDATA lpCX,
                                      lp_INFO lpGFSInfo, LP_FIO_DATA pdata, LP_PCX_DATA lpPcxData,
                                      LP_PCX_EXPAND_DATA lpPcxExpandData);
static void NEAR PASCAL ExpandPcxLine(HWND hWnd, LPCXDATA lpCX,
                                      lp_INFO lpGFSInfo, LP_FIO_DATA pdata, 
                                      LP_PCX_DATA lpPcxData, LP_PCX_EXPAND_DATA lpPcxExpandData);
static void near pascal PlaneToColor(LPSTR lpSrc, LPSTR lpDest, unsigned SrcWidth,
                                     unsigned DestWidth,int Planes);
static void near pascal MyPlaneToColor(LPSTR lpSrc, LPSTR lpDest, unsigned SrcWidth,
                                       int bytes_per_line);

WORD ReadPcx(HWND hWnd, lp_INFO lpGFSInfo, LPCXDATA lpcxdata,
             LP_FIO_DATA pdata, LPSTR lpDest, LPSTR lpSrc,
             LPINT Line, LPINT LineCount)
{
    WORD   status = 0;
    int    errcode = 0;
    long   bytesread = 0;
    HLOCAL hLine;
    unsigned long strip_size;
    PCX_DATA PcxData;
    HANDLE hPcxExpandData;
    LP_PCX_EXPAND_DATA lpPcxExpandData;
    hLine = LocalAlloc(LMEM_FIXED, lpGFSInfo->_file.fmt.pcx.bpl * 2);
    if (hLine == 0)
    {
        status = FIO_LOCAL_ALLOC_FAILED;
        return (status);
    }
    
    PcxData.pLine = (char *) LocalLock(hLine);
    PcxData.pOutLine = PcxData.pLine;
    PcxData.linbytes = lpGFSInfo->_file.fmt.pcx.bpl;

    lpcxdata->lpExpandData = lpDest;
    lpcxdata->ExpandLines = *LineCount;
    lpcxdata->LinesToSkip = 0;
         
    lpcxdata->Status = 0;

    if (pdata->CmpBufEmpty)
        strip_size = (unsigned long) pdata->CmpBuffersize;
    else
        strip_size = min(pdata->CmpBuffersize, pdata->bytes_left);

    while (lpcxdata->Status != c_erro)
    {
        if (pdata->CmpBufEmpty)
        {
            if ((bytesread = wgfsread(hWnd, (int) pdata->filedes, (char far *) lpSrc,
                                      (long) pdata->start_byte, strip_size,
				      &(pdata->bytes_left),
				      (unsigned short)pdata->pgnum, &errcode)) < 0)
            {
                return errcode;
            }
            else
            {
                lpcxdata->CompressBytes = (unsigned int) bytesread;
                lpcxdata->lpCompressData = lpSrc;
                // 7/13/95  rwr  Save buffer address for later reloads
                lpcxdata->lpCompressData0 = lpSrc;
                pdata->CmpBufEmpty = FALSE;
                if ( (hPcxExpandData = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT,  sizeof(PCX_EXPAND_DATA))) == NULL)
                {
                    LocalUnlock(hLine);
                    LocalFree(hLine);
                    return(FIO_GLOBAL_ALLOC_FAILED);
                }
                if( (lpPcxExpandData = (LP_PCX_EXPAND_DATA) GlobalLock(hPcxExpandData)) == NULL)
                {
                    LocalUnlock(hLine);
                    LocalFree(hLine);
                    GlobalFree(hPcxExpandData);
                    return(FIO_GLOBAL_LOCK_FAILED);
                }
                if (pdata->start_byte == 0)
                    ExpandPcxOpen (lpcxdata, lpGFSInfo->_file.fmt.pcx.bpl, (LP_PCX_DATA)&PcxData,
                    lpPcxExpandData);

                FioSetProp(hWnd, "PCX_EXPAND_DATA", hPcxExpandData);
                pdata->start_byte += bytesread;
                strip_size = min(strip_size, pdata->bytes_left);
                lpcxdata->Status = 0;
            }
        }
        hPcxExpandData = FioGetProp(hWnd, "PCX_EXPAND_DATA");
        lpPcxExpandData = (LP_PCX_EXPAND_DATA)GlobalLock(hPcxExpandData);
        if (lpPcxExpandData == NULL)
        {
            LocalUnlock(hLine);
            LocalFree(hLine);
            GlobalFree(hPcxExpandData);
            return(FIO_GLOBAL_LOCK_FAILED);
        }
        *Line += ExpandPcxData (hWnd, lpcxdata, lpGFSInfo, pdata, (LP_PCX_DATA)&PcxData,
        lpPcxExpandData);
        /* see if we've reached the end of the image */
        if (*Line >= (int) lpGFSInfo->vert_size)
            FioRemoveProp(hWnd, "PCX_EXPAND_DATA");

        if (lpcxdata->Status != c_erri)
            break;
        pdata->CmpBufEmpty = TRUE;
        FioRemoveProp(hWnd, "PCX_EXPAND_DATA");
    }

    LocalUnlock(hLine);
    LocalFree(hLine);
    return (status);
}

static void NEAR PASCAL ExpandPcxOpen(LPCXDATA lpCX, short pcxbpl, 
LP_PCX_DATA lpPcxData, LP_PCX_EXPAND_DATA lpPcxExpandData)
{
    lpPcxExpandData->carrycnt = 0;
    lpPcxExpandData->carryval = 0;
    lpPcxExpandData->bpl = pcxbpl;
    lpPcxData->linbytes = lpPcxExpandData->bpl;
}

static int NEAR PASCAL  ExpandPcxData(HWND hWnd, LPCXDATA lpCX,
                                      lp_INFO lpGFSInfo, LP_FIO_DATA pdata,
                                      LP_PCX_DATA lpPcxData,
                                      LP_PCX_EXPAND_DATA lpPcxExpandData)
{
    int LinesToDo = lpCX->ExpandLines;
    int i;
    int bytes_per_line; /* KMC - new variable, # bytes per scan line */

    /* KMC - actual bytes_per_line is equal to (...bpl)/(...planes) 
       because in pcxinfo(...) which gets this info, bpl is given as follows: 
       lpGFSInfo->_file.fmt.pcx.bpl = (actual bpl)*(number of planes)
    */

    if ( lpGFSInfo->_file.fmt.pcx.planes == 3)
	{
	bytes_per_line = lpGFSInfo->horiz_size;
	}
    else
	{
	bytes_per_line = lpGFSInfo->_file.fmt.pcx.bpl /
			 lpGFSInfo->_file.fmt.pcx.planes;
	}

    while (lpCX->ExpandLines && lpCX->Status != c_erri)
    {
        ExpandPcxLine(hWnd, lpCX, lpGFSInfo, pdata, lpPcxData, lpPcxExpandData);
        if (lpCX->Status != c_erri)
        {
            if (lpGFSInfo->_file.fmt.pcx.planes > 1)
            {
                if (lpGFSInfo->_file.fmt.pcx.planes == 3) /* 24 bit color */
                {
                    /* KMC - now call MyPlaneToColor instead of PlaneToColor
                       because PlaneToColor is for decoding 4 bit (16 color)
                       images, not 24 bit images.
                    */
                    MyPlaneToColor(lpPcxData->pLine, lpCX->lpExpandData,
                                   bytes_per_line,
                                   bytes_per_line);
                }
                else 
                {
                    PlaneToColor(lpPcxData->pLine, lpCX->lpExpandData,
                                 bytes_per_line,
                                 lpCX->BufferByteWidth,
                                 lpGFSInfo->_file.fmt.pcx.planes);
                }
                lpCX->lpExpandData += lpCX->BufferByteWidth;
            }
            else
            {
                for (i = 0; i < lpCX->BufferByteWidth; i++)
                    *lpCX->lpExpandData++ = lpPcxData->pLine[i];
            }
            lpPcxData->pOutLine = lpPcxData->pLine;
            lpPcxData->linbytes = lpPcxExpandData->bpl;
            if (!( lpCX->ExpandLines-- ))
                lpCX->Status = c_erro;
        }
        else break;
    }
    return LinesToDo - lpCX->ExpandLines;
}

static void NEAR PASCAL ExpandPcxLine(HWND hWnd, LPCXDATA lpCX,
                                      lp_INFO lpGFSInfo, LP_FIO_DATA pdata,
                                      LP_PCX_DATA lpPcxData,
                                      LP_PCX_EXPAND_DATA lpPcxExpandData)
{
    short inbytes;
    int   errcode = 0;
    long  bytesread = 0;

    if (lpPcxExpandData->carrycnt)
    {
        inbytes = min((short) lpPcxExpandData->carrycnt, lpPcxData->linbytes);
        lpPcxExpandData->carrycnt -= inbytes;
        lpPcxData->linbytes -= inbytes;
        while (inbytes--)
            *lpPcxData->pOutLine++ = lpPcxExpandData->carryval;
    }
    
    /* We may need to preload another data buffer! */
    if ((lpPcxData->linbytes > 0) && (lpCX->CompressBytes == 0))
     {
      lpCX->lpCompressData = lpCX->lpCompressData0;
      if ((bytesread = wgfsread( hWnd, (int)pdata->filedes,
                    lpCX->lpCompressData, (long)pdata->start_byte,
                    min(pdata->CmpBuffersize, pdata->bytes_left),
		    &(pdata->bytes_left), (unsigned short)pdata->pgnum,
		    &errcode)) < 0)
        {
         lpCX->Status = c_erri;
         return;
        }
      else
        {
         lpCX->CompressBytes = (unsigned int) bytesread;
         lpCX->lpCompressData = lpCX->lpCompressData0;
         pdata->CmpBufEmpty = FALSE;
         pdata->start_byte += bytesread;
         lpCX->Status = 0;
        }
     }

    /* Now we can process the line */
    while(lpPcxData->linbytes > 0)
    {
        inbytes = *lpCX->lpCompressData++;
        if ((inbytes & 0xc0) != 0xc0)
        {
            *lpPcxData->pOutLine++ = (char)inbytes;
            lpPcxData->linbytes--;
            if (!(lpCX->CompressBytes -= 1))
            {
                if (lpPcxData->linbytes == 0)
		    break;
		// 9504.13 jar I don't think we need to set offset to 0
		//FP_OFF (lpCX->lpCompressData) = 0;
                lpCX->lpCompressData = lpCX->lpCompressData0;

                if ((bytesread = wgfsread( hWnd, (int)pdata->filedes,
                    lpCX->lpCompressData, (long)pdata->start_byte,
                    min(pdata->CmpBuffersize, pdata->bytes_left),
		    &(pdata->bytes_left), (unsigned short)pdata->pgnum,
		    &errcode)) < 0)
                {
                    lpCX->Status = c_erri;
                    return;
                }
                else
                {
		    lpCX->CompressBytes = (unsigned int) bytesread;
		    // 9504.13 jar I don't think we need to set offset to 0
		    //FP_OFF (lpCX->lpCompressData) = 0;
                    lpCX->lpCompressData = lpCX->lpCompressData0;
                    pdata->CmpBufEmpty = FALSE;
                    pdata->start_byte += bytesread;
                    lpCX->Status = 0;
                }
            }
        }
        else
        {
            if (!(lpCX->CompressBytes -= 1))
            {
                if (lpPcxData->linbytes == 0)
		    break;
		// 9504.13 jar I don't think we need to set offset to 0
		//FP_OFF (lpCX->lpCompressData) = 0;
                lpCX->lpCompressData = lpCX->lpCompressData0;
                if ((bytesread = wgfsread(hWnd, (int)pdata->filedes,
                    lpCX->lpCompressData, (long)pdata->start_byte,
                    min(pdata->CmpBuffersize, pdata->bytes_left),
		    &(pdata->bytes_left), (unsigned short)pdata->pgnum,
		    &errcode)) < 0)
                {
                    lpCX->Status = c_erri;
                    return;
                }
                else
                {
		    lpCX->CompressBytes = (unsigned int) bytesread;
		    // 9504.13 jar I don't think we need to set offset to 0
		    //FP_OFF (lpCX->lpCompressData) = 0;
                    lpCX->lpCompressData = lpCX->lpCompressData0;
                    pdata->CmpBufEmpty = FALSE;
                    pdata->start_byte += bytesread;
                    lpCX->Status = 0;
                }
            }
            if ((inbytes &= 0x3f) > lpPcxData->linbytes)
            {
                lpPcxExpandData->carrycnt = inbytes - lpPcxData->linbytes;
                lpPcxExpandData->carryval = *lpCX->lpCompressData;
                inbytes = lpPcxData->linbytes;
            }
            lpPcxData->linbytes -= inbytes;
            while (inbytes--)
                *lpPcxData->pOutLine++ = *lpCX->lpCompressData;
            lpCX->lpCompressData++;
            if (!(lpCX->CompressBytes -= 1))
            {
                if (lpPcxData->linbytes == 0)
		    break;
		// 9504.13 jar I don't think we need to set offset to 0
		//FP_OFF (lpCX->lpCompressData) = 0;
                lpCX->lpCompressData = lpCX->lpCompressData0;
                if ((bytesread = wgfsread(hWnd, (int)pdata->filedes,
                    lpCX->lpCompressData, (long)pdata->start_byte,
                    min(pdata->CmpBuffersize, pdata->bytes_left),
		    &(pdata->bytes_left), (unsigned short)pdata->pgnum,
		    &errcode)) < 0)
                {
                    lpCX->Status = c_erri;
                    return;
                }
                else
                {
                    lpCX->CompressBytes = (unsigned int) bytesread;
                    pdata->CmpBufEmpty = FALSE;
                    pdata->start_byte += bytesread;
                    lpCX->Status = 0;
                }
            }
        }
    }
}

#define RGB_RED    0
#define RGB_GREEN  1
#define RGB_BLUE   2
#define RGB_SIZE   3

/* NEW FUNCTION: (3.7)

   FUNCTION: MyPlaneToColor

   DESCRIPTION:
   The following function decodes the color information from 24 bit PCX image
   files. Three scan lines (one for each plane) are given and the first byte 
   of each is combined to form a single RGB triplet rather than a palette value.

   INPUT:
   LPSTR lpSrc: Pointer to the buffer containing the encoded scan lines.
   LPSTR lpDest: Pointer to the buffer which will contain the decoded output.
   unsigned SrcWidth: The width in bytes of the source buffer.
   int bytes_per_line: The number of bytes for each source scan line.

   OUTPUT:
   Nothing is returned. The decoded data is stored in the buffer pointed to by
   lpDest.
*/
static void near pascal MyPlaneToColor(LPSTR lpSrc, LPSTR lpDest,
                                       unsigned SrcWidth, int bytes_per_line)
{
    int i;    
    for (i = 0; (unsigned)i < SrcWidth; ++i)
    {
        lpDest[i*RGB_SIZE+RGB_RED] = lpSrc[i];
        lpDest[i*RGB_SIZE+RGB_GREEN] = lpSrc[RGB_GREEN*bytes_per_line+i];
        lpDest[i*RGB_SIZE+RGB_BLUE] = lpSrc[RGB_BLUE*bytes_per_line+i];
    }
}

/**************************************************************************/
/*  PlaneToColor()                                                        */
/*    Function to convert 2-, 3- or 4-plane PCX files to linear format    */
/**************************************************************************/

static void near pascal PlaneToColor(LPSTR lpSrc, LPSTR lpDest,
                                     unsigned SrcWidth, unsigned DestWidth,
                                     int Planes)
{

 /* Here's the PCX image plane translation table.
    It translates one byte from the "Red" plane (#1) to the resulting
    8 nibbles (4 bytes), one nibble per original "Red" bit. Translations
    for the "Green" (#2), "Blue" (#3) and "Intensity" (#4) planes use the
    same translation values shifted left 1, 2 and 3 bits, respectively.
    Appropriate (?) substitutions/defaults are made for images containing
    fewer than 4 planes (see code). */

 static unsigned long table[256]
            = { 0x00000000, 0x01000000, 0x10000000, 0x11000000,
                0x00010000, 0x01010000, 0x10010000, 0x11010000,
                0x00100000, 0x01100000, 0x10100000, 0x11100000,
                0x00110000, 0x01110000, 0x10110000, 0x11110000,
                0x00000100, 0x01000100, 0x10000100, 0x11000100,
                0x00010100, 0x01010100, 0x10010100, 0x11010100,
                0x00100100, 0x01100100, 0x10100100, 0x11100100,
                0x00110100, 0x01110100, 0x10110100, 0x11110100,
                0x00001000, 0x01001000, 0x10001000, 0x11001000,
                0x00011000, 0x01011000, 0x10011000, 0x11011000,
                0x00101000, 0x01101000, 0x10101000, 0x11101000,
                0x00111000, 0x01111000, 0x10111000, 0x11111000,
                0x00001100, 0x01001100, 0x10001100, 0x11001100,
                0x00011100, 0x01011100, 0x10011100, 0x11011100,
                0x00101100, 0x01101100, 0x10101100, 0x11101100,
                0x00111100, 0x01111100, 0x10111100, 0x11111100,
                0x00000001, 0x01000001, 0x10000001, 0x11000001,
                0x00010001, 0x01010001, 0x10010001, 0x11010001,
                0x00100001, 0x01100001, 0x10100001, 0x11100001,
                0x00110001, 0x01110001, 0x10110001, 0x11110001,
                0x00000101, 0x01000101, 0x10000101, 0x11000101,
                0x00010101, 0x01010101, 0x10010101, 0x11010101,
                0x00100101, 0x01100101, 0x10100101, 0x11100101,
                0x00110101, 0x01110101, 0x10110101, 0x11110101,
                0x00001001, 0x01001001, 0x10001001, 0x11001001,
                0x00011001, 0x01011001, 0x10011001, 0x11011001,
                0x00101001, 0x01101001, 0x10101001, 0x11101001,
                0x00111001, 0x01111001, 0x10111001, 0x11111001,
                0x00001101, 0x01001101, 0x10001101, 0x11001101,
                0x00011101, 0x01011101, 0x10011101, 0x11011101,
                0x00101101, 0x01101101, 0x10101101, 0x11101101,
                0x00111101, 0x01111101, 0x10111101, 0x11111101,
                0x00000010, 0x01000010, 0x10000010, 0x11000010,
                0x00010010, 0x01010010, 0x10010010, 0x11010010,
                0x00100010, 0x01100010, 0x10100010, 0x11100010,
                0x00110010, 0x01110010, 0x10110010, 0x11110010,
                0x00000110, 0x01000110, 0x10000110, 0x11000110,
                0x00010110, 0x01010110, 0x10010110, 0x11010110,
                0x00100110, 0x01100110, 0x10100110, 0x11100110,
                0x00110110, 0x01110110, 0x10110110, 0x11110110,
                0x00001010, 0x01001010, 0x10001010, 0x11001010,
                0x00011010, 0x01011010, 0x10011010, 0x11011010,
                0x00101010, 0x01101010, 0x10101010, 0x11101010,
                0x00111010, 0x01111010, 0x10111010, 0x11111010,
                0x00001110, 0x01001110, 0x10001110, 0x11001110,
                0x00011110, 0x01011110, 0x10011110, 0x11011110,
                0x00101110, 0x01101110, 0x10101110, 0x11101110,
                0x00111110, 0x01111110, 0x10111110, 0x11111110,
                0x00000011, 0x01000011, 0x10000011, 0x11000011,
                0x00010011, 0x01010011, 0x10010011, 0x11010011,
                0x00100011, 0x01100011, 0x10100011, 0x11100011,
                0x00110011, 0x01110011, 0x10110011, 0x11110011,
                0x00000111, 0x01000111, 0x10000111, 0x11000111,
                0x00010111, 0x01010111, 0x10010111, 0x11010111,
                0x00100111, 0x01100111, 0x10100111, 0x11100111,
                0x00110111, 0x01110111, 0x10110111, 0x11110111,
                0x00001011, 0x01001011, 0x10001011, 0x11001011,
                0x00011011, 0x01011011, 0x10011011, 0x11011011,
                0x00101011, 0x01101011, 0x10101011, 0x11101011,
                0x00111011, 0x01111011, 0x10111011, 0x11111011,
                0x00001111, 0x01001111, 0x10001111, 0x11001111,
                0x00011111, 0x01011111, 0x10011111, 0x11011111,
                0x00101111, 0x01101111, 0x10101111, 0x11101111,
                0x00111111, 0x01111111, 0x10111111, 0x11111111
              };

 /* Temporary merged image data quadruplet */
 unsigned long tempDest;

 /* Register stuff to speed up processing (we'll need it!) */
 register unsigned char * rlpSrc = (unsigned char *)lpSrc;
 register unsigned long * rlpDestL = (unsigned long *)lpDest;
 register regDestWidth = DestWidth;
 register regSrcWidth = SrcWidth;

 /* PCX supports 2, 3 or 4 planes (anything else is ignored) */

 switch (Planes)
  {
   /* We have a full 4-plane image (Red, Green, Blue & Intensity) */
   case 4:

     /* Loop on the destination width (we're doing 8 nibbles at a time) */
     for (;regDestWidth>0;regDestWidth-=4)
      {

       /* Read one byte each from the four planes & munge them together */
       tempDest = table[(unsigned int)*rlpSrc]
                 |(table[(unsigned int)*(rlpSrc+regSrcWidth)]<<1)
                 |(table[(unsigned int)*(rlpSrc+regSrcWidth+regSrcWidth)]<<2)
                 |(table[(unsigned int)*((rlpSrc++)+regSrcWidth+regSrcWidth+regSrcWidth)]<<3);

       /* Now copy 1-4 bytes to the destination buffer */
       if (regDestWidth >= 4)
        {
         *rlpDestL++ = tempDest;
        }
       else
        {
         lpDest = (LPSTR)rlpDestL;
         *lpDest++ = (char)(tempDest & 0x000000FF);
         if (regDestWidth >= 2)
           *lpDest++ = (char)((tempDest >> 8) & 0x000000FF);
         if (regDestWidth >= 3)
           *lpDest++ = (char)((tempDest >> 16) & 0x000000FF);
        }
      }
     break;

   /* We have a 3-plane image (Red, Green & Blue only) */
   case 3:

     /* Loop on the destination width (we're doing 8 nibbles at a time) */
     for (;regDestWidth>0;regDestWidth-=4)
      {

       /* Read one byte each from the three planes & munge them together */
       tempDest = table[(unsigned int)*rlpSrc]
                 |(table[(unsigned int)*(rlpSrc+regSrcWidth)]<<1)
                 |(table[(unsigned int)*((rlpSrc++)+regSrcWidth+regSrcWidth)]<<2);

       /* Set the per-nibble Intensity bit only if any color bits are set */
       /* We should really use a separate 3-plane translation table */
       if (tempDest & 0xF0000000)
         tempDest |= 0x80000000;
       if (tempDest & 0x0F000000)
         tempDest |= 0x08000000;
       if (tempDest & 0x00F00000)
         tempDest |= 0x00800000;
       if (tempDest & 0x000F0000)
         tempDest |= 0x00080000;
       if (tempDest & 0x0000F000)
         tempDest |= 0x00008000;
       if (tempDest & 0x00000F00)
         tempDest |= 0x00000800;
       if (tempDest & 0x000000F0)
         tempDest |= 0x00000080;
       if (tempDest & 0x0000000F)
         tempDest |= 0x00000008;

       /* Now copy 1-4 bytes to the destination buffer */
       if (regDestWidth >= 4)
        {
         *rlpDestL++ = tempDest;
        }
       else
        {
         lpDest = (LPSTR)rlpDestL;
         *lpDest++ = (char)(tempDest & 0x000000FF);
         if (regDestWidth >= 2)
           *lpDest++ = (char)((tempDest >> 8) & 0x000000FF);
         if (regDestWidth >= 3)
           *lpDest++ = (char)((tempDest >> 16) & 0x000000FF);
        }
      }
     break;

   /* We have a 2-plane image (Red & Green only) */
   case 2:

     /* Loop on the destination width (we're doing 8 nibbles at a time) */
     for (;regDestWidth>0;regDestWidth-=4)
      {

       /* Read one byte each from the two planes & munge them together */
       tempDest = table[(unsigned int)*rlpSrc]
                 |(table[(unsigned int)*((rlpSrc++)+regSrcWidth)]<<1);

       /* Now copy 1-4 bytes to the destination buffer */
       if (regDestWidth >= 4)
        {
         *rlpDestL++ = tempDest;
        }
       else
        {
         lpDest = (LPSTR)rlpDestL;
         *lpDest++ = (char)(tempDest & 0x000000FF);
         if (regDestWidth >= 2)
           *lpDest++ = (char)((tempDest >> 8) & 0x000000FF);
         if (regDestWidth >= 3)
           *lpDest++ = (char)((tempDest >> 16) & 0x000000FF);
        }
      }
     break;

   /* If it's not a 2-, 3- or 4-plane image, we don't wanna know */
   default:
     break;
  }
}
