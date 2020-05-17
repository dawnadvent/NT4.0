/* Copyright (c) 1991, Microsoft Corporation, all rights reserved

    ipaddr.c - TCP/IP Address custom control

    November 9, 1992    Greg Strange
    December 13, 1993   Ronald Meijer - Wildcard and readonly style bits
    April    18, 1994   Ronald Meijer - Added IP_SETREADONLY, IP_SETFIELD
*/

#include <windows.h>
#include <stdlib.h>
#ifdef IP_CUST_CTRL
#include <custcntl.h>
#endif
#include "ipaddr.h"             // Global IPAddress definitions
#include "ipadd.h"              // Internal IPAddress definitions

/* global static variables */
HANDLE           hLibInstance = NULL;
#ifdef IP_CUST_CTRL
    HANDLE           hLibData;
    LPFNSTRTOID      lpfnVerId;
    LPFNIDTOSTR      lpfnIdStr;
#endif

/*
    Strings loaded at initialization.
*/
TCHAR szNoMem[MAX_IPNOMEMSTRING];       // Out of memory string
TCHAR szCaption[MAX_IPCAPTION];         // Alert message box caption

#define IPADDRESS_CLASS            TEXT("IPAddress")

//
// The character that is displayed between address fields.
//
#define FILLER          TEXT('.')
#define SZFILLER        TEXT(".")
#define SPACE           TEXT(' ')
#define WILDCARD        TEXT('*')
#define SZWILDCARD      TEXT("  *")
#define BACK_SPACE      8

//
// Min, max values
//
#define NUM_FIELDS      4
#define CHARS_PER_FIELD 3
#define HEAD_ROOM       2       // space at top of control
#define LEAD_ROOM       3       // space at front of control
#define MIN_FIELD_VALUE 0       // default minimum allowable field value
#define MAX_FIELD_VALUE 255     // default maximum allowable field value

//
// Number of characters available in a string array
//
#define NUMCHARS(str)   ((sizeof(str) / sizeof(str[0])) - 1)


//
// All the information unique to one control is stuffed in one of these
// structures in global memory and the handle to the memory is stored in the
// Windows extra space.
//

typedef struct tagFIELD {
    HANDLE      hWnd;
    FARPROC     lpfnWndProc;
    BYTE        byLow;  // lowest allowed value for this field.
    BYTE        byHigh; // Highest allowed value for this field.
} FIELD;

typedef struct tagCONTROL {
    HWND        hwndParent;
    UINT        uiFieldWidth;
    UINT        uiFillerWidth;
    BOOL        fEnabled;
    BOOL        fPainted;
    BOOL        fAllowWildcards;
    BOOL        fReadOnly;
    BOOL        fInMessageBox;  // Set when a message box is displayed so that
                                // we don't send a EN_KILLFOCUS message when
                                // we receive the EN_KILLFOCUS message for the
                                // current field.
    BOOL        fModified ;     // Indicates whether field has changed
    FIELD       Children[NUM_FIELDS];
} CONTROL;


//
// The following macros extract and store the CONTROL structure for a control.
//
#define    IPADDRESS_EXTRA            sizeof(DWORD)

#define GET_CONTROL_HANDLE(hWnd)        ((HGLOBAL)(GetWindowLong((hWnd), GWL_USERDATA)))
#define SAVE_CONTROL_HANDLE(hWnd,x)     (SetWindowLong((hWnd), GWL_USERDATA, (LONG)x))


/* internal IPAddress function prototypes */
#ifdef IP_CUST_CTRL
BOOL FAR WINAPI IPAddressDlgFn( HWND, WORD, WORD, LONG );
void GetStyleBit(HWND, LPCTLSTYLE, int, DWORD);
#endif
LONG FAR WINAPI IPAddressWndFn( HWND, UINT, WPARAM, LPARAM );
LONG FAR WINAPI IPAddressFieldProc(HWND, UINT, WPARAM, LPARAM);
BOOL SwitchFields(CONTROL FAR *, int, int, WORD, WORD);
void EnterField(FIELD FAR *, WORD, WORD);
BOOL ExitField(CONTROL FAR *, int iField);
int GetFieldValue(FIELD FAR *);
int FAR CDECL IPAlertPrintf(HWND hwndParent, UINT ids, int iCurrent, int iLow, int iHigh);
BOOL IPLoadOem(HINSTANCE hInst, UINT idResource, TCHAR* lpszBuffer, int cbBuffer);



/*
    LibMain() - Called once before anything else.

    call
        hInstance = library instance handle
        wDataSegment = library data segment
        wHeapSize = default heap size
        lpszCmdLine = command line arguements

    When this file is compiled as a DLL, this function is called by Libentry()
    when the library is first loaded.  See the SDK docs for details.
*/
#ifdef IPDLL

DLL_BASED BOOL WINAPI IpAddrDllEntry (
   HINSTANCE hDll,
   DWORD dwReason,
   LPVOID lpReserved
   )
{
    BOOL bResult = TRUE ;

    switch ( dwReason )
    {
        case DLL_PROCESS_ATTACH:
            bResult = IPAddrInit( hDll ) ;
            break ;
        case DLL_THREAD_ATTACH:
            break ;
        case DLL_PROCESS_DETACH:
            break ;
        case DLL_THREAD_DETACH:
            break ;
    }

    return bResult ;
}

#endif

LOGFONT logfont;

void SetDefaultFont( )
{
    logfont.lfWidth            = 0;
    logfont.lfEscapement       = 0;
    logfont.lfOrientation      = 0;
    logfont.lfOutPrecision     = OUT_DEFAULT_PRECIS;
    logfont.lfClipPrecision    = CLIP_DEFAULT_PRECIS;
    logfont.lfQuality          = DEFAULT_QUALITY;
    logfont.lfPitchAndFamily   = VARIABLE_PITCH | FF_SWISS;
    logfont.lfUnderline        = 0;
    logfont.lfStrikeOut        = 0;
    logfont.lfItalic           = 0;
    logfont.lfWeight           = FW_NORMAL;
#ifdef JAPAN
    //
    // BUGBUG This should not be necessary if "MS Shell Dlg" is mapped
    // to the system font.
    //
    logfont.lfHeight           = -(10*GetDeviceCaps(GetDC(NULL),LOGPIXELSY)/72);
    logfont.lfCharSet          = SHIFTJIS_CHARSET;
    lstrcpy(logfont.lfFaceName, TEXT("System"));
#else /* not JAPAN */
    logfont.lfHeight           = -(8*GetDeviceCaps(GetDC(NULL),LOGPIXELSY)/72);
    logfont.lfCharSet          = ANSI_CHARSET;
    lstrcpy(logfont.lfFaceName, TEXT("MS Shell Dlg"));
#endif /* JAPAN */
}


/*
    IPAddrInit() - IPAddress custom control initialization
    call
        hInstance = library or application instance
    return
        TRUE on success, FALSE on failure.

    This function does all the one time initialization of IPAddress custom
    controls.  Specifically it creates the IPAddress window class.
*/

DLL_BASED int FAR WINAPI IPAddrInit(HANDLE hInstance)
{
    HGLOBAL            hClassStruct;
    LPWNDCLASS        lpClassStruct;

    /* register IPAddress window if necessary */
    if ( hLibInstance == NULL ) {

        /* allocate memory for class structure */
        hClassStruct = GlobalAlloc( GHND, (DWORD)sizeof(WNDCLASS) );
        if ( hClassStruct ) {

            /* lock it down */
            lpClassStruct = (LPWNDCLASS)GlobalLock( hClassStruct );
            if ( lpClassStruct ) {

                /* define class attributes */
                lpClassStruct->lpszClassName = IPADDRESS_CLASS;
                lpClassStruct->hCursor =       LoadCursor(NULL,IDC_IBEAM);
                lpClassStruct->lpszMenuName =  (LPCTSTR)NULL;
                lpClassStruct->style =         CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS|CS_GLOBALCLASS;
                lpClassStruct->lpfnWndProc =   IPAddressWndFn;
                lpClassStruct->hInstance =     hInstance;
                lpClassStruct->hIcon =         NULL;
                lpClassStruct->cbWndExtra =    IPADDRESS_EXTRA;
                lpClassStruct->hbrBackground = (HBRUSH)(COLOR_WINDOW + 1 );

                /* register IPAddress window class */
                hLibInstance = ( RegisterClass(lpClassStruct) ) ? hInstance : NULL;

                if (hLibInstance)
                {
                    /* Load caption and out of memory string before we're
                       out of memory. */
                    if (!IPLoadOem(hLibInstance, IDS_IPNOMEM, szNoMem,NUMCHARS(szNoMem))
                        || !IPLoadOem(hLibInstance, IDS_IPMBCAPTION, szCaption,NUMCHARS(szCaption)))
                    {
                        return FALSE;
                    }
                }
                GlobalUnlock( hClassStruct );
            }
            GlobalFree( hClassStruct );
        }
    }
    SetDefaultFont();

    return hLibInstance != NULL ;
}

/*
    IPAddressInfo() - Returns various bits of information about the control.

    returns
        A handle for a CtlInfo structure.

    This function is only included in the DLL and is used by the dialog
    editor.
*/

#ifdef IP_CUST_CTRL
HANDLE FAR WINAPI IPAddressInfo()
{
    HGLOBAL        hCtlInfo;
    LPCTLINFO    lpCtlInfo;

    /* allocate space for information structure */
    hCtlInfo = GlobalAlloc( GHND, (DWORD)sizeof(CTLINFO) );
    if ( hCtlInfo ) {

        /* attempt to lock it down */
        lpCtlInfo = (LPCTLINFO)GlobalLock( hCtlInfo );
        if ( lpCtlInfo ) 
        {
            /* define the fixed portion of the structure */
            lpCtlInfo->wVersion = 100;
            lpCtlInfo->wCtlTypes = 1;
            lstrcpy( lpCtlInfo->szClass, IPADDRESS_CLASS );
            lstrcpy( lpCtlInfo->szTitle, TEXT("TCP/IP IP Address") );

            /* define the variable portion of the structure */
            lpCtlInfo->Type[0].wWidth = NUM_FIELDS*(CHARS_PER_FIELD+1) * 4 + 4;
            lpCtlInfo->Type[0].wHeight = 13;
            lpCtlInfo->Type[0].dwStyle = WS_CHILD | WS_TABSTOP;
            lstrcpy( lpCtlInfo->Type[0].szDescr, TEXT("IPAddress") );

            /* unlock it */
            GlobalUnlock( hCtlInfo );
        } 
        else 
        {
            GlobalFree( hCtlInfo );
            hCtlInfo = NULL;
        }

    }

    /* return result */
    return hCtlInfo;
}
#endif


/*
    IPAddressStyle()
    call
        hWnd            handle of parent window
        hCtlStyle       handle to control style info
        lpfnVerifyId    pointer to the VerifyId function from dialog editor
        lpfnGetIDStr    pointer to the GetIdStr function from dialog editor

    This function is called by the dialog editor when the user double clicks
    on the custom control.  Or when the user chooses to edit the control's
    styles.
*/

#ifdef IP_CUST_CTRL
BOOL FAR WINAPI IPAddressStyle(
    HWND        hWnd,
    HANDLE      hCtlStyle,
    LPFNSTRTOID lpfnVerifyId,
    LPFNIDTOSTR lpfnGetIdStr 
    )
{
    FARPROC       lpDlgFn;
    HANDLE        hNewCtlStyle;

    //
    // initialization
    //
    hLibData = hCtlStyle;
    lpfnVerId = lpfnVerifyId;
    lpfnIdStr = lpfnGetIdStr;

    //
    // display dialog box
    //
    lpDlgFn = MakeProcInstance( (FARPROC)IPAddressDlgFn, hLibInstance );
    hNewCtlStyle = ( DialogBox(hLibInstance,TEXT("IPAddressStyle"),hWnd,lpDlgFn) ) ? hLibData : NULL;
    FreeProcInstance( lpDlgFn );

    //
    // return updated data block
    //
    return hNewCtlStyle;
}
#endif




/*
    IPAddressDlgFn() - Dialog editor style dialog

    hDlg        styles dialog box handle
    wMessage    window message
    wParam      word parameter
    lParam      long parameter

    This is the dialog function for the styles dialog that is displayed when
    the user wants to edit an IPAddress control's style from the dialog editor.
*/
#ifdef IP_CUST_CTRL
BOOL FAR WINAPI IPAddressDlgFn(
    HWND        hDlg,
    WORD        wMessage,
    WORD        wParam,
    LONG        lParam )
{
    BOOL            bResult;

    /* initialization */
    bResult = TRUE;

    /* process message */
    switch( wMessage )
    {
        case WM_INITDIALOG :
        {
            HANDLE        hCtlStyle;
            LPCTLSTYLE    lpCtlStyle;

            /* disable Ok button & save dialog data handle */
            hCtlStyle = hLibData;

            /* retrieve & display style parameters */
            if ( hCtlStyle ) {

                /* add handle to property list */
                SetProp( hDlg, MAKEINTRESOURCE(1), hCtlStyle );

                /* update dialog box fields */
                lpCtlStyle = (LPCTLSTYLE)GlobalLock( hCtlStyle );

                lstrcpy( lpCtlStyle->szClass, IPADDRESS_CLASS );
                SendDlgItemMessage(hDlg, ID_VISIBLE, BM_SETCHECK,
                        (WPARAM)((lpCtlStyle->dwStyle & WS_VISIBLE) != 0), 0L);
                SendDlgItemMessage(hDlg, ID_GROUP, BM_SETCHECK,
                        (WPARAM)((lpCtlStyle->dwStyle & WS_GROUP) != 0), 0L);
                SendDlgItemMessage(hDlg, ID_DISABLED, BM_SETCHECK,
                        (WPARAM)((lpCtlStyle->dwStyle & WS_DISABLED) != 0), 0L);
                SendDlgItemMessage(hDlg, ID_TABSTOP, BM_SETCHECK,
                        (WPARAM)((lpCtlStyle->dwStyle & WS_TABSTOP) != 0), 0L);
                GlobalUnlock( hCtlStyle );

            } else
                EndDialog( hDlg, FALSE );
        }
        break;

        case WM_COMMAND :

            switch( wParam )
            {
                case IDCANCEL:
                    RemoveProp( hDlg, MAKEINTRESOURCE(1) );
                    EndDialog( hDlg, FALSE );
                    break;

                case IDOK:
                {
                    HANDLE        hCtlStyle;
                    LPCTLSTYLE    lpCtlStyle;

                    hCtlStyle = GetProp( hDlg, MAKEINTRESOURCE(1) );
                    lpCtlStyle = (LPCTLSTYLE)GlobalLock( hCtlStyle );

                    GetStyleBit(hDlg, lpCtlStyle, ID_VISIBLE,  WS_VISIBLE);
                    GetStyleBit(hDlg, lpCtlStyle, ID_DISABLED, WS_DISABLED);
                    GetStyleBit(hDlg, lpCtlStyle, ID_GROUP,    WS_GROUP);
                    GetStyleBit(hDlg, lpCtlStyle, ID_TABSTOP,  WS_TABSTOP);

                    GlobalUnlock( hCtlStyle );

                    RemoveProp( hDlg, MAKEINTRESOURCE(1) );

                    hLibData = hCtlStyle;
                    EndDialog( hDlg, TRUE );
                }
                break;

                default :
                    bResult = FALSE;
                    break;
            }
            break;

        default :
            bResult = FALSE;
            break;
    }
    return( bResult );
}
#endif




/*
    Get the value of a check box and set the appropriate style bit.
*/
#ifdef IP_CUST_CTRL
void GetStyleBit(HWND hDlg, LPCTLSTYLE lpCtlStyle, int iControl, DWORD dwStyle)
{
    if (SendDlgItemMessage(hDlg, iControl, BM_GETSTATE, 0, 0L))
        lpCtlStyle->dwStyle |= dwStyle;
    else
        lpCtlStyle->dwStyle &= ~dwStyle;
}
#endif


/*
    IPAddressFlags()

    call
        wFlags          class style flags
        lpszString      class style string
        wMaxString      maximum size of class style string

  This function translates the class style flags provided into a
  corresponding text string for output to an RC file.  The general
  windows flags (contained in the low byte) are not interpreted,
  only those in the high byte.

  The value returned by this function is the library instance
  handle when sucessful, and NULL otherwise.
*/
#ifdef IP_CUST_CTRL
WORD FAR WINAPI IPAddressFlags(
    WORD        wFlags,
    LPSTR       lpszString,
    WORD        wMaxString )
{
    lpszString[0] = NULL;
    return( 0 );
}
#endif

/*
    IPAddressWndFn() - Main window function for an IPAddress control.

    call
        hWnd    handle to IPAddress window
        wMsg    message number
        wParam  word parameter
        lParam  long parameter
*/
LONG FAR WINAPI IPAddressWndFn( hWnd, wMsg, wParam, lParam )
    HWND            hWnd;
    UINT            wMsg;
    WPARAM          wParam;
    LPARAM          lParam;
{
    LONG lResult;
    HGLOBAL hControl;
    CONTROL *pControl;
    int i;

    lResult = TRUE;

    switch( wMsg )
    {

        case WM_SETTEXT:
            {
                static TCHAR szBuf[CHARS_PER_FIELD+1];
                DWORD dwValue[4];
                int nField, nPos;
                BOOL fFinish = FALSE;
                TCHAR *pszString = (TCHAR*)lParam;
                dwValue[0]=0;
                dwValue[1]=0;
                dwValue[2]=0;
                dwValue[3]=0;

                for( nField = 0, nPos = 0; !fFinish; nPos++)
                {
                    if (( pszString[nPos]<TEXT('0')) || (pszString[nPos]>TEXT('9')))
                    {
                        // not a number
                        nField++;
                        fFinish = (nField == 4);
                    }
                    else
                    {
                        dwValue[nField] *= 10;
                        dwValue[nField] += (pszString[nPos]-TEXT('0'));
                    }
                }

                hControl = GET_CONTROL_HANDLE(hWnd);
                pControl = (CONTROL *)GlobalLock(hControl);
                for (i = 0; i < NUM_FIELDS; ++i)
                {
                    if ( lstrcmp(pszString, TEXT("")) == 0 )
                    {
                        wsprintf(szBuf,TEXT(""));
                    }
                    else
                    {
                        wsprintf(szBuf, TEXT("%d"), dwValue[i]);
                    }
                    SendMessage(pControl->Children[i].hWnd, WM_SETTEXT,
                                0, (LPARAM) (LPSTR) szBuf);
                }
                GlobalUnlock(hControl);
            }
            break;
        case WM_GETTEXTLENGTH:
        case WM_GETTEXT:
            {
                int iFieldValue;
                int srcPos, desPos;
                DWORD dwValue[4];
                TCHAR pszResult[30];
                TCHAR *pszDest = (TCHAR *)lParam;

                hControl = GET_CONTROL_HANDLE(hWnd);
                pControl = (CONTROL *)GlobalLock(hControl);

                lResult = 0;
                dwValue[0] = 0;
                dwValue[1] = 0;
                dwValue[2] = 0;
                dwValue[3] = 0;
                for (i = 0; i < NUM_FIELDS; ++i)
                {
                    iFieldValue = GetFieldValue(&(pControl->Children[i]));
                    if (iFieldValue == -1)
                        iFieldValue = 0;
                    else
                        ++lResult;
                    dwValue[i] = iFieldValue;
                }
                wsprintf( pszResult, TEXT("%d.%d.%d.%d"), dwValue[0], dwValue[1], dwValue[2], dwValue[3] );
                if ( wMsg == WM_GETTEXTLENGTH )
                {
                    lResult = lstrlen( pszResult );
                }
                else
                {
                    for ( srcPos=0, desPos=0; (srcPos+1<(INT)wParam) && (pszResult[srcPos]!=TEXT('\0')); )
                    {
                        pszDest[desPos++] = pszResult[srcPos++];
                    }
                    pszDest[desPos]=TEXT('\0');
                    lResult = desPos;
                }
                GlobalUnlock(hControl);
            }
            break;

        case WM_GETDLGCODE :
            lResult = DLGC_WANTCHARS;
            break;

        case WM_CREATE : /* create pallette window */
            {
                HDC hdc;
                UINT uiFieldStart;
                FARPROC lpfnFieldProc;

                hControl = GlobalAlloc(GMEM_MOVEABLE, sizeof(CONTROL));
                if (hControl)
                {
                    HFONT OldFont;

                    #define LPCS    ((CREATESTRUCT *)lParam)

                    pControl = (CONTROL *)GlobalLock(hControl);
                    pControl->fEnabled = TRUE;
                    pControl->fPainted = FALSE;
                    pControl->fModified = FALSE ;
                    pControl->fInMessageBox = FALSE;
                    pControl->hwndParent = LPCS->hwndParent;
                    pControl->fAllowWildcards = (LPCS->style & IPS_ALLOWWILDCARDS);
                    pControl->fReadOnly = (LPCS->style & IPS_READONLY);

                    hdc = GetDC(hWnd);
                    OldFont = SelectObject( hdc, CreateFontIndirect(&logfont) );
                    GetCharWidth(hdc, FILLER, FILLER,
                                            (int *)(&pControl->uiFillerWidth));
                    DeleteObject(SelectObject(hdc, OldFont ));
                    ReleaseDC(hWnd, hdc);

#ifdef JAPAN
                    {
                       POINT pt;
                       pt.x = LPCS->cx;
                       pt.y = LPCS->cy;
                       DPtoLP( hdc, &pt, 1 );
                       pControl->uiFieldWidth = (pt.x
					       -LEAD_ROOM*2 
						   - pControl->uiFillerWidth * (NUM_FIELDS-1))
                           / NUM_FIELDS;
                    }
#else /* not JAPAN */
                    pControl->uiFieldWidth = (LPCS->cx
                                          - LEAD_ROOM
                                          - pControl->uiFillerWidth
                                              *(NUM_FIELDS-1))
                                                  / NUM_FIELDS;
#endif /* JAPAN */
                    uiFieldStart = LEAD_ROOM;

                    lpfnFieldProc = MakeProcInstance((FARPROC)IPAddressFieldProc,
                                                 LPCS->hInstance);

                    for (i = 0; i < NUM_FIELDS; ++i)
                    {
                        DWORD dwStyle = WS_CHILD | WS_VISIBLE | /*ES_MULTILINE |*/ ES_CENTER;
                        pControl->Children[i].byLow = MIN_FIELD_VALUE;
                        pControl->Children[i].byHigh = MAX_FIELD_VALUE;
            
                        pControl->Children[i].hWnd = CreateWindow(
                                        TEXT("Edit"),
                                        NULL,
                                        dwStyle,
                                        uiFieldStart,
                                        HEAD_ROOM,
                                        pControl->uiFieldWidth,
                                        LPCS->cy-(HEAD_ROOM*2),
                                        hWnd,
                                        (HMENU) i,
                                        LPCS->hInstance,
                                        (LPVOID)NULL);

                        SendMessage(pControl->Children[i].hWnd, EM_LIMITTEXT,
                                CHARS_PER_FIELD, 0L);

                        SendMessage(pControl->Children[i].hWnd, WM_SETFONT,
                                (WPARAM) CreateFontIndirect(&logfont), TRUE);

                        pControl->Children[i].lpfnWndProc =
                            (FARPROC) GetWindowLong(pControl->Children[i].hWnd,
                                                GWL_WNDPROC);

                        SetWindowLong(pControl->Children[i].hWnd,
                                  GWL_WNDPROC, (LONG)lpfnFieldProc);

                        uiFieldStart += pControl->uiFieldWidth
                                    + pControl->uiFillerWidth;
                    }

                    GlobalUnlock(hControl);
                    SAVE_CONTROL_HANDLE(hWnd, hControl);

                    #undef LPCS
                }
                else
                    DestroyWindow(hWnd);
            }
            lResult = 0;
            break;

        case WM_PAINT: /* paint control window */
            {
                PAINTSTRUCT Ps;
                RECT rect;
                UINT uiFieldStart;
                COLORREF TextColor;
                HFONT OldFont;
                HBRUSH hBrush;
                HBRUSH hOldBrush;

                BeginPaint(hWnd, (LPPAINTSTRUCT)&Ps);
                OldFont = SelectObject( Ps.hdc, CreateFontIndirect(&logfont));
                GetClientRect(hWnd, &rect);
                hControl = GET_CONTROL_HANDLE(hWnd);
                pControl = (CONTROL *)GlobalLock(hControl);
                hBrush = CreateSolidBrush( GetSysColor( COLOR_WINDOW ));
                hOldBrush = SelectObject( Ps.hdc, hBrush );

                Rectangle(Ps.hdc, 0, 0, rect.right, rect.bottom);
                DeleteObject( SelectObject( Ps.hdc, hOldBrush ));

                if (pControl->fEnabled)
                    TextColor = GetSysColor(COLOR_WINDOWTEXT);
                else
                    TextColor = GetSysColor(COLOR_GRAYTEXT);

                if (TextColor)
                    SetTextColor(Ps.hdc, TextColor);
                SetBkColor(Ps.hdc, GetSysColor(COLOR_WINDOW));

                uiFieldStart = pControl->uiFieldWidth + LEAD_ROOM;
                for (i = 0; i < NUM_FIELDS-1; ++i)
                {
                    TextOut(Ps.hdc, uiFieldStart, HEAD_ROOM, SZFILLER, 1);
                    uiFieldStart +=pControl->uiFieldWidth + pControl->uiFillerWidth;
                }

                pControl->fPainted = TRUE;

                GlobalUnlock(hControl);
                DeleteObject(SelectObject(Ps.hdc, OldFont));
                EndPaint(hWnd, &Ps);
            }
            break;

        case WM_SETFOCUS : /* get focus - display caret */
            hControl = GET_CONTROL_HANDLE(hWnd);
            pControl = (CONTROL *)GlobalLock(hControl);
            EnterField(&(pControl->Children[0]), 0, CHARS_PER_FIELD);
            GlobalUnlock(hControl);
            break;

        case WM_LBUTTONDOWN : /* left button depressed - fall through */
            SetFocus(hWnd);
            break;

        case WM_ENABLE:
            {
                hControl = GET_CONTROL_HANDLE(hWnd);
                pControl = (CONTROL *)GlobalLock(hControl);
                pControl->fEnabled = (BOOL)wParam;
                for (i = 0; i < NUM_FIELDS; ++i)
                {
                    EnableWindow(pControl->Children[i].hWnd, (BOOL)wParam);
                }
                if (pControl->fPainted)    InvalidateRect(hWnd, NULL, FALSE);
                GlobalUnlock(hControl);
            }
            break;

        case WM_DESTROY :
            hControl = GET_CONTROL_HANDLE(hWnd);
            pControl = (CONTROL *)GlobalLock(hControl);

            //
            // Restore all the child window procedures before we delete our memory block.
            //
            for (i = 0; i < NUM_FIELDS; ++i)
            {
                SetWindowLong(pControl->Children[i].hWnd, GWL_WNDPROC,
                          (LONG)pControl->Children[i].lpfnWndProc);
            }

            GlobalUnlock(hControl);
            GlobalFree(hControl);
            break;

        case WM_COMMAND:
            switch (HIWORD(wParam))
            {
                //
                // One of the fields lost the focus, see if it lost the focus to another field
                // of if we've lost the focus altogether.  If its lost altogether, we must send
                // an EN_KILLFOCUS notification on up the ladder.
                //
                case EN_KILLFOCUS:
                    {
                        HWND hFocus;

                        hControl = GET_CONTROL_HANDLE(hWnd);
                        pControl = (CONTROL *)GlobalLock(hControl);

                        if (!pControl->fInMessageBox)
                        {
                            hFocus = GetFocus();
                            for (i = 0; i < NUM_FIELDS; ++i)
                                if (pControl->Children[i].hWnd == hFocus)
                                    break;

                            if (i >= NUM_FIELDS)
                                SendMessage(pControl->hwndParent, WM_COMMAND,
                                    MAKEWPARAM(GetWindowLong(hWnd, GWL_ID),
                                    EN_KILLFOCUS), (LPARAM)hWnd);
                            }
                            GlobalUnlock(hControl);
                    }
                    break;
                case EN_CHANGE:
                    hControl = GET_CONTROL_HANDLE(hWnd);
                    pControl = (CONTROL *)GlobalLock(hControl);

                    SendMessage(pControl->hwndParent, WM_COMMAND,
                        MAKEWPARAM(GetWindowLong(hWnd, GWL_ID), EN_CHANGE), (LPARAM)hWnd);

                    GlobalUnlock(hControl);
                    break;
            }
            break;

// Get the value of the IP Address.  The address is placed in the DWORD pointed
// to by lParam and the number of non-blank fields is returned.
        case IP_GETADDRESS:
            {
                int iFieldValue;
                DWORD dwValue;

                hControl = GET_CONTROL_HANDLE(hWnd);
                pControl = (CONTROL *)GlobalLock(hControl);

                lResult = 0;
                dwValue = 0;
                for (i = 0; i < NUM_FIELDS; ++i)
                {
                    iFieldValue = GetFieldValue(&(pControl->Children[i]));
                    if (iFieldValue == -1)
                        iFieldValue = 0;
                    else
                        ++lResult;
                    dwValue = (dwValue << 8) + iFieldValue;
                }
                *((DWORD *)lParam) = dwValue;

                GlobalUnlock(hControl);
            }
            break;

        case IP_GETMASK:
            {
                TCHAR szBuf[CHARS_PER_FIELD+1];
                WORD wLength;

                lResult = 0;
                hControl = GET_CONTROL_HANDLE(hWnd);
                pControl = (CONTROL *)GlobalLock(hControl);
                for (i = 0; i < NUM_FIELDS; ++i)
                {
                    *(WORD *)szBuf = sizeof(szBuf) - 1;
                    wLength = (WORD)SendMessage(pControl->Children[i].hWnd,
                            EM_GETLINE,0,(DWORD)(LPSTR)szBuf);
                    szBuf[wLength] = TEXT('\0');
                    if (!lstrcmp(szBuf, SZWILDCARD))
                    {
                        lResult |= 1L<<i;
                    }
                }
                GlobalUnlock(hControl);
            }
            break;

        case IP_GETMODIFY:
            {
                hControl = GET_CONTROL_HANDLE(hWnd);
                if ( ! hControl )
                    break ;
                pControl = (CONTROL *)GlobalLock(hControl);

                lResult = pControl->fModified > 0 ;
                for (i = 0 ; i < NUM_FIELDS ; )
                {
                    lResult |= SendMessage( pControl->Children[i++].hWnd, EM_GETMODIFY, 0, 0 ) > 0 ;
                }
                GlobalUnlock(hControl);
            }
            break ;

        case IP_SETMODIFY:
            {
                hControl = GET_CONTROL_HANDLE(hWnd);
                if ( ! hControl )
                    break ;
                pControl = (CONTROL *)GlobalLock(hControl);
                pControl->fModified =  wParam > 0 ;
                for (i = 0 ; i < NUM_FIELDS ; )
                {
                    SendMessage( pControl->Children[i++].hWnd, EM_GETMODIFY, wParam, 0 ) ;
                }
                GlobalUnlock(hControl);
            }
            break ;

        //
        // Clear all fields to blanks.
        //
        case IP_CLEARADDRESS:
            {
                hControl = GET_CONTROL_HANDLE(hWnd);
                pControl = (CONTROL *)GlobalLock(hControl);
                for (i = 0; i < NUM_FIELDS; ++i)
                {
                    SendMessage(pControl->Children[i].hWnd, WM_SETTEXT,
                            0, (LPARAM) (LPSTR) TEXT(""));
                }
                GlobalUnlock(hControl);
            }
            break;

        //
        // Set the value of the IP Address.  The address is in the lParam with the
        // first address byte being the high byte, the second being the second byte,
        // and so on.  A lParam value of -1 removes the address.
        //
        case IP_SETADDRESS:
            {
                static TCHAR szBuf[CHARS_PER_FIELD+1];

                hControl = GET_CONTROL_HANDLE(hWnd);
                pControl = (CONTROL *)GlobalLock(hControl);
                for (i = 0; i < NUM_FIELDS; ++i)
                {
                    wsprintf(szBuf, TEXT("%d"), HIBYTE(HIWORD(lParam)));
                    SendMessage(pControl->Children[i].hWnd, WM_SETTEXT,
                                0, (LPARAM) (LPSTR) szBuf);
                    lParam <<= 8;
                }
                GlobalUnlock(hControl);
            }
            break;

        case IP_SETREADONLY:
            {
                hControl = GET_CONTROL_HANDLE(hWnd);
                pControl = (CONTROL *)GlobalLock(hControl);
                pControl->fReadOnly = (wParam != 0);
                GlobalUnlock(hControl);
            }
            break;

        //
        // Set a single field value.  The wparam (0-3) indicates the field,
        // the lparam (0-255) indicates the value
        //
        case IP_SETFIELD:
            {
                static TCHAR szBuf[CHARS_PER_FIELD+1] = TEXT("");

                hControl = GET_CONTROL_HANDLE(hWnd);
                pControl = (CONTROL *)GlobalLock(hControl);
                if (wParam >= 0 && wParam < NUM_FIELDS)
                {
                    if (lParam != -1)
                    {
                        wsprintf(szBuf, TEXT("%d"), HIBYTE(HIWORD(lParam)));
                    }
                    SendMessage(pControl->Children[wParam].hWnd, WM_SETTEXT,
                                0, (LPARAM) (LPSTR) szBuf);
                }
                GlobalUnlock(hControl);
            }
            break;

        case IP_SETMASK:
            {
                BYTE bMask = (BYTE)wParam;
                static TCHAR szBuf[CHARS_PER_FIELD+1];

                hControl = GET_CONTROL_HANDLE(hWnd);
                pControl = (CONTROL *)GlobalLock(hControl);
                for (i = 0; i < NUM_FIELDS; ++i)
                {
                    if (bMask & 1<<i)
                    {
                        SendMessage(pControl->Children[i].hWnd, WM_SETTEXT,
                                    0, (LPARAM)SZWILDCARD);
                    }
                    else
                    {
                        wsprintf(szBuf, TEXT("%d"), HIBYTE(HIWORD(lParam)));
                        SendMessage(pControl->Children[i].hWnd, WM_SETTEXT,
                                    0, (LPARAM) (LPSTR) szBuf);
                    }
                    lParam <<= 8;
                }
                GlobalUnlock(hControl);
            }
            break;

        case IP_SETRANGE:
            if (wParam < NUM_FIELDS)
            {
                hControl = GET_CONTROL_HANDLE(hWnd);
                pControl = (CONTROL *)GlobalLock(hControl);

                pControl->Children[wParam].byLow = LOBYTE(LOWORD(lParam));
                pControl->Children[wParam].byHigh = HIBYTE(LOWORD(lParam));

                GlobalUnlock(hControl);
            }
            break;

        //
        // Set the focus to this control.
        // wParam = the field number to set focus to, or -1 to set the focus to the
        // first non-blank field.
        //
        case IP_SETFOCUS:
            hControl = GET_CONTROL_HANDLE(hWnd);
            pControl = (CONTROL *)GlobalLock(hControl);

            if (wParam >= NUM_FIELDS)
            {
                for (wParam = 0; wParam < NUM_FIELDS; ++wParam)
                    if (GetFieldValue(&(pControl->Children[wParam])) == -1)
                        break;
                if (wParam >= NUM_FIELDS)    wParam = 0;
            }
            //
            // 0, -1 select the entire control
            //
            EnterField(&(pControl->Children[wParam]), 0, (WORD)-1);

            GlobalUnlock(hControl);
            break;

        default:
            lResult = DefWindowProc( hWnd, wMsg, wParam, lParam );
            break;
    }
    return lResult;
}

/*
    IPAddressFieldProc() - Edit field window procedure

    This function sub-classes each edit field.
*/
LONG FAR WINAPI IPAddressFieldProc(HWND hWnd,
                                   UINT wMsg,
                                   WPARAM wParam,
                                   LPARAM lParam)
{
    HANDLE hControl;
    CONTROL *pControl;
    FIELD *pField;
    HWND hControlWindow;
    WORD wChildID;
    LRESULT lresult;

    if (!(hControlWindow = GetParent(hWnd)))
        return 0;

    hControl = GET_CONTROL_HANDLE(hControlWindow);
    pControl = (CONTROL *)GlobalLock(hControl);
    wChildID = (WORD) GetWindowLong(hWnd, GWL_ID);
    pField = &(pControl->Children[wChildID]);
    if (pField->hWnd != hWnd)
    {
        return 0;
    }

    switch (wMsg)
    {
        case WM_DESTROY:
            DeleteObject( (HGDIOBJ) SendMessage( hWnd, WM_GETFONT, 0, 0 ));
            return 0;

        case WM_CHAR:
            if (pControl->fReadOnly)
            {
                MessageBeep((UINT)-1);
                GlobalUnlock( hControl );
                return 0;
            }

            //
            // Typing in the last digit in a field, skips to the next field.
            //
            if (wParam >= TEXT('0') && wParam <= TEXT('9'))
            {
                DWORD dwResult;

                pControl->fModified = TRUE ;
                dwResult = CallWindowProc((WNDPROC)pControl->Children[wChildID].lpfnWndProc,
                                      hWnd, wMsg, wParam, lParam);
                dwResult = SendMessage(hWnd, EM_GETSEL, 0, 0L);

                if (dwResult == MAKELPARAM(CHARS_PER_FIELD, CHARS_PER_FIELD)
                    && ExitField(pControl, wChildID)
                    && wChildID < NUM_FIELDS-1)
                {
                    EnterField(&(pControl->Children[wChildID+1]),
                                0, CHARS_PER_FIELD);
                }
                GlobalUnlock( hControl );
                return dwResult;
            }

            //
            // spaces and periods fills out the current field and then if possible,
            // goes to the next field.
            //
            else if (wParam == FILLER || wParam == SPACE )
            {
                DWORD dwResult;
                dwResult = SendMessage(hWnd, EM_GETSEL, 0, 0L);

                if (dwResult != 0L && HIWORD(dwResult) == LOWORD(dwResult)
                    && ExitField(pControl, wChildID))
                {
                    if (wChildID >= NUM_FIELDS-1)
                    {
                        MessageBeep((UINT)-1);
                    }
                    else
                    {
                        EnterField(&(pControl->Children[wChildID+1]),
                                    0, CHARS_PER_FIELD);
                    }
                }
                GlobalUnlock( hControl );
                return 0;
            }

            //
            // Backspaces go to the previous field if at the beginning of the current field.
            // Also, if the focus shifts to the previous field, the backspace must be
            // processed by that field.
            //
            else if (wParam == BACK_SPACE)
            {
                pControl->fModified = TRUE ;
                if (wChildID > 0 && SendMessage(hWnd, EM_GETSEL, 0, 0L) == 0L)
                {
                    if (SwitchFields(pControl, wChildID, wChildID-1,
                            CHARS_PER_FIELD, CHARS_PER_FIELD)
                        && SendMessage(pControl->Children[wChildID-1].hWnd,
                        EM_LINELENGTH, 0, 0L) != 0L)
                    {
                        SendMessage(pControl->Children[wChildID-1].hWnd,
                                wMsg, wParam, lParam);
                    }
                    GlobalUnlock( hControl );
                    return 0;
                }
                else
                {
                    TCHAR szBuf[CHARS_PER_FIELD+1];
                    WORD wLength;

                    *(WORD *)szBuf = sizeof(szBuf) - 1;
                    wLength = (WORD)SendMessage(pControl->Children[wChildID].hWnd,
                            EM_GETLINE,0,(DWORD)(LPSTR)szBuf);
                    szBuf[wLength] = TEXT('\0');
                    if (!lstrcmp(szBuf, SZWILDCARD))
                    {
                        SendMessage(pControl->Children[wChildID].hWnd,
                            WM_SETTEXT, 0, (LPARAM)TEXT(""));
                    }
                }
            }

            else if ((wParam == WILDCARD) && (pControl->fAllowWildcards))
            {
                //
                // Only works at the beginning of the line.
                //
                if (SendMessage(hWnd, EM_GETSEL, 0, 0L) == 0L)
                {
                    pControl->fModified = TRUE;
                    SendMessage(pControl->Children[wChildID].hWnd, WM_SETTEXT, 0, (LPARAM)SZWILDCARD);

                    if (ExitField(pControl, wChildID) && (wChildID < NUM_FIELDS-1))
                    {
                        EnterField(&(pControl->Children[wChildID+1]),0, CHARS_PER_FIELD);
                    }
                }
                else
                {
                    //
                    // Not at the beginning of the line, complain
                    //
                    MessageBeep((UINT)-1);
                }
                GlobalUnlock( hControl );
                return 0;
            }

            //
            // Any other printable characters are not allowed.
            //
            else if (wParam > SPACE)
            {
                MessageBeep((UINT)-1);
                GlobalUnlock( hControl );
                return 0;
            }
            break;

    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_DELETE:
            {
                TCHAR szBuf[CHARS_PER_FIELD+1];
                WORD wLength;

                if (pControl->fReadOnly)
                {
                    MessageBeep((UINT)-1);
                    GlobalUnlock( hControl );
                    return 0;
                }

                pControl->fModified = TRUE ;

                *(WORD *)szBuf = sizeof(szBuf) - 1;
                wLength = (WORD)SendMessage(pControl->Children[wChildID].hWnd,
                            EM_GETLINE,0,(DWORD)(LPSTR)szBuf);
                szBuf[wLength] = TEXT('\0');
                if (!lstrcmp(szBuf, SZWILDCARD))
                {
                    SendMessage(pControl->Children[wChildID].hWnd,
                            WM_SETTEXT, 0, (LPARAM)TEXT(""));
                    GlobalUnlock( hControl );
                    return 0;
                }
            }
            break;

        //
        // Arrow keys move between fields when the end of a field is reached.
        //
        case VK_LEFT:
        case VK_RIGHT:
        case VK_UP:
        case VK_DOWN:
            if (GetKeyState(VK_CONTROL) < 0)
            {
                if ((wParam == VK_LEFT || wParam == VK_UP) && wChildID > 0)
                {
                    SwitchFields(pControl, wChildID, wChildID-1,
                                  0, CHARS_PER_FIELD);
                    GlobalUnlock( hControl );
                    return 0;
                }
                else if ((wParam == VK_RIGHT || wParam == VK_DOWN)
                         && wChildID < NUM_FIELDS-1)
                {
                    SwitchFields(pControl, wChildID, wChildID+1,
                                      0, CHARS_PER_FIELD);
                    GlobalUnlock( hControl );
                    return 0;
                }
            }
            else
            {
                DWORD dwResult;
                WORD wStart, wEnd;

                dwResult = SendMessage(hWnd, EM_GETSEL, 0, 0L);
                wStart = LOWORD(dwResult);
                wEnd = HIWORD(dwResult);
                if (wStart == wEnd)
                {
                    if ((wParam == VK_LEFT || wParam == VK_UP)
                        && wStart == 0
                        && wChildID > 0)
                    {
                        SwitchFields(pControl, wChildID, wChildID-1,
                                          CHARS_PER_FIELD, CHARS_PER_FIELD);
                        GlobalUnlock( hControl );
                        return 0;
                    }
                    else if ((wParam == VK_RIGHT || wParam == VK_DOWN)
                             && wChildID < NUM_FIELDS-1)
                    {
                        dwResult = SendMessage(hWnd, EM_LINELENGTH, 0, 0L);
                        if (wStart >= dwResult)
                        {
                            SwitchFields(pControl, wChildID, wChildID+1, 0, 0);
                            GlobalUnlock( hControl );
                            return 0;
                        }
                    }
                }
            }
            break;

        //
        // Home jumps back to the beginning of the first field.
        //
        case VK_HOME:
            if (wChildID > 0)
            {
                SwitchFields(pControl, wChildID, 0, 0, 0);
                GlobalUnlock( hControl );
                return 0;
            }
            break;

        //
        // End scoots to the end of the last field.
        //
        case VK_END:
            if (wChildID < NUM_FIELDS-1)
            {
                SwitchFields(pControl, wChildID, NUM_FIELDS-1,
                                CHARS_PER_FIELD, CHARS_PER_FIELD);
                GlobalUnlock( hControl );
                return 0;
            }
            break;


        } // switch (wParam)

        break;

    case WM_KILLFOCUS:
        if ( !ExitField( pControl, wChildID ))
        {
            GlobalUnlock( hControl );
            return 0;
        }

    } // switch (wMsg)

    lresult = CallWindowProc( (WNDPROC)pControl->Children[wChildID].lpfnWndProc,
        hWnd, wMsg, wParam, lParam);
    GlobalUnlock( hControl );

    return lresult;
}




/*
    Switch the focus from one field to another.
    call
        pControl = Pointer to the CONTROL structure.
        iOld = Field we're leaving.
        iNew = Field we're entering.
        hNew = Window of field to goto
        wStart = First character selected
        wEnd = Last character selected + 1
    returns
        TRUE on success, FALSE on failure.

    Only switches fields if the current field can be validated.
*/
BOOL SwitchFields(CONTROL *pControl, int iOld, int iNew, WORD wStart, WORD wEnd)
{
    if (!ExitField(pControl, iOld))
        return FALSE;
    EnterField(&(pControl->Children[iNew]), wStart, wEnd);
}

/*
    Set the focus to a specific field's window.
    call
        pField = pointer to field structure for the field.
        wStart = First character selected
        wEnd = Last character selected + 1
*/
void EnterField(FIELD *pField, WORD wStart, WORD wEnd)
{
    SetFocus(pField->hWnd);
    SendMessage(pField->hWnd, EM_SETSEL, wStart, wEnd);
}


/*
    Exit a field.
    call
        pControl = pointer to CONTROL structure.
        iField = field number being exited.
    returns
        TRUE if the user may exit the field.
        FALSE if he may not.
*/
BOOL ExitField(CONTROL  *pControl, int iField)
{
    HWND hControlWnd;
    HWND hDialog;
    WORD wLength;
    FIELD *pField;
    static TCHAR szBuf[CHARS_PER_FIELD+1];
    int i,j;

    pField = &(pControl->Children[iField]);
    *(WORD *)szBuf = sizeof(szBuf) - 1;
    wLength = (WORD)SendMessage(pField->hWnd,EM_GETLINE,0,(DWORD)(LPSTR)szBuf);
    if (wLength != 0)
    {
        szBuf[wLength] = TEXT('\0');
        if (pControl->fAllowWildcards && !lstrcmp(szBuf, SZWILDCARD))
        {
            return TRUE;
        }
        for (j=0,i=0;j<(INT)wLength;j++)
        {
            i=i*10+szBuf[j]-TEXT('0');
        }
        if (i < (int)(UINT)pField->byLow || i > (int)(UINT)pField->byHigh)
        {
            if ( i < (int)(UINT) pField->byLow )
            {
                /* too small */
                wsprintf(szBuf, TEXT("%d"), (int)(UINT)pField->byLow );
            }
            else
            {
                /* must be bigger */
                wsprintf(szBuf, TEXT("%d"), (int)(UINT)pField->byHigh );
            }
            SendMessage(pField->hWnd, WM_SETTEXT, 0, (LPARAM) (LPSTR) szBuf);
            if ((hControlWnd = GetParent(pField->hWnd)) != NULL
                && (hDialog = GetParent(hControlWnd)) != NULL)
            {
                pControl->fInMessageBox = TRUE;
                IPAlertPrintf(hDialog, IDS_IPBAD_FIELD_VALUE, i,
                            pField->byLow, pField->byHigh);
                pControl->fInMessageBox = FALSE;
                SendMessage(pField->hWnd, EM_SETSEL, 0, CHARS_PER_FIELD);
                return FALSE;
            }
        }
    }
    return TRUE;
}


/*
    Get the value stored in a field.
    call
        pField = pointer to the FIELD structure for the field.
    returns
        The value (0..255) or -1 if the field has not value.
*/
int GetFieldValue(FIELD *pField)
{
    WORD wLength;
    static TCHAR szBuf[CHARS_PER_FIELD+1];
    INT i,j;

    //
    // Bug? According to docs, wparam should be number of _characters_.
    //      but in real this needs to be _bytes_ apparently.
    //
    //wLength = (WORD)SendMessage(pField->hWnd,WM_GETTEXT,NUMCHARS(szBuf),(DWORD)(LPSTR)szBuf);
    wLength = (WORD)SendMessage(pField->hWnd,WM_GETTEXT,sizeof(szBuf),(DWORD)(LPSTR)szBuf);
    if (wLength != 0)
    {
        szBuf[wLength] = TEXT('\0');
        if (!lstrcmp(szBuf, SZWILDCARD))
        {
            return 255;
        }
        for (j=0,i=0;j<(INT)wLength;j++)
        {
            i=i*10+szBuf[j]-TEXT('0');
        }
        return i;
    }
    else
        return -1;
}



/*
    IPAlertPrintf() - Does a printf to a message box.
*/
int FAR CDECL IPAlertPrintf(HWND hwndParent, UINT ids, int iCurrent, int iLow, int iHigh)
{
    static TCHAR szBuf[MAX_IPRES];
    static TCHAR szFormat[MAX_IPRES];
    TCHAR * psz;

    if (ids != IDS_IPNOMEM &&
        //
        // Why OEM?
        //
        //IPLoadOem(hLibInstance, ids, szFormat, NUMCHARS(szFormat)))
        LoadString(hLibInstance, ids, szFormat, NUMCHARS(szFormat)))
    {
        wsprintf(szBuf, szFormat, iCurrent, iLow, iHigh);
        psz = szBuf;
    }
    else
    {
        psz = szNoMem;
    }

    MessageBeep(MB_ICONEXCLAMATION);
    return MessageBox(hwndParent, psz, szCaption, MB_ICONEXCLAMATION);
}



/*
    Load an OEM string and convert it to ANSI.
    call
        hInst = This instance
        idResource = The ID of the string to load
        lpszBuffer = Pointer to buffer to load string into.
        cbBuffer = Length of the buffer.
    returns
        TRUE if the string is loaded, FALSE if it is not.
*/

BOOL 
IPLoadOem(
    HINSTANCE hInst, 
    UINT idResource, 
    TCHAR* lpszBuffer, 
    int cbBuffer
    )
{
    if (LoadString(hInst, idResource, lpszBuffer, cbBuffer))
    {
        //OemToAnsi(lpszBuffer, lpszBuffer);
        return TRUE;
    }
    else
    {
        lpszBuffer[0] = 0;
        return FALSE;
    }
}
