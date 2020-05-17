/***
*winput.c - wscanf style input from a FILE (wchar_t version)
*
*	Copyright (c) 1991-1992, Microsoft Corporation. All rights reserved.
*
*Purpose:
*	This file contains the code that does all the work for the
*	wscanf family of functions.  It should not be called directly, only
*	by the *wscanf functions.  We don't make any assumtions about the
*	sizes of ints, longs, shorts, or long doubles, but if types do overlap,
*	we also try to be efficient.
*	**** DOESN'T CURRENTLY DO MTHREAD LOCKING ****
*
*Revision History:
*	04-27-93  CFW 	Module created.
*
*******************************************************************************/

#define WPRFLAG 1

#ifndef _UNICODE   /* CRT flag */
#define _UNICODE 1
#endif

#ifndef UNICODE	   /* NT flag */
#define UNICODE 1
#endif

#include "input.c"
