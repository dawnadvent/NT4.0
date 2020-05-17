/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 3.00.44 */
/* at Tue Jun 25 18:36:45 1996
 */
/* Compiler settings for hlink.idl:
    Oic (OptLev=i1), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: none
*/
//@@MIDL_FILE_HEADING(  )
#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

const IID IID_IHlink = {0x79eac9c3,0xbaf9,0x11ce,{0x8c,0x82,0x00,0xaa,0x00,0x4b,0xa9,0x0b}};


const IID IID_IHlinkSite = {0x79eac9c2,0xbaf9,0x11ce,{0x8c,0x82,0x00,0xaa,0x00,0x4b,0xa9,0x0b}};


const IID IID_IHlinkTarget = {0x79eac9c4,0xbaf9,0x11ce,{0x8c,0x82,0x00,0xaa,0x00,0x4b,0xa9,0x0b}};


const IID IID_IHlinkFrame = {0x79eac9c5,0xbaf9,0x11ce,{0x8c,0x82,0x00,0xaa,0x00,0x4b,0xa9,0x0b}};


const IID IID_IEnumHLITEM = {0x79eac9c6,0xbaf9,0x11ce,{0x8c,0x82,0x00,0xaa,0x00,0x4b,0xa9,0x0b}};


const IID IID_IHlinkBrowseContext = {0x79eac9c7,0xbaf9,0x11ce,{0x8c,0x82,0x00,0xaa,0x00,0x4b,0xa9,0x0b}};


#ifdef __cplusplus
}
#endif

