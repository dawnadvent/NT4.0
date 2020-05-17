//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1992 - 1994.
//
//  File:       controls.hxx
//
//  Contents:   Public declarations for Disk Administrator custom controls
//
//  History:    7-Oct-94 BruceFo    Created
//
//--------------------------------------------------------------------------

#ifndef __CONTROLS_HXX__
#define __CONTROLS_HXX__

//
// Initialize all the controls
//

BOOL
UseWindiskControls(
    IN HINSTANCE hInstance
    );

BOOL
ReleaseWindiskControls(
    IN HINSTANCE hInstance
    );

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//
// Bitmap control
//
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#ifndef RC_INVOKED

#define BITMAP_CONTROL_STRING   TEXT("WinDiskBitmapControl")
#define BMPCTL_SETBITMAP        WM_USER

#endif // RC_INVOKED


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//
// Box control
//
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#ifndef RC_INVOKED

#define COLORBOX_CONTROL_STRING TEXT("WinDiskColorBoxControl")
#define BOXCTL_SETCOLOR         WM_USER

#endif // RC_INVOKED


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//
// Line control.
//
// This simply draws a line from its left to right side a single pixel high.
//
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#ifndef RC_INVOKED

#define WC_LINECONTROLCLASS TEXT("SmLineControl")

#endif // RC_INVOKED

#endif // __CONTROLS_HXX__
