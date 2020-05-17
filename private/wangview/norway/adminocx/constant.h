//  File Types

#define IMAGE_FILETYPE_UNKNOWN  0
#define IMAGE_FILETYPE_TIFF     1
#define IMAGE_FILETYPE_AWD      2
#define IMAGE_FILETYPE_BMP      3
#define IMAGE_FILETYPE_PCX      4
#define IMAGE_FILETYPE_DCX      5
#define IMAGE_FILETYPE_JPEG     6
//#ifdef WITH_XIF
#define IMAGE_FILETYPE_XIF      7
#define MAXFILETYPE             7
//#else 
//#define MAXFILETYPE             6
//#endif //WITH_XIF

// Page Types
#define IMAGE_PAGETYPE_UNKNOWN  0
#define IMAGE_PAGETYPE_BW       1
#define IMAGE_PAGETYPE_GRAY4    2
#define IMAGE_PAGETYPE_GRAY8    3
#define IMAGE_PAGETYPE_PAL4     4
#define IMAGE_PAGETYPE_PAL8     5
#define IMAGE_PAGETYPE_RGB24    6
#define IMAGE_PAGETYPE_BGR24    7
#define MAXPAGETYPE             7

// Compression Types
#define IMAGE_COMPTYPE_UNKNOWN      0
#define IMAGE_COMPTYPE_NONE         1
#define IMAGE_COMPTYPE_GROUP3_1D    2
#define IMAGE_COMPTYPE_GROUP3_HUFF  3
#define IMAGE_COMPTYPE_PACKED_BITS  4
#define IMAGE_COMPTYPE_GROUP4_2D    5
#define IMAGE_COMPTYPE_JPEG         6
#define IMAGE_COMPTYPE_RBA          7
#define IMAGE_COMPTYPE_GROUP3_2D_FAX 8
#define IMAGE_COMPTYPE_LZW          9
#define MAXCOMPTYPE                 9

// Compression Info 
#define IMAGE_COMPINFO_EOL          0x0001
#define IMAGE_COMPINFO_PACKED_LINES 0x0002
#define IMAGE_COMPINFO_PREFIXED_EOL 0x0004
#define IMAGE_COMPINFO_COMP_LTR     0x0008
#define IMAGE_COMPINFO_EXP_LTR      0x0010
#define IMAGE_COMPINFO_NEGATE       0x0020
#define IMAGE_COMPINFO_HICMP_HIQ    0x0040
#define IMAGE_COMPINFO_HICMP_MEDQ   0x0080
#define IMAGE_COMPINFO_HICMP_LOWQ   0x0100
#define IMAGE_COMPINFO_MEDCMP_HIQ   0x0200
#define IMAGE_COMPINFO_MEDCMP_MEDQ  0x0400
#define IMAGE_COMPINFO_MEDCMP_LOWQ  0x0800
#define IMAGE_COMPINFO_LOWCMP_HIQ   0x1000
#define IMAGE_COMPINFO_LOWCMP_MEDQ  0x2000
#define IMAGE_COMPINFO_LOWCMP_LOWQ  0x4000

#define NONJPEG_COMPRESSION_INFO    0x003f
#define JPEG_COMPRESSION_INFO       0x7fc0

// Print Range Options
#define IMAGE_RANGE_ALL     0
#define IMAGE_RANGE_PAGES   1
#define IMAGE_RANGE_CURRENT 2

// Dialog Box options
#define DIALOG_OPTION_OPEN      0
#define DIALOG_OPTION_SAVEAS    1

// Verify Options
#define VERIFY_EXISTS   0
#define VERIFY_READ     1
#define VERIFY_WRITE    2
#define VERIFY_RW       3
