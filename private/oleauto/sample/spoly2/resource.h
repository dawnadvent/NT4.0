/*** 
*resource.h
*
*  Copyright (C) 1992-1994, Microsoft Corporation.  All Rights Reserved.
*
*Purpose:
*
*Implementation Notes:
*
*****************************************************************************/

#ifdef _MAC

#define kMinSize	500	/* minimum size (in K) */
#define kPrefSize	500	/* preferred size (in K) */

#define	rMenuBar	128	/* menu bar */
#define	rAboutAlert	128	/* about alert */
#define	rUserAlert	129	/* error alert */
#define	rWindow		128	/* application's window */

#define	mApple		128	/* Apple menu */
#define	iAbout		1

#define	mFile		129	/* File menu */
#define	iNew		1
#define	iClose		4
#define	iQuit		12

#define	mEdit		130	/* Edit menu */
#define	iUndo		1
#define	iCut		3
#define	iCopy		4
#define	iPaste		5
#define	iClear		6

#define mSpoly		131
#define iTest		1

#define kMinHeap	21 * 1024
#define kMinSpace	8 * 1024

#else /* WIN16 || WIN32 */

# define IDM_CLEAR	1
# define IDM_DUMP	2
# define IDM_FIRSTCHILD	100

#endif

