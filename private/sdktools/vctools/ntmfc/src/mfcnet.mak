# This is a part of the Microsoft Foundation Classes C++ library.
# Copyright (C) 1992 Microsoft Corporation
# All rights reserved.
#
# This source code is only intended as a supplement to the
# Microsoft Foundation Classes Reference and Microsoft
# QuickHelp documentation provided with the library.
# See these sources for detailed information regarding the
# Microsoft Foundation Classes product.
#

# MFCN30[D].DLL is a DLL
#  which exports all the MFC/NET classes
#
# If you need a private build of the MFC NET DLL, be sure to rename
#  "MFCN30.DLL" to something more appropriate for you application.
# Please do not re-distribute a privately built version with the
#  name "MFCN30.DLL".
#
# Use nmake /f mfcnet.mak LIBNAME=<my name> to do this.
#
# Note: LIBNAME must be 6 characters or less.

!ifndef LIBNAME
!error LIBNAME is not defined. LIBNAME=MFCN30 builds the prebuilt DLL.
!endif

!if "$(NTSDK)" == "1"
CRTDLL=CRTDLL.LIB
!else
CRTDLL=MSVCRT.LIB
!endif

TARGET=w
DLL=2
TARG=$(LIBNAME)
TARGDEFS=/D_AFX_NET_IMPL
MFCDLL=MFC30
LFLAGS=/nodefaultlib

!if "$(UNICODE)" == "1"
TARG=$(TARG)U
MFCDLL=$(MFCDLL)U
!endif

!if "$(DEBUG)" != "0"
# Debug DLL build
RCDEFINES=/D_DEBUG
LFLAGS=$(LFLAGS)
PCH_FILE=$D\stdafxn.pch
PCH_CPP=dllnet
TARG=$(TARG)D
MFCDLL=$(MFCDLL)D

!ELSE
# Release DLL build
RCDEFINES=
LFLAGS=$(LFLAGS)
PCH_FILE=$D\stdafxn.pch
PCH_CPP=dllnet
!ENDIF

DEFFILE=$(PLATFORM)\$(TARG).DEF

MFCDLL=..\lib\$(MFCDLL).lib

!if "$(DEBUGTYPE)" == ""
DEBUGTYPE=cv
!endif

!if "$(CODEVIEW)" != "0"
LFLAGS=$(LFLAGS) /debug:full /debugtype:$(DEBUGTYPE)
!if "$(NO_PDB)" != "1" && "$(REGEN)" != "1"
LFLAGS=$(LFLAGS) /pdb:$(TARG).pdb
!else
LFLAGS=$(LFLAGS) /pdb:none
!endif
!else
LFLAGS=$(LFLAGS) /debug:none /incremental:no
!endif

!ifdef RELEASE # Release VERSION info
RCDEFINES=$(RCDEFINES) /DRELEASE
LFLAGS=$(LFLAGS) /release
!endif

LFLAGS=$(LFLAGS) /dll /subsystem:windows,3.10 /version:3.1 /base:0x5FB00000

# OPT:noref keeps unreferenced functions (ie. no dead-code elimination)
!if "$(REGEN)" == "1"
LFLAGS=$(LFLAGS) /opt:noref
!else
LFLAGS=$(LFLAGS) /opt:ref
!endif

LIBS=$(MFCDLL) $(CRTDLL) kernel32.lib gdi32.lib user32.lib shell32.lib \
	comdlg32.lib advapi32.lib wsock32.lib

!if "$(ORDER)" == "1"
!if exist($(PLATFORM)\$(TARG).prf)
DEFS=$(DEFS) /D_AFX_FUNCTION_ORDER
LFLAGS=$(LFLAGS) /order:@$(PLATFORM)\$(TARG).prf
!endif
!endif

dll_goal: create2.dir $(TARG).dll ..\lib\$(TARG).lib

#############################################################################
# import most rules and library files from normal makefile

!include makefile

create2.dir:
	@-if not exist $D\*.* mkdir $D

#############################################################################
# Build target

$D\$(TARG).res: mfcnet.rc
	rc /r $(RCDEFINES) /fo $D\$(TARG).res mfcnet.rc

DLL_OBJS=$(SOCKETS)

$(TARG).dll ..\lib\$(TARG).lib: $D\$(PCH_CPP).obj $(DLL_OBJS) $(DEFFILE) $D\$(TARG).res
	link @<<
$(LFLAGS)
$(LIBS)
$D\$(PCH_CPP).obj
$(DLL_OBJS)
$D\$(TARG).res
/def:$(DEFFILE)
/out:$(TARG).DLL
/map:$D\$(TARG).MAP
/implib:..\lib\$(TARG).LIB
<<
	if exist ..\lib\$(TARG).exp del ..\lib\$(TARG).exp

#############################################################################
