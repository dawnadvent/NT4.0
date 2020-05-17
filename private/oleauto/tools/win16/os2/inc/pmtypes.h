/***************************************************************************\
*
* Module Name: PMTYPES.H
*
* OS/2 Presentation Manager Datatypes include file
*
* This is conditionally included from PMWIN.H
*
* Copyright (c) International Business Machines Corporation 1981, 1988, 1989
* Copyright (c) Microsoft Corporation 1981, 1988, 1989
*
*****************************************************************************/

#define DTYP_USER               ((SHORT)16384)

#define DTYP_CTL_ARRAY          ((SHORT)1)
#define DTYP_CTL_PARRAY         ((SHORT)(-1))
#define DTYP_CTL_OFFSET         ((SHORT)2)
#define DTYP_CTL_LENGTH         ((SHORT)3)


/**********************************************************************/
/* Ordinary datatypes                                                 */
/**********************************************************************/

#define DTYP_ACCEL              ((SHORT)28)
#define DTYP_ACCELTABLE         ((SHORT)29)
#define DTYP_ARCPARAMS          ((SHORT)38)
#define DTYP_AREABUNDLE         ((SHORT)139)
#define DTYP_ATOM               ((SHORT)90)
#define DTYP_BITMAPINFO         ((SHORT)60)
#define DTYP_BITMAPINFOHEADER   ((SHORT)61)
#define DTYP_BIT16              ((SHORT)20)
#define DTYP_BIT32              ((SHORT)21)
#define DTYP_BIT8               ((SHORT)19)
#define DTYP_BOOL               ((SHORT)18)
#define DTYP_BTNCDATA           ((SHORT)35)
#define DTYP_BYTE               ((SHORT)13)
#define DTYP_CATCHBUF           ((SHORT)141)
#define DTYP_CHAR               ((SHORT)15)
#define DTYP_CHARBUNDLE         ((SHORT)135)
#define DTYP_CLASSINFO          ((SHORT)95)
#define DTYP_COUNT2             ((SHORT)93)
#define DTYP_COUNT2B            ((SHORT)70)
#define DTYP_COUNT2CH           ((SHORT)82)
#define DTYP_COUNT4             ((SHORT)152)
#define DTYP_COUNT4B            ((SHORT)42)
#define DTYP_CPID               ((SHORT)57)
#define DTYP_CREATESTRUCT       ((SHORT)98)
#define DTYP_CURSORINFO         ((SHORT)34)
#define DTYP_DEVOPENSTRUC       ((SHORT)124)
#define DTYP_DLGTEMPLATE        ((SHORT)96)
#define DTYP_DLGTITEM           ((SHORT)97)
#define DTYP_ENTRYFDATA         ((SHORT)127)
#define DTYP_ERRORID            ((SHORT)45)
#define DTYP_FATTRS             ((SHORT)75)
#define DTYP_FFDESCS            ((SHORT)142)
#define DTYP_FIXED              ((SHORT)99)
#define DTYP_FONTMETRICS        ((SHORT)74)
#define DTYP_FRAMECDATA         ((SHORT)144)
#define DTYP_GRADIENTL          ((SHORT)48)
#define DTYP_HAB                ((SHORT)10)
#define DTYP_HACCEL             ((SHORT)30)
#define DTYP_HAPP               ((SHORT)146)
#define DTYP_HATOMTBL           ((SHORT)91)
#define DTYP_HBITMAP            ((SHORT)62)
#define DTYP_HCINFO             ((SHORT)46)
#define DTYP_HDC                ((SHORT)132)
#define DTYP_HENUM              ((SHORT)117)
#define DTYP_HHEAP              ((SHORT)109)
#define DTYP_HINI               ((SHORT)53)
#define DTYP_HLIB               ((SHORT)147)
#define DTYP_HMF                ((SHORT)85)
#define DTYP_HMQ                ((SHORT)86)
#define DTYP_HPOINTER           ((SHORT)106)
#define DTYP_HPROGRAM           ((SHORT)131)
#define DTYP_HPS                ((SHORT)12)
#define DTYP_HRGN               ((SHORT)116)
#define DTYP_HSEM               ((SHORT)140)
#define DTYP_HSPL               ((SHORT)32)
#define DTYP_HSWITCH            ((SHORT)66)
#define DTYP_HVPS               ((SHORT)58)
#define DTYP_HWND               ((SHORT)11)
#define DTYP_IDENTITY           ((SHORT)133)
#define DTYP_IDENTITY4          ((SHORT)169)
#define DTYP_IMAGEBUNDLE        ((SHORT)136)
#define DTYP_INDEX2             ((SHORT)81)
#define DTYP_IPT                ((SHORT)155)
#define DTYP_KERNINGPAIRS       ((SHORT)118)
#define DTYP_LENGTH2            ((SHORT)68)
#define DTYP_LENGTH4            ((SHORT)69)
#define DTYP_LINEBUNDLE         ((SHORT)137)
#define DTYP_LONG               ((SHORT)25)
#define DTYP_MARKERBUNDLE       ((SHORT)138)
#define DTYP_MATRIXLF           ((SHORT)113)
#define DTYP_MLECTLDATA         ((SHORT)161)
#define DTYP_MLEMARGSTRUCT      ((SHORT)157)
#define DTYP_MLEOVERFLOW        ((SHORT)158)
#define DTYP_OFFSET2B           ((SHORT)112)
#define DTYP_OWNERITEM          ((SHORT)154)
#define DTYP_PID                ((SHORT)92)
#define DTYP_PIX                ((SHORT)156)
#define DTYP_POINTERINFO        ((SHORT)105)
#define DTYP_POINTL             ((SHORT)77)
#define DTYP_PROGCATEGORY       ((SHORT)129)
#define DTYP_PROGRAMENTRY       ((SHORT)128)
#define DTYP_PROGTYPE           ((SHORT)130)
#define DTYP_PROPERTY2          ((SHORT)88)
#define DTYP_PROPERTY4          ((SHORT)89)
#define DTYP_QMSG               ((SHORT)87)
#define DTYP_RECTL              ((SHORT)121)
#define DTYP_RESID              ((SHORT)125)
#define DTYP_RGB                ((SHORT)111)
#define DTYP_RGNRECT            ((SHORT)115)
#define DTYP_SBCDATA            ((SHORT)159)
#define DTYP_SEGOFF             ((SHORT)126)
#define DTYP_SHORT              ((SHORT)23)
#define DTYP_SIZEF              ((SHORT)101)
#define DTYP_SIZEL              ((SHORT)102)
#define DTYP_STRL               ((SHORT)17)
#define DTYP_STR16              ((SHORT)40)
#define DTYP_STR32              ((SHORT)37)
#define DTYP_STR64              ((SHORT)47)
#define DTYP_STR8               ((SHORT)33)
#define DTYP_SWBLOCK            ((SHORT)63)
#define DTYP_SWCNTRL            ((SHORT)64)
#define DTYP_SWENTRY            ((SHORT)65)
#define DTYP_SWP                ((SHORT)31)
#define DTYP_TID                ((SHORT)104)
#define DTYP_TIME               ((SHORT)107)
#define DTYP_TRACKINFO          ((SHORT)73)
#define DTYP_UCHAR              ((SHORT)22)
#define DTYP_ULONG              ((SHORT)26)
#define DTYP_USERBUTTON         ((SHORT)36)
#define DTYP_USHORT             ((SHORT)24)
#define DTYP_WIDTH4             ((SHORT)108)
#define DTYP_WNDPARAMS          ((SHORT)83)
#define DTYP_WNDPROC            ((SHORT)84)
#define DTYP_WPOINT             ((SHORT)59)
#define DTYP_WRECT              ((SHORT)55)
#define DTYP_XYWINSIZE          ((SHORT)52)


/**********************************************************************/
/* Pointer datatypes                                                  */
/**********************************************************************/

#define DTYP_PACCEL             ((SHORT)(-28))
#define DTYP_PACCELTABLE        ((SHORT)(-29))
#define DTYP_PARCPARAMS         ((SHORT)(-38))
#define DTYP_PAREABUNDLE        ((SHORT)(-139))
#define DTYP_PATOM              ((SHORT)(-90))
#define DTYP_PBITMAPINFO        ((SHORT)(-60))
#define DTYP_PBITMAPINFOHEADER  ((SHORT)(-61))
#define DTYP_PBIT16             ((SHORT)(-20))
#define DTYP_PBIT32             ((SHORT)(-21))
#define DTYP_PBIT8              ((SHORT)(-19))
#define DTYP_PBOOL              ((SHORT)(-18))
#define DTYP_PBTNCDATA          ((SHORT)(-35))
#define DTYP_PBYTE              ((SHORT)(-13))
#define DTYP_PCATCHBUF          ((SHORT)(-141))
#define DTYP_PCHAR              ((SHORT)(-15))
#define DTYP_PCHARBUNDLE        ((SHORT)(-135))
#define DTYP_PCLASSINFO         ((SHORT)(-95))
#define DTYP_PCOUNT2            ((SHORT)(-93))
#define DTYP_PCOUNT2B           ((SHORT)(-70))
#define DTYP_PCOUNT2CH          ((SHORT)(-82))
#define DTYP_PCOUNT4            ((SHORT)(-152))
#define DTYP_PCOUNT4B           ((SHORT)(-42))
#define DTYP_PCPID              ((SHORT)(-57))
#define DTYP_PCREATESTRUCT      ((SHORT)(-98))
#define DTYP_PCURSORINFO        ((SHORT)(-34))
#define DTYP_PDEVOPENSTRUC      ((SHORT)(-124))
#define DTYP_PDLGTEMPLATE       ((SHORT)(-96))
#define DTYP_PDLGTITEM          ((SHORT)(-97))
#define DTYP_PENTRYFDATA        ((SHORT)(-127))
#define DTYP_PERRORID           ((SHORT)(-45))
#define DTYP_PFATTRS            ((SHORT)(-75))
#define DTYP_PFFDESCS           ((SHORT)(-142))
#define DTYP_PFIXED             ((SHORT)(-99))
#define DTYP_PFONTMETRICS       ((SHORT)(-74))
#define DTYP_PFRAMECDATA        ((SHORT)(-144))
#define DTYP_PGRADIENTL         ((SHORT)(-48))
#define DTYP_PHAB               ((SHORT)(-10))
#define DTYP_PHACCEL            ((SHORT)(-30))
#define DTYP_PHAPP              ((SHORT)(-146))
#define DTYP_PHATOMTBL          ((SHORT)(-91))
#define DTYP_PHBITMAP           ((SHORT)(-62))
#define DTYP_PHCINFO            ((SHORT)(-46))
#define DTYP_PHDC               ((SHORT)(-132))
#define DTYP_PHENUM             ((SHORT)(-117))
#define DTYP_PHHEAP             ((SHORT)(-109))
#define DTYP_PHINI              ((SHORT)(-53))
#define DTYP_PHLIB              ((SHORT)(-147))
#define DTYP_PHMF               ((SHORT)(-85))
#define DTYP_PHMQ               ((SHORT)(-86))
#define DTYP_PHPOINTER          ((SHORT)(-106))
#define DTYP_PHPROGRAM          ((SHORT)(-131))
#define DTYP_PHPS               ((SHORT)(-12))
#define DTYP_PHRGN              ((SHORT)(-116))
#define DTYP_PHSEM              ((SHORT)(-140))
#define DTYP_PHSPL              ((SHORT)(-32))
#define DTYP_PHSWITCH           ((SHORT)(-66))
#define DTYP_PHVPS              ((SHORT)(-58))
#define DTYP_PHWND              ((SHORT)(-11))
#define DTYP_PIDENTITY          ((SHORT)(-133))
#define DTYP_PIDENTITY4         ((SHORT)(-169))
#define DTYP_PIMAGEBUNDLE       ((SHORT)(-136))
#define DTYP_PINDEX2            ((SHORT)(-81))
#define DTYP_PIPT               ((SHORT)(-155))
#define DTYP_PKERNINGPAIRS      ((SHORT)(-118))
#define DTYP_PLENGTH2           ((SHORT)(-68))
#define DTYP_PLENGTH4           ((SHORT)(-69))
#define DTYP_PLINEBUNDLE        ((SHORT)(-137))
#define DTYP_PLONG              ((SHORT)(-25))
#define DTYP_PMARKERBUNDLE      ((SHORT)(-138))
#define DTYP_PMATRIXLF          ((SHORT)(-113))
#define DTYP_PMLECTLDATA        ((SHORT)(-161))
#define DTYP_PMLEMARGSTRUCT     ((SHORT)(-157))
#define DTYP_PMLEOVERFLOW       ((SHORT)(-158))
#define DTYP_POFFSET2B          ((SHORT)(-112))
#define DTYP_POWNERITEM         ((SHORT)(-154))
#define DTYP_PPID               ((SHORT)(-92))
#define DTYP_PPIX               ((SHORT)(-156))
#define DTYP_PPOINTERINFO       ((SHORT)(-105))
#define DTYP_PPOINTL            ((SHORT)(-77))
#define DTYP_PPROGCATEGORY      ((SHORT)(-129))
#define DTYP_PPROGRAMENTRY      ((SHORT)(-128))
#define DTYP_PPROGTYPE          ((SHORT)(-130))
#define DTYP_PPROPERTY2         ((SHORT)(-88))
#define DTYP_PPROPERTY4         ((SHORT)(-89))
#define DTYP_PQMSG              ((SHORT)(-87))
#define DTYP_PRECTL             ((SHORT)(-121))
#define DTYP_PRESID             ((SHORT)(-125))
#define DTYP_PRGB               ((SHORT)(-111))
#define DTYP_PRGNRECT           ((SHORT)(-115))
#define DTYP_PSBCDATA           ((SHORT)(-159))
#define DTYP_PSEGOFF            ((SHORT)(-126))
#define DTYP_PSHORT             ((SHORT)(-23))
#define DTYP_PSIZEF             ((SHORT)(-101))
#define DTYP_PSIZEL             ((SHORT)(-102))
#define DTYP_PSTRL              ((SHORT)(-17))
#define DTYP_PSTR16             ((SHORT)(-40))
#define DTYP_PSTR32             ((SHORT)(-37))
#define DTYP_PSTR64             ((SHORT)(-47))
#define DTYP_PSTR8              ((SHORT)(-33))
#define DTYP_PSWBLOCK           ((SHORT)(-63))
#define DTYP_PSWCNTRL           ((SHORT)(-64))
#define DTYP_PSWENTRY           ((SHORT)(-65))
#define DTYP_PSWP               ((SHORT)(-31))
#define DTYP_PTID               ((SHORT)(-104))
#define DTYP_PTIME              ((SHORT)(-107))
#define DTYP_PTRACKINFO         ((SHORT)(-73))
#define DTYP_PUCHAR             ((SHORT)(-22))
#define DTYP_PULONG             ((SHORT)(-26))
#define DTYP_PUSERBUTTON        ((SHORT)(-36))
#define DTYP_PUSHORT            ((SHORT)(-24))
#define DTYP_PWIDTH4            ((SHORT)(-108))
#define DTYP_PWNDPARAMS         ((SHORT)(-83))
#define DTYP_PWNDPROC           ((SHORT)(-84))
#define DTYP_PWPOINT            ((SHORT)(-59))
#define DTYP_PWRECT             ((SHORT)(-55))
#define DTYP_PXYWINSIZE         ((SHORT)(-52))

