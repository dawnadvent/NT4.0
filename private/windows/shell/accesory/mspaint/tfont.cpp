/******************************************************************************/
/* Tfont.CPP: IMPLEMENTATION OF THE CTfont CLASS which encapsulates the       */
/* CTfondDlg and CTfontTbar classes                                           */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/* Methods in this file                                                       */
/*                                                                            */
/*  TextFont Object                                                           */
/*      CTfont::CTfont                                                        */
/*      CTfont::CTfont                                                        */
/*      CTfont::~CTfont                                                       */
/*      CTfont::EmptyMap                                                      */
/*      CTfont::FreeMemoryFromCBox                                            */
/*                                                                            */
/*  Miscellaneous Methods                                                     */
/*      CTfont::Create                                                        */
/*      CTfont::Undo                                                          */
/*      CTfont::RefreshToolBar                                                */
/*      CTfont::ResizeWindow                                                  */
/*      CTfont::DrawItem                                                      */
/*      CTfont::SetColorsInDC                                                 */
/*      CTfont::SaveToIniFile                                                 */
/*      CTfont::RestoreFontAttrsFromIniFile                                   */
/*      CTfont::ReadFromIniFile                                               */
/*                                                                            */
/*  Font/Size enumeration methods                                             */
/*      CTfont::RefreshFontList                                               */
/*      CTfont::EnumFontFace                                                  */
/*      CTfont::EnumFontFaceProc                                              */
/*      CTfont::EnumFontSizes                                                 */
/*      CTfont::EnumFontOneFaceProc                                           */
/*                                                                            */
/*  Combo Box Notification and processing methods                             */
/*      CTfont::ProcessNewTypeface                                            */
/*      CTfont::UpdateEditControlFont                                         */
/*      CTfont::OnTypeFaceComboBoxUpdate                                      */
/*      CTfont::OnPointSizeComboBoxUpdate                                     */
/*      CTfont::OnSelendokTypeface                                            */
/*      CTfont::OnCloseupTypeface                                             */
/*      CTfont::OnSelendokPointSize                                           */
/*      CTfont::OnCloseupPointSize                                            */
/*      CTfont::OnKillfocusPointsize                                          */
/*      CTfont::OnRButtonDown                                                 */
/*                                                                            */
/*                                                                            */
/*  Control Notification/Window Messages                                      */
/*      CTfont::OnMove                                                        */
/*      CTfont::OnClose                                                       */
/*      CTfont::OnSetFocus                                                    */
/*      CTfont::OnDestroy                                                     */
/*      CTfont::OnBold                                                        */
/*      CTfont::OnItalic                                                      */
/*      CTfont::OnUnderline                                                   */
/*      CTfont::OnShadow                                                      */
/*      CTfont::OnPen                                                         */
/*      CTfont::OnEditText                                                    */
/*      CTfont::OnKeyboard                                                    */
/*      CTfont::OnInsSpace                                                    */
/*      CTfont::OnBackSpace                                                   */
/*      CTfont::OnNewLine                                                     */
/*      CTfont::OnDrawItem                                                    */
/*      CTfont::OnMeasureItem                                                 */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*  Tool Bar Object                                                           */
/*      CTfontTbar::CTfontTbar                                                */
/*      CTfontTbar::Create                                                    */
/*      CTfontTbar::~CTfontTbar                                               */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*  Dialog Bar Object                                                         */
/*      TfontDlg::CTfontDlg(void)                                             */
/*      TfontDlg::Create(CWnd* pcParentWnd)                                   */
/*      TfontDlg::~CTfontDlg(void)                                            */
/*      TfontDlg::OnRButtonDown                                               */
/*                                                                            */
/*                                                                            */
/******************************************************************************/

#include "stdafx.h"
#include <memory.h>
#include <tchar.h>
#include "global.h"
#include "pbrush.h"
#include "pbrusfrm.h"
#include "pbrusvw.h"
#include "pictures.h"
#include "resource.h"
#include "minifwnd.h"
#include "Tfont.h"
#include "Tedit.h"
#ifndef NT
#include <penwin.h> // soe we can bring up the lens from the toolbar
#endif
#ifdef _DEBUG
#undef THIS_FILE
static CHAR BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE( CTfont, CMiniFrmWnd )
IMPLEMENT_DYNCREATE( CTfontTbar, CToolBar )
IMPLEMENT_DYNCREATE( CTfontDlg, CDialogBar )

#include "memtrace.h"

class CFontDesc
{
public:
        CFontDesc(LPCTSTR lpszName, BYTE nCharSet, int iFontType);
        CString m_strName;
        BYTE m_nCharSet;
        int m_iFontType;
};

CFontDesc::CFontDesc(LPCTSTR lpszName, BYTE nCharSet, int iFontType)
{
        m_strName = lpszName;
        m_nCharSet = nCharSet;
        m_iFontType = iFontType;
}

class CFontComboBox : public CComboBox
{
public:

      
        int AddFontName(LPCTSTR lpszName, LPCTSTR lpszScript, BYTE nCharSet,
                int iFontType);
        CFontDesc* GetFontDesc(int nIndex) {return (CFontDesc*)GetItemData(nIndex);};
        BOOL IsSameName(CFontDesc* pDesc, int index);

        // I'm not really doing the message map stuff
        void ClearList();
} ;

/******************************************************************************/
// CTfont

BEGIN_MESSAGE_MAP( CTfont, CMiniFrmWnd )
    //{{AFX_MSG_MAP(CTfont)
    ON_CBN_SELENDOK(IDC_TYPEFACE, OnSelendokTypeface)
    ON_CBN_CLOSEUP(IDC_TYPEFACE, OnCloseupTypeface)
    ON_CBN_SELENDOK(IDC_POINTSIZE, OnSelendokPointSize)
    ON_CBN_CLOSEUP(IDC_POINTSIZE, OnCloseupPointSize)
    ON_WM_ERASEBKGND()
    ON_WM_SETFOCUS()
    ON_WM_DESTROY()
    ON_WM_MOVE()
    ON_WM_CLOSE()
    ON_CBN_KILLFOCUS(IDC_POINTSIZE, OnKillfocusPointsize)
    ON_WM_RBUTTONDOWN()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_COMMAND(IDC_BOLD, OnBold)
    ON_COMMAND(IDC_ITALIC, OnItalic)
    ON_COMMAND(IDC_UNDERLINE, OnUnderline)
#ifdef DBCS //VertEdit
    ON_COMMAND(IDC_VERTEDIT, OnVertEdit)
    ON_UPDATE_COMMAND_UI(IDC_VERTEDIT, OnVertEditUpdate)
#endif //DBCS
    ON_COMMAND(IDC_SHADOW, OnShadow)
    ON_COMMAND(IDC_PENEXT, OnPen)
    ON_COMMAND(IDC_EDITTEXT, OnEditText)
    ON_COMMAND(IDC_KEYBOARD, OnKeyboard)
    ON_COMMAND(IDC_INS_SPACE, OnInsSpace)
    ON_COMMAND(IDC_BACKSPACE, OnBackSpace)
    ON_COMMAND(IDC_NEWLINE, OnNewLine)
        //}}AFX_MSG_MAP

    ON_MESSAGE(UM_DELAYED_TOOLBAR, OnDelayedPen)

END_MESSAGE_MAP()


/******************************************************************************/
// CTfont construction/destruction

CTfont::CTfont( CTedit *pcTedit )
    {
    ASSERT( pcTedit != NULL );

    m_pcTedit = pcTedit;

    m_cStrTypeFaceName.Empty();
    m_cStrTypeFaceNamePrev.Empty();

    m_iWeight              = FW_NORMAL;
    m_bBoldOn              = FALSE;
    m_bItalicOn            = FALSE;
    m_bUnderlineOn         = FALSE;
#ifdef DBCS //VertEdit
    m_bVertEditOn          = FALSE;
#endif //DBCS
    m_bShadowOn            = FALSE;
    m_bPenOn               = FALSE;
    m_bInUpdate            = FALSE;
    m_iControlIDLastChange = 0;
    m_iPointSize           = 0;
    m_iPointSizePrev       = 0;
    m_iFontType            = 0;


    m_cRectWindow.SetRectEmpty();


    m_pcTfontTbar = new CTfontTbar();
    }

/******************************************************************************/

CTfont::CTfont()
    {
    m_cStrTypeFaceName.Empty();
    m_cStrTypeFaceNamePrev.Empty();

    m_iWeight              = FW_NORMAL;
    m_bBoldOn              = FALSE;
    m_bItalicOn            = FALSE;
    m_bUnderlineOn         = FALSE;
#ifdef DBCS //VertEdit
    m_bVertEditOn          = FALSE;
#endif //DBCS
    m_bShadowOn            = FALSE;
    m_bPenOn               = FALSE;
    m_bInUpdate            = FALSE;
    m_iControlIDLastChange = 0;
    m_iPointSize           = 0;
    m_iPointSizePrev       = 0;
    m_iFontType            = 0;

    m_cRectWindow.SetRectEmpty();

    m_pcTfontTbar = new CTfontTbar();
    }

/******************************************************************************/
// CTfont construction/destruction

CTfont::~CTfont(void)
    {
    SaveToIniFile();

//  *DK* this deletion of the current font assumes this object, the font picker,
//  is going away at the same time the edit control is going away, since this
//  current font is selected into the edit control.

    m_cCurrentFont.DeleteObject();

    delete m_pcTfontTbar;
    }

/******************************************************************************/

BOOL CTfont::Create( CRect rectEditArea )
    {
    BOOL bRC = TRUE;

    CRect cRectWindow( 0, 0, 0, 0 );

    CString pWindowName;

    pWindowName.LoadString( IDS_FONT_TOOL);

    bRC = CMiniFrmWnd::Create( pWindowName, 0, cRectWindow, AfxGetMainWnd() );

    if (bRC != FALSE)
        {
        TRY
            {
            bRC = m_cTfontDlg.Create   ( this );   // can throw excpetion
            bRC = m_pcTfontTbar->Create( this );   // can throw excpetion

            ResizeWindow();
            }
        CATCH(CResourceException, e)
            {
            /*DK* ##ERROR CResourceException caught, could not create either toolbar or dialog bar object */
            }
        END_CATCH
        }
    else
        {
        /*DK* ##ERROR Could not create window for font tool box*/
        }

    ASSERT( bRC != FALSE );

    if (bRC != FALSE)
        {
        RefreshFontList();

        //Select the first item in the combobox.

        CComboBox* pCBox = (CComboBox*)m_cTfontDlg.GetDlgItem( IDC_TYPEFACE );

        ASSERT( pCBox != NULL );

        if (pCBox != NULL)
            {
            pCBox->SetCurSel(0);

            // simulate a selection
            OnSelendokTypeface();
            }
        ReadFromIniFile();  // will show the window the same state as saved

        // make sure the font tools does not show up on top of the edit box
        CRect rectFont;
        CRect rect;

        GetWindowRect( &rectFont );

        if (rect.IntersectRect( &rectEditArea, &rectFont ))
            SetWindowPos( &wndTop, rectFont.left,
                          rectEditArea.top - (rectFont.Height() + theApp.m_cyFrame),
                          0, 0, SWP_NOSIZE );

        if (theApp.m_bShowTextToolbar)
            ShowWindow( SW_SHOWNOACTIVATE );
        }
    return bRC;
    }

/***************************************************************************/

BOOL CTfont::PreCreateWindow(CREATESTRUCT& cs)
{
        BOOL bRet = CMiniFrmWnd::PreCreateWindow(cs);

        if (bRet)
        {
                // We don't want CLIENTEDGE on this window
                cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
        }

        return(bRet);
}

/******************************************************************************/

void CTfont::GetFontInfo(int iSelection)
{
        CFontComboBox* pCBox = (CFontComboBox*)m_cTfontDlg.GetDlgItem(IDC_TYPEFACE);

        CFontDesc* pDesc = pCBox->GetFontDesc(iSelection);

        m_strFontName = pDesc->m_strName;
        m_iFontType   = pDesc->m_iFontType;
        m_nCharSet    = pDesc->m_nCharSet;
}


void CTfont::Undo(void)
    {
    switch( m_iControlIDLastChange )
        {
        case IDC_TYPEFACE:
            if (m_cStrTypeFaceName.Compare(m_cStrTypeFaceNamePrev) != 0)
                {
                TRY
                    {
                    CString cStrTemp = m_cStrTypeFaceName;

                    m_cStrTypeFaceName      = m_cStrTypeFaceNamePrev;
                    m_cStrTypeFaceNamePrev = cStrTemp;

                    CFontComboBox* pCBox = (CFontComboBox*)m_cTfontDlg.GetDlgItem(IDC_TYPEFACE);

                    int iSelection = pCBox->SelectString( -1, m_cStrTypeFaceName );

                    ASSERT( iSelection != CB_ERR );

                    if (iSelection != CB_ERR)
                        {
                                                GetFontInfo(iSelection);

                        ProcessNewTypeface();
                        }
                    }
                CATCH(CMemoryException,e)
                    {
                    /*DK* ##ERROR CString Thrown CMemoryException*/
                    }
                END_CATCH
                }

            // the processing of process new font may cause a re-selection in the
            // combo box (same code as user selection) changing this value.  We
            // must re-set for undo of undo.
            m_iControlIDLastChange = IDC_TYPEFACE;

            break;

        case IDC_POINTSIZE:
            if (m_iPointSize != m_iPointSizePrev)
                {
                int iPointSizeTemp = m_iPointSize;

                m_iPointSize     = m_iPointSizePrev;
                m_iPointSizePrev = iPointSizeTemp;

                m_cTfontDlg.SetDlgItemInt( IDC_POINTSIZE, m_iPointSize );

                UpdateEditControlFont();
                }
            break;

        case IDC_BOLD:
            OnBold();
            RefreshToolBar();
            break;

        case IDC_ITALIC:
            OnItalic();
            RefreshToolBar();
            break;

        case IDC_UNDERLINE:
            OnUnderline();
            RefreshToolBar();
            break;

#ifdef DBCS //VertEdit
        case IDC_VERTEDIT:
            OnVertEdit();
            RefreshToolBar();
            break;
#endif //DBCS

        case IDC_SHADOW:
            OnShadow();
            RefreshToolBar();
            break;

        default:
            break;
        }
    }

/******************************************************************************/

void CTfont::RefreshToolBar(void)
    {
    // Set the bold button state
    if (m_bBoldOn)
        {
        m_pcTfontTbar->SetButtonInfo( BOLD_TBAR_POS, IDC_BOLD, TBBS_CHECKBOX | TBBS_CHECKED, BOLD_BMP_POS);
        }
    else
        {
        m_pcTfontTbar->SetButtonInfo( BOLD_TBAR_POS, IDC_BOLD, TBBS_CHECKBOX, BOLD_BMP_POS);
        }

    // Set the italic button state
    if (m_bItalicOn)
        {
        m_pcTfontTbar->SetButtonInfo( ITALIC_TBAR_POS, IDC_ITALIC, TBBS_CHECKBOX | TBBS_CHECKED, ITALIC_BMP_POS);
        }
    else
        {
        m_pcTfontTbar->SetButtonInfo( ITALIC_TBAR_POS, IDC_ITALIC, TBBS_CHECKBOX, ITALIC_BMP_POS);
        }

    // Set the underline button state
    if (m_bUnderlineOn)
        {
        m_pcTfontTbar->SetButtonInfo(UNDERLINE_TBAR_POS, IDC_UNDERLINE, TBBS_CHECKBOX | TBBS_CHECKED, UNDERLINE_BMP_POS);
        }
    else
        {
        m_pcTfontTbar->SetButtonInfo(UNDERLINE_TBAR_POS, IDC_UNDERLINE, TBBS_CHECKBOX, UNDERLINE_BMP_POS);
        }

#ifdef DBCS //VertEdit
    // Set the VertEdit button state
        if (m_bVertEditOn)
        {
        m_pcTfontTbar->SetButtonInfo(VERTEDIT_TBAR_POS, IDC_VERTEDIT, TBBS_CHECKBOX | TBBS_CHECKED, VERTEDIT_BMP_POS);
        }
    else
        {
        m_pcTfontTbar->SetButtonInfo(VERTEDIT_TBAR_POS, IDC_VERTEDIT, TBBS_CHECKBOX, VERTEDIT_BMP_POS);
        }
#endif //DBCS

    // Set the underline button state
    if (theApp.m_bPenSystem)
        if (m_bPenOn)
            {
            m_pcTfontTbar->SetButtonInfo(PEN_TBAR_PEN_POS, IDC_PENEXT, TBBS_CHECKBOX | TBBS_CHECKED, PEN_BMP_POS);
            }
        else
            {
            m_pcTfontTbar->SetButtonInfo(PEN_TBAR_TEXT_POS, IDC_PENEXT, TBBS_CHECKBOX, PEN_BMP_POS);
            }

    // Set the shadow button state
//  currently this is not present on the toolbar.  When it is available, fix the
//  SHADOW_TBAR_POS and SHADOW_BMP_POS #define in tfont.h

//   if (m_bShadowOn)
//       {
//       m_pcTfontTbar->SetButtonInfo(SHADOW_TBAR_POS, IDC_SHADOW, TBBS_CHECKBOX | TBBS_CHECKED, SHADOW_BMP_POS);
//       }
//   else
//       {
//       m_pcTfontTbar->SetButtonInfo(SHADOW_TBAR_POS, IDC_SHADOW, TBBS_CHECKBOX, SHADOW_BMP_POS);
//       }
    }

/******************************************************************************/

void CTfont::ResizeWindow( void )
    {
    CRect cClientRect;
    CRect cRectDlgBar;
    CRect cRectTbar;

    int ixPosDlgBar   = 0;
    int iyPosDlgBar   = 0;
    int ixPosTbar     = 0;
    int iyPosTbar     = 0;
    int iWindowHeight = 0;
    int iWindowWidth  = 0;
    int iBorder       = 0;
    int ixNCBorder    = 0;
    int iyNCBorder    = 0;

    ixNCBorder += theApp.m_cxBorder * 2;
    iyNCBorder += theApp.m_cyBorder * 2;
    iyNCBorder += theApp.m_cyCaption;

    m_cTfontDlg.GetWindowRect( &cRectDlgBar );

    cRectTbar.SetRectEmpty();

    m_pcTfontTbar->GetWindowRect( &cRectTbar );

    // /4 since border is for top/bottom or left/right
    // the boder is 1/4 the height of the tool/dialog bar
    iBorder += cRectDlgBar.Height() / 4;

    // Compute the Width
    // Width is combination of both
    iWindowWidth += ixNCBorder;
    iWindowWidth += cRectDlgBar.Width();
    iWindowWidth += cRectTbar.Width();
    iWindowWidth += 3 * iBorder;  // border on left and right and between dlgbar and tbar

    // Compute the Height
    // Height is combination of both
    iWindowHeight += iyNCBorder;
    iWindowHeight += cRectDlgBar.Height();
    iWindowHeight += 2 * iBorder;  // border on top and bottom

    // position the main window
    if (GetSafeHwnd() != NULL)
        {
        // size this window to fit children
        SetWindowPos(&wndTop, 0, 0, iWindowWidth, iWindowHeight, SWP_NOMOVE);
        }

    GetClientRect(&cClientRect);

    // calculate the x positions of the 2 control bars (next to eachother)
    ixPosDlgBar   = iBorder;
    ixPosTbar     = iBorder * 2 + cRectDlgBar.Width();

    // center the 2 control bars in the vertical position
    iyPosDlgBar   = (cClientRect.Height() - cRectDlgBar.Height()) / 2;
    iyPosTbar     = (cClientRect.Height() - cRectTbar.Height()) / 2;

    // Position the Dialog Bar
    if (m_cTfontDlg.GetSafeHwnd() != NULL)
        {
        m_cTfontDlg.SetWindowPos(&wndTop, ixPosDlgBar, iyPosDlgBar, 0, 0, SWP_NOSIZE);
        m_cTfontDlg.ShowWindow(SW_SHOWNOACTIVATE);
        }

    // Position the Toolbar
    if (m_pcTfontTbar->GetSafeHwnd() != NULL)
        {
        m_pcTfontTbar->SetWindowPos(&wndTop, ixPosTbar, iyPosTbar, 0, 0, SWP_NOSIZE);
        m_pcTfontTbar->ShowWindow(SW_SHOWNOACTIVATE);
        }
    }

/******************************************************************************/


/******************************************************************************/
/*                                                                            */
/* Ini File section format  PBrush.INI in Windows Dir                         */
/* [Text]                                                                     */
/* TypeFaceName= string                                                       */
/* PointSize=  #                                                              */
/* FontPalette= x y nCmdShow  (x,y) is location screen.  nCmdShow is param to */
/* Bold= ON   F                              ShowWindow of SW_SHOW or SW_HIDE */
/* Italic= ON                                                                 */
/* Underline= ON                                                              */
/*                                                                            */
/******************************************************************************/

void CTfont::SaveToIniFile(void)
    {
    theApp.m_iPointSize      = m_iPointSize;
    theApp.m_strTypeFaceName = m_cStrTypeFaceName;
    theApp.m_iPosTextX       = m_cRectWindow.left;
    theApp.m_iPosTextY       = m_cRectWindow.top;
    theApp.m_iBoldText       = m_bBoldOn;
    theApp.m_iItalicText     = m_bItalicOn;
    theApp.m_iUnderlineText  = m_bUnderlineOn;
#ifdef  DBCS    //VertEdit
    theApp.m_iVertEditText   = m_bVertEditOn;
#endif  //DBCS
    theApp.m_iPenText        = m_bPenOn;
    }

/******************************************************************************/
/* See Save to ini for format of ini file                                     */

void CTfont::ReadFromIniFile(void)
    {
    CRect rect;

    GetWindowRect( &rect );

    CSize size = rect.Size();

    CPoint ptPos = theApp.CheckWindowPosition( CPoint( theApp.m_iPosTextX,
                                                       theApp.m_iPosTextY ),
                                               size );
    if (! ptPos.x
    &&  ! ptPos.y)
        {
        AfxGetMainWnd()->GetWindowRect( &rect );

        rect.OffsetRect( 15, 15 );

        ptPos.x = rect.left;
        ptPos.y = rect.top;
        }

    if (GetSafeHwnd() != NULL)
        {
        SetWindowPos(&wndTop, ptPos.x, ptPos.y, 0, 0, SWP_NOSIZE);
        }

    if (theApp.m_strTypeFaceName.IsEmpty() == 0)
        {
        m_cStrTypeFaceName = theApp.m_strTypeFaceName;
        m_iPointSize = theApp.m_iPointSize;

//        m_iPointSize = 0; // 0 is initial value => 1st in list will be selected

        CFontComboBox* pCBox = (CFontComboBox*)m_cTfontDlg.GetDlgItem(IDC_TYPEFACE);

        int iSelection = pCBox->SelectString(-1, m_cStrTypeFaceName);

        if (iSelection != CB_ERR)
            {
            GetFontInfo(iSelection);

            ProcessNewTypeface();  // this will also set the pointsize to what we have in our variable
            }
        }

    if (theApp.m_iBoldText)
        {
        m_bBoldOn = FALSE;
        OnBold();  // toggles from false to TRUE
        m_pcTfontTbar->SetButtonInfo(BOLD_TBAR_POS, IDC_BOLD,  TBBS_CHECKBOX | TBBS_CHECKED, BOLD_BMP_POS);
        }

    if (theApp.m_iItalicText)
        {
        m_bItalicOn = FALSE;
        OnItalic();  // toggles from false to TRUE
        m_pcTfontTbar->SetButtonInfo(ITALIC_TBAR_POS, IDC_ITALIC,  TBBS_CHECKBOX | TBBS_CHECKED, ITALIC_BMP_POS);
        }

    if (theApp.m_iUnderlineText)
        {
        m_bUnderlineOn = FALSE;
        OnUnderline();  // toggles from false to TRUE
        m_pcTfontTbar->SetButtonInfo(UNDERLINE_TBAR_POS, IDC_UNDERLINE,  TBBS_CHECKBOX | TBBS_CHECKED, UNDERLINE_BMP_POS);
        }

#ifdef  DBCS    //VertEdit
        if (theApp.m_iVertEditText == -1)       //no setting in profile
                {
                        theApp.m_iVertEditText = (IS_DBCS_CHARSET(m_nCharSet)) ? FALSE : 2;
                }

        if (theApp.m_iVertEditText == 2)
                {
                m_bVertEditOn = 2;
        m_pcTfontTbar->SetButtonInfo(VERTEDIT_TBAR_POS, IDC_VERTEDIT,  TBBS_CHECKBOX | TBBS_DISABLED, VERTEDIT_BMP_POS);
                }
    else if (theApp.m_iVertEditText)
        {
        m_bVertEditOn = FALSE;
        OnVertEdit();  // toggles from false to TRUE
        m_pcTfontTbar->SetButtonInfo(VERTEDIT_TBAR_POS, IDC_VERTEDIT,  TBBS_CHECKBOX | TBBS_CHECKED, VERTEDIT_BMP_POS);
        }
        else
                {
                m_bVertEditOn = FALSE;
        m_pcTfontTbar->SetButtonInfo(VERTEDIT_TBAR_POS, IDC_VERTEDIT,  TBBS_CHECKBOX, VERTEDIT_BMP_POS);
                }
#endif  //DBCS

    if (theApp.m_bPenSystem)
        if (theApp.m_iPenText)
            {
            m_bPenOn = FALSE;
            OnPen();  // toggles from false to TRUE
            }
    }

/******************************************************************************/

void CTfont::RefreshFontList(void)
    {
        CFontComboBox* pBox = (CFontComboBox *)m_cTfontDlg.GetDlgItem(IDC_TYPEFACE);
        pBox->ClearList();

    CClientDC cdcWindow(this);
    HDC hDC = cdcWindow.GetSafeHdc();

    ASSERT(hDC != NULL);

    if (hDC != NULL)
        {
        FONTENUMPROC lpEnumFamCallBack;
        lpEnumFamCallBack = (FONTENUMPROC) CTfont::EnumFontFaceProc;

        LOGFONT lf;
        memset(&lf, 0, sizeof(lf));
        lf.lfCharSet = DEFAULT_CHARSET;

        ::EnumFontFamiliesEx(hDC, &lf, lpEnumFamCallBack, (LPARAM) this, NULL);
        }
    }

/******************************************************************************/

int CFontComboBox::AddFontName(LPCTSTR lpszName, LPCTSTR lpszScript, BYTE nCharSet,
        int iFontType)
{
        CString str = lpszName;
        str += TEXT(" (");
        str += lpszScript;
        str += TEXT(")");

        LPCTSTR lpszFontName = str;

        ASSERT(lpszFontName != NULL);
        int nIndex = FindStringExact(-1, lpszFontName);
        if (nIndex != CB_ERR)
        {
                return(nIndex);
        }

        CFontDesc* pDesc = new CFontDesc(lpszName, nCharSet, iFontType);
        if (!pDesc)
        {
                return(-1);
        }

        nIndex = AddString(lpszFontName);
        ASSERT(nIndex >=0);
        if (nIndex >=0) //no error
        {
                SetItemData(nIndex, (DWORD)pDesc);
        }
        else
        {
                delete pDesc;
        }

        return nIndex;
}

void CFontComboBox::ClearList()
{
        // destroy all the CFontDesc's
        int nCount = GetCount();
        for (int i=0;i<nCount;i++)
                delete GetFontDesc(i);

        ResetContent();
}

BOOL CFontComboBox::IsSameName(CFontDesc* pDesc, int index)
{
        CFontDesc* pDescOther = GetFontDesc(index);
        if (pDescOther == (CFontDesc*)CB_ERR)
        {
                return(FALSE);
        }

        return(lstrcmp(pDesc->m_strName, pDescOther->m_strName) == 0);
}

int CTfont::EnumFontFace( ENUMLOGFONTEX*   lpEnumLogFont,
                          NEWTEXTMETRICEX* lpNewTextMetric,
                          int iFontType )
{
        // only enumerate TrueType faces
        // in DBCS builds also exclude vertical faces
        if ((lpEnumLogFont->elfLogFont.lfCharSet != OEM_CHARSET)
                && (lpEnumLogFont->elfLogFont.lfCharSet != MAC_CHARSET)
#ifdef DBCS
                && (lpEnumLogFont->elfLogFont.lfFaceName[0] != TEXT('@'))
#endif
                )
        {
                CFontComboBox* pBox = (CFontComboBox *)m_cTfontDlg.GetDlgItem(IDC_TYPEFACE);

                pBox->AddFontName(lpEnumLogFont->elfLogFont.lfFaceName,
                        (LPCTSTR)lpEnumLogFont->elfScript, lpEnumLogFont->elfLogFont.lfCharSet, iFontType);
        }
        return 1;
}

/******************************************************************************/

int CALLBACK CTfont::EnumFontFaceProc( ENUMLOGFONTEX*   lpEnumLogFont,
                                       NEWTEXTMETRICEX* lpNewTextMetric,
                                       int iFontType, LPARAM lParam )
    {
    class CTfont* pCTfont;

    ASSERT(lParam != NULL);

    if (lParam != NULL)
        {
        pCTfont = (CTfont*)lParam;
        return pCTfont->EnumFontFace(lpEnumLogFont, lpNewTextMetric, iFontType);
        }

    return 0;
    }

/******************************************************************************/

int  CTfont::EnumFontSizes( LPENUMLOGFONT lpEnumLogFont,
                            LPNEWTEXTMETRIC lpNewTextMetric,
                            int iFontType )
    {
    int iPtSize;

    /* testint */

    CAttrEdit* pcEdit = m_pcTedit->GetEditWindow();

    ASSERT(pcEdit != NULL);

    if (pcEdit != NULL)
        {
        CClientDC cdcClient( pcEdit );

        // ptsize = char height * 72 / pixels  per inch
        // char height = cell height - internal leading

        iPtSize = MulDiv( lpNewTextMetric->tmHeight -
                          lpNewTextMetric->tmInternalLeading,
                      72, cdcClient.GetDeviceCaps( LOGPIXELSY ) );
        }

    TCHAR buffTmp[10];

    // Leading zero prefixed for Combobox sorting order.
    wsprintf( buffTmp, TEXT("%2d"), iPtSize );

    CComboBox* pCBox = (CComboBox*)m_cTfontDlg.GetDlgItem(IDC_POINTSIZE);

    ASSERT (pCBox != NULL);

    if (pCBox != NULL)
        {
        // only add the string if it does not exist
        int iRC = pCBox->FindStringExact(-1, buffTmp);

        if (iRC == CB_ERR)
            {
            pCBox->AddString(buffTmp);
            }
        }
    return 1;
    }

/******************************************************************************/

int CALLBACK CTfont::EnumFontOneFaceProc( LPENUMLOGFONT lpEnumLogFont,
                                                  LPNEWTEXTMETRIC lpNewTextMetric,
                                                  int iFontType, LPARAM lParam )
    {
    class CTfont* pCTfont;

    ASSERT(lParam != NULL);

    if (lParam != NULL)
        {
        pCTfont = (CTfont*)lParam;
        return pCTfont->EnumFontSizes(lpEnumLogFont, lpNewTextMetric, iFontType);
        }

    return 0;
    }

/******************************************************************************/
// User selected a new typeface in the combo box
/******************************************************************************/

void CTfont::ProcessNewTypeface(void)
    {
    CString cStringText;
    CClientDC cdcDlgBox( this );
    HDC hDC = cdcDlgBox.GetSafeHdc();

    ASSERT(hDC != NULL);

    if (hDC != NULL)
        {
        FONTENUMPROC lpEnumFamCallBack;
        lpEnumFamCallBack = (FONTENUMPROC)CTfont::EnumFontOneFaceProc;

        CComboBox* pCBox = (CComboBox*)m_cTfontDlg.GetDlgItem(IDC_POINTSIZE);

        ASSERT (pCBox != NULL);

        if (pCBox != NULL)
            {
            pCBox->ResetContent();

            // only do this if non-true-type font  if true type, filll in with  default sizes
            if (
                (m_iFontType & TRUETYPE_FONTTYPE) ||
                !( (m_iFontType & TRUETYPE_FONTTYPE) || (m_iFontType & RASTER_FONTTYPE) )
                ) // if truetype or vector font
                {
                // True Type and Vector Fonts are continuously scallable.
                // There are the reccomended values
                pCBox->AddString(TEXT(" 8"));
                pCBox->AddString(TEXT(" 9"));
                pCBox->AddString(TEXT("10"));
                pCBox->AddString(TEXT("11"));
                pCBox->AddString(TEXT("12"));
                pCBox->AddString(TEXT("14"));
                pCBox->AddString(TEXT("16"));
                pCBox->AddString(TEXT("18"));
                pCBox->AddString(TEXT("20"));
                pCBox->AddString(TEXT("22"));
                pCBox->AddString(TEXT("24"));
                pCBox->AddString(TEXT("26"));
                pCBox->AddString(TEXT("28"));
                pCBox->AddString(TEXT("36"));
                pCBox->AddString(TEXT("48"));
                pCBox->AddString(TEXT("72"));
                }
            else
                {
                ::EnumFontFamilies(hDC, m_strFontName, lpEnumFamCallBack, (LPARAM) this);
                }

            // 0 is uninitialized value
            if (m_iPointSize != 0)
                {
                m_cTfontDlg.SetDlgItemInt(IDC_POINTSIZE, m_iPointSize);
                }
            else
                {
                pCBox->SetCurSel(0);
                }
           // simulate a selection
           OnPointSizeComboBoxUpdate();
           }
       }
    }

/******************************************************************************/
// change the font in the edit control based on the font selection
/******************************************************************************/

void CTfont::UpdateEditControlFont(void)
    {
    LOGFONT    lf;
    CFont*     pcOldFont;
    BOOL       bRC;
    HFONT      hFont;
    int        iCellHeight;
    TEXTMETRIC tm;

    BeginWaitCursor();

    CAttrEdit* pcEdit = m_pcTedit->GetEditWindow();

    ASSERT(pcEdit != NULL);

    if (pcEdit != NULL)
        {
        CClientDC cdcClient( pcEdit );

        bRC = cdcClient.GetTextMetrics(&tm);

        // ptsize = char height * 72 / pixels  per inch
        // char height = cell height - internal leading
        iCellHeight = MulDiv( m_iPointSize, cdcClient.GetDeviceCaps(LOGPIXELSY),
                                                     72) + tm.tmInternalLeading;
        }

    lf.lfWidth          = 0;
    lf.lfHeight         = iCellHeight;
#ifdef DBCS //VertEdit
    lf.lfEscapement     = (m_bVertEditOn == TRUE) ? 2700 : 0;
    lf.lfOrientation    = (m_bVertEditOn == TRUE) ? 2700 : 0;
#else //DBCS
    lf.lfEscapement     = 0;
    lf.lfOrientation    = 0;
#endif //DBCS
    lf.lfWeight         = m_iWeight;
    lf.lfItalic         = m_bItalicOn;
    lf.lfUnderline      = m_bUnderlineOn;
    lf.lfStrikeOut      = 0;
    lf.lfCharSet        = m_nCharSet;
    lf.lfOutPrecision   = OUT_DEFAULT_PRECIS;
    lf.lfClipPrecision  = CLIP_DEFAULT_PRECIS;
    lf.lfQuality        = DEFAULT_QUALITY;
    lf.lfPitchAndFamily = FF_DONTCARE | DEFAULT_PITCH;

#ifdef DBCS //VertEdit
        if ( (m_bVertEditOn == TRUE) && IS_DBCS_CHARSET( lf.lfCharSet ) )
        {
                lstrcpy(lf.lfFaceName, TEXT(TEXT("@")));
                strncat(lf.lfFaceName, m_strFontName, sizeof(lf.lfFaceName) - sizeof(TCHAR));
        }
        else
#endif //DBCS
    _tcsncpy(lf.lfFaceName, m_strFontName, sizeof(lf.lfFaceName)/sizeof(TCHAR) - 1);

    hFont = (HFONT)m_cCurrentFont.Detach();
    bRC = m_cCurrentFont.CreateFontIndirect(&lf);

    ASSERT(bRC != 0);

    if (bRC != 0)
        {
        if (m_pcTedit != NULL)
            {
            CAttrEdit* pcEdit = m_pcTedit->GetEditWindow();

            ASSERT( pcEdit != NULL );

            pcOldFont = pcEdit->GetFont();
            pcEdit->SetFont( &m_cCurrentFont );
            m_pcTedit->RefreshWindow();
            }

        //hFont is last font allocated here
        if (hFont != NULL)
            {
            ::DeleteObject( hFont );
            }
        }
    else
        {
        /*DK* ##ERROR Could not create font indirect */
        }

    ASSERT( m_pcTedit != NULL );

    if (m_pcTedit != NULL && bRC != 0)
        {
        m_pcTedit->OnAttrEditFontChange();
        }

    EndWaitCursor();
    }

/******************************************************************************/
// Combo box for type face changed, determine what processing to do
/******************************************************************************/

void CTfont::OnTypeFaceComboBoxUpdate(void)
    {
    int iSelection;
    CString cStringText;

//  LPFONTINFORMATION lpFontInformation;

    CComboBox* pCBox = (CComboBox*)m_cTfontDlg.GetDlgItem(IDC_TYPEFACE);

    ASSERT(pCBox != NULL);

    if (pCBox != NULL)
        {
        TRY
            {
            iSelection = pCBox->GetCurSel();

            ASSERT(iSelection != CB_ERR);

            if (iSelection != CB_ERR)
                {
                pCBox->GetLBText(iSelection, cStringText);
                }
            }
        CATCH(CMemoryException,e)
            {
            cStringText.Empty();
            /*DK* ##ERROR CString Thrown CMemoryException*/
            }
        END_CATCH

        if (m_cStrTypeFaceName.Compare(cStringText) != 0)
            {

#ifdef DBCS //VertEdit
                        CFontComboBox* pCBox = (CFontComboBox*)m_cTfontDlg.GetDlgItem(IDC_TYPEFACE);
                        CFontDesc* pDesc = pCBox->GetFontDesc(iSelection);
                        if ( IS_DBCS_CHARSET( pDesc->m_nCharSet ) )
                                {
                                if ( m_bVertEditOn == 2 )
                                        {
                                        m_bVertEditOn = FALSE;
                                m_pcTfontTbar->SetButtonInfo(VERTEDIT_TBAR_POS, IDC_VERTEDIT, TBBS_CHECKBOX, VERTEDIT_BMP_POS);
                                        }
                                }
                        else
                                {
                                if ( m_bVertEditOn == TRUE )
                                        {
                                        AfxMessageBox(IDS_ERROR_DBCSFONTONLY);
                        pCBox->SelectString( -1, m_cStrTypeFaceName );
                                        return;
                                        }
                                else
                                        {
                                        m_bVertEditOn = 2;
                                m_pcTfontTbar->SetButtonInfo(VERTEDIT_TBAR_POS, IDC_VERTEDIT, TBBS_CHECKBOX | TBBS_DISABLED, VERTEDIT_BMP_POS);
                                }
                                }
#endif //DBCS

            // could have been set in OnCloseUpTypeFace method, so don't do double
            // processing
            TRY
                {
                m_cStrTypeFaceNamePrev = m_cStrTypeFaceName;
                m_cStrTypeFaceName = cStringText;
                }
            CATCH(CMemoryException,e)
                {
                m_cStrTypeFaceName.Empty();
                /*DK* ##ERROR CString Thrown CMemoryException*/
                }
            END_CATCH

            GetFontInfo(iSelection);

            ProcessNewTypeface();
            m_iControlIDLastChange = IDC_TYPEFACE;
            }
        }
    }

/******************************************************************************/

void CTfont::OnPointSizeComboBoxUpdate(void)
    {
    int iSelection;
    int iHeight;
    CString cStringText;

    CComboBox* pCBox = (CComboBox*)m_cTfontDlg.GetDlgItem(IDC_POINTSIZE);

    ASSERT(pCBox != NULL);

    if (pCBox != NULL)
        {
        TRY
            {
            iSelection = pCBox->GetCurSel();

            if (iSelection != CB_ERR)
                {
                pCBox->GetLBText(iSelection, cStringText);
                iHeight = Atoi(cStringText);
                }
            else
                {
                // if no selection, get displayed value in combo edit part
                iHeight = m_cTfontDlg.GetDlgItemInt(IDC_POINTSIZE);
                }
            }
        CATCH(CMemoryException,e)
            {
            cStringText.Empty();
            /*DK* ##ERROR CString Thrown CMemoryException*/
            }
        END_CATCH


        if (iHeight !=0 )
            {
            if (iHeight != m_iPointSize )
                                {
                    // could have been set in OnCloseUpTypeFace method, so don't do double
                    // processing
                    m_iPointSizePrev = m_iPointSize;
                    m_iPointSize = iHeight;
                    }
                        }
                else
                        {
                        AfxMessageBox(IDS_ERROR_FONTSIZENUMERIC);
                        m_cTfontDlg.SetDlgItemInt(IDC_POINTSIZE,m_iPointSize);
                        }

        // need to call update font  because could be same size with diff
        // typeface, and we got called indirectly by filling the combo box
        UpdateEditControlFont();
        m_iControlIDLastChange = IDC_POINTSIZE;
        }
    }

/******************************************************************************/

void CTfont::OnSelendokTypeface()
    {
    if (! m_bInUpdate)
        {
        m_bInUpdate = TRUE;
        OnTypeFaceComboBoxUpdate();
        m_bInUpdate = FALSE;
        }
    }

/******************************************************************************/

void CTfont::OnCloseupTypeface()
    {
    if (! m_bInUpdate)
        {
        m_bInUpdate = TRUE;
        OnTypeFaceComboBoxUpdate();
        m_bInUpdate = FALSE;
        }
    }

/******************************************************************************/

void CTfont::OnSelendokPointSize()
    {
    if (! m_bInUpdate)
        {
        m_bInUpdate = TRUE;
        OnPointSizeComboBoxUpdate();
        m_bInUpdate = FALSE;
        }
    }

/******************************************************************************/

void CTfont::OnCloseupPointSize()
    {
    if (! m_bInUpdate)
        {
        m_bInUpdate = TRUE;
        OnPointSizeComboBoxUpdate();
        m_bInUpdate = FALSE;
        }
    }

/******************************************************************************/

void CTfont::OnKillfocusPointsize()
    {
    if (! m_bInUpdate)
        {
        m_bInUpdate = TRUE;
        // in case the user tabs off
        OnPointSizeComboBoxUpdate();
        m_bInUpdate = FALSE;
        }
    }

/******************************************************************************/

void CTfont::OnRButtonDown(UINT nFlags, CPoint point)
    {
#if 0
    CMenu cMenuPopup;
    CMenu *pcContextMenu;
    BOOL  bRC;

    bRC = cMenuPopup.LoadMenu(IDR_TEXT_POPUP);

    ASSERT(bRC != 0);

    if (bRC != 0)
        {
        pcContextMenu = cMenuPopup.GetSubMenu(ID_TOOL_POPUPMENU_POS);
        ASSERT(pcContextMenu != NULL);
        if (pcContextMenu != NULL)
            {
            ClientToScreen(&point);
            pcContextMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this, NULL);
            }
        }
    else
        {
        /*DK* ##ERROR Could not loadmenu */
        }
#endif
    }

/******************************************************************************/

void CTfont::OnMove(int x, int y)
    {
    CMiniFrmWnd::OnMove(x, y);

    // TODO: Add your message handler code here
    GetWindowRect(&m_cRectWindow);
    }

/******************************************************************************/

void CTfont::OnClose()
    {
    theApp.m_bShowTextToolbar = FALSE;

    ShowWindow( SW_HIDE );
    }

/******************************************************************************/

BOOL CTfont::OnEraseBkgnd( CDC* pDC )
    {
    CRect rect;

    GetClientRect( rect );

    pDC->FillRect( rect, GetSysBrush( COLOR_BTNFACE ) );

        return CMiniFrmWnd::OnEraseBkgnd( pDC );
    }

/******************************************************************************/

void CTfont::OnSetFocus(CWnd* pOldWnd)
    {
    CMiniFrmWnd::OnSetFocus(pOldWnd);

    CComboBox* pCBox = (CComboBox*)m_cTfontDlg.GetDlgItem( IDC_TYPEFACE );

    ASSERT(pCBox != NULL);

    if (pCBox != NULL)
        {
        pCBox->SetFocus();
        }
    }

/******************************************************************************/

void CTfont::OnDestroy()
{
        CFontComboBox* pBox = (CFontComboBox *)m_cTfontDlg.GetDlgItem(IDC_TYPEFACE);
        pBox->ClearList();

        CMiniFrmWnd::OnDestroy();
}

/******************************************************************************/

void CTfont::OnBold(void)
    {
    m_iControlIDLastChange = IDC_BOLD;

    m_bBoldOn = !m_bBoldOn;

    if (m_bBoldOn)
        {
        m_iWeight = FW_BOLD;
        }
    else
        {
        m_iWeight = FW_NORMAL;
        }

    UpdateEditControlFont();
    }

/******************************************************************************/

void CTfont::OnItalic(void)
    {
    m_bItalicOn = !m_bItalicOn;

    UpdateEditControlFont();

    m_iControlIDLastChange = IDC_ITALIC;
    }

/******************************************************************************/

void CTfont::OnUnderline(void)
    {
    m_bUnderlineOn = !m_bUnderlineOn;

    UpdateEditControlFont();

    m_iControlIDLastChange = IDC_UNDERLINE;
    }

#ifdef DBCS //VertEdit
/******************************************************************************/

void CTfont::OnVertEdit(void)
    {
        if (m_bVertEditOn == 2)
                {
                return;
                }

    m_bVertEditOn = !m_bVertEditOn;

        m_pcTedit->m_bVertEdit = m_bVertEditOn;

    UpdateEditControlFont();

    m_iControlIDLastChange = IDC_VERTEDIT;
    }

/******************************************************************************/

void CTfont::OnVertEditUpdate(CCmdUI* pCmdUI)
    {
        pCmdUI->Enable( !(m_bVertEditOn == 2) );
    }
#endif //DBCS

/******************************************************************************/

void CTfont::OnShadow(void)
    {
    m_bShadowOn = !m_bShadowOn;

    UpdateEditControlFont();

    m_iControlIDLastChange = IDC_SHADOW;
    }

/******************************************************************************/

void CTfont::OnPen(void)
    {
    PostMessage( UM_DELAYED_TOOLBAR );
    }

/******************************************************************************/

long CTfont::OnDelayedPen( WPARAM, LPARAM )
    {
    if (! theApp.m_bPenSystem)
        m_bPenOn = FALSE;

    m_bPenOn               = !m_bPenOn;
    m_iControlIDLastChange = IDC_PENEXT;

    delete m_pcTfontTbar;

    m_pcTfontTbar = new CTfontTbar();

    BOOL bRC = m_pcTfontTbar->Create( this, m_bPenOn );

    if (bRC)
        {
        ResizeWindow();
        RefreshToolBar();
        }
    return 0;
    }

/******************************************************************************/

void CTfont::OnEditText(void)
    {
    m_iControlIDLastChange = IDC_EDITTEXT;

    CEdit *ctl = m_pcTedit? m_pcTedit->GetEditWindow() : NULL;
    if (ctl)
        {
        ctl->SetFocus();
        #ifndef NT
        // NT doesn't support pen computing
        ctl->SendMessage(WM_PENMISC, PMSC_EDITTEXT, 0);
        #endif
        }
    }

/******************************************************************************/

void CTfont::OnKeyboard(void)
    {
    m_iControlIDLastChange = IDC_KEYBOARD;

    CEdit *ctl = m_pcTedit? m_pcTedit->GetEditWindow() : NULL;
    if (ctl)
        {
        }
    }

/******************************************************************************/

void CTfont::OnInsSpace(void)
    {
    m_iControlIDLastChange = IDC_INS_SPACE;

    CEdit *ctl = m_pcTedit? m_pcTedit->GetEditWindow() : NULL;
    if (ctl)
        {
        ctl->SendMessage(WM_CHAR, (WPARAM)VK_SPACE, 0);
        ctl->SetFocus();
        }
    }

/******************************************************************************/

void CTfont::OnBackSpace(void)
    {
    m_iControlIDLastChange = IDC_BACKSPACE;

    CEdit *ctl = m_pcTedit? m_pcTedit->GetEditWindow() : NULL;
    if (ctl)
        {
        ctl->SendMessage(WM_CHAR, (WPARAM)VK_BACK, 0);
        ctl->SetFocus();
        }
    }

/******************************************************************************/

void CTfont::OnNewLine(void)
    {
    m_iControlIDLastChange = IDC_NEWLINE;

    CEdit *ctl = m_pcTedit? m_pcTedit->GetEditWindow() : NULL;
    if (ctl)
        {
        ctl->SendMessage(WM_CHAR, (WPARAM)VK_RETURN, 0);
        ctl->SetFocus();
        }
    }

/******************************************************************************/

void CTfontDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
    {
    CString   cStringText;
    CComboBox cCBox;
    BOOL      bRC;

    TRY
        {
        ASSERT( lpDrawItemStruct->CtlType == ODT_COMBOBOX );
        ASSERT( lpDrawItemStruct->CtlID   == IDC_TYPEFACE );

        if (lpDrawItemStruct->CtlType != ODT_COMBOBOX
        ||  lpDrawItemStruct->CtlID   != IDC_TYPEFACE)
            {
            AfxThrowNotSupportedException();
            }

        // empty combo boxes have -1 id

        if (lpDrawItemStruct->itemID != 0xFFFFFFFF)
            {
            bRC = cCBox.Attach(lpDrawItemStruct->hwndItem);

            ASSERT(bRC != 0);

            if (bRC == 0)
                {
                AfxThrowNotSupportedException();
                }

            TRY
                {
                cCBox.GetLBText(lpDrawItemStruct->itemID, cStringText);
                cCBox.Detach();

                DrawItem(lpDrawItemStruct, &cStringText);
                }
            CATCH(CMemoryException,e)
                {
                cStringText.Empty();
                cCBox.Detach();
                /*DK* ##ERROR CString Thrown CMemoryException*/
                }
            END_CATCH
            }
        }
    CATCH(CNotSupportedException,e)
        {
        CWnd::OnDrawItem(nIDCtl, lpDrawItemStruct);
        }
    END_CATCH
    }

/******************************************************************************/


void CTfont::OnLButtonDown(UINT nFlags, CPoint point)
    {
    CMiniFrmWnd::OnLButtonDown(nFlags, point);
    }

/******************************************************************************/

void CTfont::OnLButtonUp(UINT nFlags, CPoint point)
    {
    CMiniFrmWnd::OnLButtonUp( nFlags, point );
    }

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
// CTfontTbar

BEGIN_MESSAGE_MAP( CTfontTbar, CToolBar )
    //{{AFX_MSG_MAP(CTfontTbar)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //  ON_COMMAND(ID_OLE_INSERT_NEW, OnInsertObject)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/******************************************************************************/
// CTfontTbar construction/destruction

CTfontTbar::CTfontTbar(void)
    {
    }

/******************************************************************************/

BOOL CTfontTbar::Create(CWnd* pcParentWnd, BOOL bShowPen)
    {
    BOOL bRC = TRUE;
    int iNumButtons;
    int iNumSeparators;

    UINT ButtonIDS[MAX_TBAR_ITEMS];

    if (bShowPen)
        {
        ButtonIDS[0]   = IDC_BOLD;
        ButtonIDS[1]   = IDC_ITALIC;
        ButtonIDS[2]   = IDC_UNDERLINE;

#ifdef DBCS //VertEdit
        ButtonIDS[3]   = IDC_VERTEDIT;

        ButtonIDS[4]   = ID_SEPARATOR;
        ButtonIDS[5]   = IDC_INS_SPACE;
        ButtonIDS[6]   = IDC_BACKSPACE;
        ButtonIDS[7]   = IDC_NEWLINE;

        ButtonIDS[8]   = ID_SEPARATOR;
        ButtonIDS[9]   = IDC_EDITTEXT;

        ButtonIDS[10]  = ID_SEPARATOR;
        ButtonIDS[11]  = IDC_PENEXT;

        iNumButtons    = 12;
#else //DBCS
        ButtonIDS[3]   = ID_SEPARATOR;
        ButtonIDS[4]   = IDC_INS_SPACE;
        ButtonIDS[5]   = IDC_BACKSPACE;
        ButtonIDS[6]   = IDC_NEWLINE;

        ButtonIDS[7]   = ID_SEPARATOR;
        ButtonIDS[8]   = IDC_EDITTEXT;

        ButtonIDS[9]   = ID_SEPARATOR;
        ButtonIDS[10]  = IDC_PENEXT;

        iNumButtons    = 11;
#endif //DBCS
        iNumSeparators = 3;
        }
    else
        {
        ButtonIDS[0] = IDC_BOLD;
        ButtonIDS[1] = IDC_ITALIC;
        ButtonIDS[2] = IDC_UNDERLINE;
#ifdef DBCS //VertEdit
        ButtonIDS[3] = IDC_VERTEDIT;
#endif //DBCS

        if (theApp.m_bPenSystem)
            {
#ifdef DBCS //VertEdit
            ButtonIDS[4] = ID_SEPARATOR;

            ButtonIDS[5] = IDC_PENEXT;
            iNumButtons    = 6;
#else //DBCS
            ButtonIDS[3] = ID_SEPARATOR;

            ButtonIDS[4] = IDC_PENEXT;
            iNumButtons    = 5;
#endif //DBCS
            iNumSeparators = 1;
            }
        else
            {
#ifdef DBCS //VertEdit
            iNumButtons    = 4;
#else //DBCS
            iNumButtons    = 3;
#endif //DBCS
            iNumSeparators = 0;
            }
        }

    bRC = CToolBar::Create( pcParentWnd, CBRS_ALIGN_TOP );

    if (bRC == 0)
        {
        AfxThrowResourceException();
        /*DK* ##ERROR Could not create toolbar object*/
        }

    bRC = LoadBitmap(IDB_TEXT_TBAR);

    if (bRC == 0)
        {
        AfxThrowResourceException();
        /*DK* ##ERROR Could not load bitmap for toolbar*/
        }

    bRC = SetButtons(ButtonIDS, iNumButtons);

    if (bShowPen)
        {
        // Set the style to be checkbox  style.
        SetButtonInfo(BOLD_TBAR_POS,      IDC_BOLD,       TBBS_CHECKBOX, BOLD_BMP_POS);
        SetButtonInfo(ITALIC_TBAR_POS,    IDC_ITALIC,     TBBS_CHECKBOX, ITALIC_BMP_POS);
        SetButtonInfo(UNDERLINE_TBAR_POS, IDC_UNDERLINE,  TBBS_CHECKBOX, UNDERLINE_BMP_POS);
#ifdef DBCS //VertEdit
        SetButtonInfo(VERTEDIT_TBAR_POS,  IDC_VERTEDIT,   TBBS_CHECKBOX, VERTEDIT_BMP_POS);
#endif //DBCS


        SetButtonInfo(INS_SPACE_TBAR_POS, IDC_INS_SPACE,  TBBS_BUTTON, INS_SPACE_BMP_POS);
        SetButtonInfo(BACKSPACE_TBAR_POS, IDC_BACKSPACE,  TBBS_BUTTON, BACKSPACE_BMP_POS);
        SetButtonInfo(NEWLINE_TBAR_POS,   IDC_NEWLINE,    TBBS_BUTTON, NEWLINE_BMP_POS);

        SetButtonInfo(EDITTEXT_TBAR_POS,  IDC_EDITTEXT,   TBBS_BUTTON, EDITTEXT_BMP_POS);

        SetButtonInfo(PEN_TBAR_PEN_POS,   IDC_PENEXT,     TBBS_CHECKBOX, PEN_BMP_POS);

// presently unused
//        SetButtonInfo(KEYBOARD_TBAR_POS,  IDC_KEYBOARD,   TBBS_BUTTON, KEYBOARD_BMP_POS);
        }
    else
        {
        // Set the style to be checkbox  style.
        SetButtonInfo(BOLD_TBAR_POS,      IDC_BOLD,       TBBS_CHECKBOX, BOLD_BMP_POS);
        SetButtonInfo(ITALIC_TBAR_POS,    IDC_ITALIC,     TBBS_CHECKBOX, ITALIC_BMP_POS);
        SetButtonInfo(UNDERLINE_TBAR_POS, IDC_UNDERLINE,  TBBS_CHECKBOX, UNDERLINE_BMP_POS);
#ifdef DBCS //VertEdit
        SetButtonInfo(VERTEDIT_TBAR_POS,  IDC_VERTEDIT,   TBBS_CHECKBOX, VERTEDIT_BMP_POS);
#endif //DBCS

        if (theApp.m_bPenSystem)
            SetButtonInfo(PEN_TBAR_TEXT_POS, IDC_PENEXT,  TBBS_CHECKBOX, PEN_BMP_POS);
        }

    CSize size = CToolBar::CalcFixedLayout( FALSE, TRUE );

    if (GetSafeHwnd() != NULL)
        {
        SetWindowPos( &wndTop, 0, 0, size.cx, size.cy, SWP_NOMOVE );
        }

    return bRC;
    }

/******************************************************************************/

CTfontTbar::~CTfontTbar(void)
    {
    }

/******************************************************************************/
/******************************************************************************/
// CTfontDlg

BEGIN_MESSAGE_MAP(CTfontDlg, CDialogBar)
    //{{AFX_MSG_MAP(CTfontDlg)
    ON_WM_RBUTTONDOWN()
    ON_WM_MEASUREITEM()
    ON_WM_DRAWITEM()
//  ON_DM_GETDEFID()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/******************************************************************************/
// CTfontDlg construction/destruction

CTfontDlg::CTfontDlg(void)
    {
       BOOL bRC;

    // Set up the Size Structures for offsets in drawing the font typeface.
    CSize cSizeBmp_TT ( 0, 0 );
    CSize cSizeBmp_PRN( 0, 0 );

    bRC = m_cPicture_TT.PictureSet( FONT_TT_BMP );

    ASSERT( bRC != 0 );

    if (bRC != 0)
        {
        cSizeBmp_TT = m_cPicture_TT.PictureSize();
        }
    else
        {
        /*DK* ##ERROR  could not create bitmap for tt font in owner draw lbox*/
        }

    bRC = m_cPicture_PRN.PictureSet( FONT_PRN_BMP );

    ASSERT( bRC != 0 );

    if (bRC != 0)
        {
        cSizeBmp_PRN = m_cPicture_PRN.PictureSize();
        }
    else
        {
        /*DK* ##ERROR  could not create bitmap for prn font in owner draw lbox*/
        }

    m_Max_cx_FontType_BMP = __max( cSizeBmp_TT.cx, cSizeBmp_PRN.cx );

    }

/******************************************************************************/

BOOL CTfontDlg::Create(CWnd* pcParentWnd)
    {
    BOOL bRC = CDialogBar::Create(pcParentWnd, IDD_TEXT_FONT_DLG,
                                               CBRS_NOALIGN, NULL);
    if (bRC)
        SetWindowPos(&wndTop, 0,0, m_sizeDefault.cx, m_sizeDefault.cy, SWP_NOMOVE);

    return bRC;
    }

/******************************************************************************/

CTfontDlg::~CTfontDlg(void)
    {
    }

/******************************************************************************/

void CTfontDlg::OnRButtonDown(UINT nFlags, CPoint point)
    {
    CWnd *pcParent = GetParent();

    const MSG *pCurrentMessage = GetCurrentMessage();

    pcParent->SendMessage(pCurrentMessage->message, pCurrentMessage->wParam,
                          pCurrentMessage->lParam);
    }

/******************************************************************************/
void CTfontDlg::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
    {
    TEXTMETRIC tm;
    BOOL bRC;

    // get default to fill in measureitem struct first
    CWnd::OnMeasureItem(nIDCtl, lpMeasureItemStruct);

    TRY
        {
        ASSERT( lpMeasureItemStruct->CtlType == ODT_COMBOBOX );
        ASSERT( lpMeasureItemStruct->CtlID   == IDC_TYPEFACE );

        if (lpMeasureItemStruct->CtlType != ODT_COMBOBOX
        ||  lpMeasureItemStruct->CtlID   != IDC_TYPEFACE)
            {
            AfxThrowNotSupportedException();
            }

        CClientDC cdcClient(this);

        bRC = cdcClient.GetTextMetrics(&tm);

        ASSERT(bRC !=0);

        if (bRC != 0)
            {
            lpMeasureItemStruct->itemHeight = tm.tmAscent + 1;
            }
        }
    CATCH(CNotSupportedException,e)
        {
        }
    END_CATCH
    }

/******************************************************************************/

void CTfontDlg::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct, CString *pcStringText)
{
   CBrush* cpBrush;
   BOOL   bSelected;
   BOOL   bRC;
   CDC    cdcCombo;
   BOOL   bDrawPicture = FALSE;
   CRect  cRectText;
   int    iPictureHeight = 0;
   CSize  cSizeBmp(0,0);
   int    ix = 0;
   int    iy = 0;
   
   CFontDesc* pDesc = (CFontDesc*)lpDrawItemStruct->itemData;
   CFontComboBox *pwndCombo = (CFontComboBox *)CFontComboBox::FromHandle(lpDrawItemStruct->hwndItem);
   if (pwndCombo)
   {
   // Only need to check the item before and after
      if (!pwndCombo->IsSameName(pDesc, lpDrawItemStruct->itemID - 1)
       && !pwndCombo->IsSameName(pDesc, lpDrawItemStruct->itemID + 1))
      {
         pcStringText = &pDesc->m_strName;
      }
   }
   
   // Set the picture object up to draw a picture if one is needed (if this
   // font is a printer/true type font).
   switch (pDesc->m_iFontType)
   {
      case DEVICE_FONTTYPE:
         bDrawPicture   = TRUE;
         cSizeBmp       = m_cPicture_PRN.PictureSize();
         iPictureHeight = cSizeBmp.cy;
         break;
   
      case TRUETYPE_FONTTYPE:
         bDrawPicture   = TRUE;
         cSizeBmp       = m_cPicture_TT.PictureSize();
         iPictureHeight = cSizeBmp.cy;
         break;
   
      default:
         bDrawPicture = FALSE;
         break;
   }
   
   TRY
   {
      if ( (lpDrawItemStruct->itemState & ODS_SELECTED) ==  ODS_SELECTED)
      {
         bSelected = TRUE;
         cpBrush = GetSysBrush( COLOR_HIGHLIGHT );
      }
      else
      {
         bSelected = FALSE;
         cpBrush = GetSysBrush( COLOR_WINDOW );
      }
   
      if (! cpBrush)
      {
         /*DK* ##ERROR Could not create solid brush */
         AfxThrowNotSupportedException();
      }
   
      bRC = cdcCombo.Attach(lpDrawItemStruct->hDC);
   
      ASSERT(bRC != 0);
   
      if (bRC == 0)
      {
          AfxThrowNotSupportedException();
      }
   
      SetColorsInDC(lpDrawItemStruct->hDC, bSelected);
   
      cdcCombo.FillRect(&(lpDrawItemStruct->rcItem), cpBrush);
   
      // If this is a Printer or True Type font, draw the image/picture.
      if (bDrawPicture)
      {
          ix = lpDrawItemStruct->rcItem.left + 1;  // 0 is focus rect
          //center vertically
          iy = lpDrawItemStruct->rcItem.top +
              abs(((lpDrawItemStruct->rcItem.bottom -
          lpDrawItemStruct->rcItem.top) - iPictureHeight))/2;
   
          switch (pDesc->m_iFontType)
          {
             case DEVICE_FONTTYPE:
                m_cPicture_PRN.Picture(&cdcCombo, ix, iy);
                break;
   
             case TRUETYPE_FONTTYPE:
                m_cPicture_TT.Picture(&cdcCombo, ix, iy);
                break;
   
             default:
               break;
          }
      }
   
      // set the rectangle  for the text, and draw the text
      cRectText       =  lpDrawItemStruct->rcItem;
      cRectText.left += m_Max_cx_FontType_BMP + FONT_BMP_TXT_BORDER;
   
      cdcCombo.DrawText(*pcStringText, -1, &(cRectText), DT_LEFT | DT_SINGLELINE | DT_VCENTER);
   
      if ( (lpDrawItemStruct->itemState & ODS_FOCUS) ==  ODS_FOCUS)
      {
         cdcCombo.DrawFocusRect(&(lpDrawItemStruct->rcItem));
      }
   
   }
   CATCH(CNotSupportedException,e)
   {
   /*DK* ##ERROR Internal Thown Unsupported Exception */
   }
   END_CATCH
   
   cdcCombo.Detach();
}

/******************************************************************************/
// bInverted is the same as text selected.

void CTfontDlg::SetColorsInDC(HDC hdc, BOOL bInverted)
{
    DWORD dwFGColor;
    DWORD dwBKColor;

    if (bInverted)
        {
        dwFGColor = ::GetSysColor( COLOR_HIGHLIGHTTEXT );
        dwBKColor = ::GetSysColor( COLOR_HIGHLIGHT );
        }
    else
       {
       dwFGColor = ::GetSysColor( COLOR_WINDOWTEXT );
       dwBKColor = ::GetSysColor( COLOR_WINDOW );
       }

    ::SetTextColor( hdc, dwFGColor );

    ::SetBkMode ( hdc, OPAQUE );
    ::SetBkColor( hdc, dwBKColor );
}

