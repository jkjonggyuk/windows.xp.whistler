/***************************************************************************\
*
*  BTDRIVER.H
*
*  Copyright (C) Microsoft Corporation 2000.
*  All Rights reserved.
*
*****************************************************************************
*
*  Module Intent
*
*   Defines some constants for the btree test driver dialog box.
*
*****************************************************************************
*
*  Testing Notes
*
*****************************************************************************
*
*  Created 00-Ooo-0000 by JohnSc
*
*****************************************************************************
*
*  Released by Development:  00-Ooo-0000
*
*****************************************************************************
*
*  Current Owner:  JohnSc
*
\***************************************************************************/

/***************************************************************************\
*                                                                           *
*                               Defines                                     *
*                                                                           *
\***************************************************************************/

#define BT_EDIT1    4001
#define BT_EDIT2    4002
#define BT_EDIT3    4003
#define BT_CAPTION  4004
#define BT_CREATE   4005
#define BT_DESTROY  4006
#define BT_OPEN     4007
#define BT_CLOSE    4008
#define BT_INSERT   4009
#define BT_DELETE   4010
#define BT_UPDATE   4011
#define BT_LOOKUP   4012
#define BT_PREV     4013
#define BT_NEXT     4014
#define BT_TEXT1    4015
#define BT_TEXT2    4016
#define BT_TEXT3    4017

/***************************************************************************\
*                                                                           *
*                               Prototypes                                  *
*                                                                           *
\***************************************************************************/

DLGRET BTDriverDlg (HWND, unsigned, WORD, LONG );

/* EOF */
