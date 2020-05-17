/*++

Copyright (c) 1989  Microsoft Corporation

Module Name:

    tz.h

Abstract:

    This is the main include file for the Windows TZ (time zone utility)

Author:

    Steve Wood (stevewo) 16-May-1989

Revision History:

--*/

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>

#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <malloc.h>
#include <sys\types.h>
#include <sys\stat.h>

#include <tools.h>
