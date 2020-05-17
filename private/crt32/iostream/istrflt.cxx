/***
* istrflt.cxx - definitions for istream operator>>(float) member function
*
*	Copyright (c) 1991-1992, Microsoft Corporation.  All rights reserved.
*
*Purpose:
*	Definitions of operator>>(float) member function for istream class.
*	[AT&T C++]
*
*Revision History:
*	09-26-91   KRS	Created.  Split out from istream.cxx for granularity.
*	12-30-92   KRS	Fix indirection problem with **endptr.
*
*******************************************************************************/

#include <cruntime.h>
#include <internal.h>
#include <stdlib.h>
#include <float.h>
#include <iostream.h>
#pragma hdrstop

#pragma check_stack(on)		// large buffer(s)

// CONSIDER: validate this maximum length
#define MAXFLTSIZ	20

istream& istream::operator>>(float& n)
{
_WINSTATIC char ibuffer[MAXFLTSIZ];
    double d;
    char ** endptr = (char**)NULL;
    if (ipfx(0))
	{
	if (getdouble(ibuffer, MAXFLTSIZ)>0)
	    {
	    d = strtod(ibuffer, endptr);
#if 0
	    if (**endptr)
		{
		//UNDONE: put back any unread characters, if possible
		}
#endif
	    if (d > FLT_MAX)
		n = FLT_MAX;
	    else if (d < -FLT_MAX)
		n =  -FLT_MAX;
	    else if ((d>0) && (d< FLT_MIN))
		n = FLT_MIN;
	    else if ((d<0) && (d> -FLT_MIN))
		n = - FLT_MIN;
	    else
		n = (float) d;
	    }
        isfx();
        }
return *this;
}
