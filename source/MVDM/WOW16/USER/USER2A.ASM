;
;   USER2A.ASM
;   More Win16 USER thunks
;
;   History:
;
;   Created 25-Jan-2000 by Jeff Parsons (jeffpar)
;   Added Win 31 thunks 22nd-March-2000 by Chandan S. Chauhan (ChandanC)
;   Split off from USER.ASM 9-Jun-92 by BobDay
;
;--

    TITLE   USER2A.ASM
    PAGE    ,132

    .286p

;    .xlist
    include wow.inc
    include wowusr.inc
    include cmacros.inc
NOEXTERNS=1         ; to suppress including most of the crap in user.inc
    include user.inc

    .list

externFP    GetModuleHandle

createSeg   _TEXT,CODE,WORD,PUBLIC,CODE
createSeg   _DATA,DATA,WORD,PUBLIC,DATA,DGROUP
defgrp      DGROUP,DATA


sBegin  CODE
assumes CS,CODE
assumes DS,DATA
assumes ES,NOTHING

    UserThunk   GETUPDATERECT
    UserThunk   GETUPDATERGN
    UserThunk   GETWC2
    UserThunk   GETWINDOWDC
    DUserThunk  GETWINDOWLONG
    UserThunk   GETWINDOWTASK
    UserThunk   GETWINDOWTEXT
    UserThunk   GETWINDOWTEXTLENGTH
    DUserThunk  GETWINDOWWORD
    DUserThunk  GLOBALADDATOM
    DUserThunk  GLOBALDELETEATOM
    UserThunk   GLOBALFINDATOM
    DUserThunk  GLOBALGETATOMNAME
    UserThunk   GRAYSTRING
    UserThunk   HIDECARET
    UserThunk   HILITEMENUITEM
    DUserThunk  ICONSIZE            ;;;;;;
;;;    UserThunk   INFLATERECT	    ; LOCALAPI in winrect.asm
;;; DUserThunk  INITAPP           ;LOCALAPI in user.asm
    DUserThunk  INSENDMESSAGE,0
    UserThunk   INSERTMENU
;;;    UserThunk   INTERSECTRECT  ; LOCALAPI in winrect.asm
    UserThunk   INVALIDATERECT
    UserThunk   INVALIDATERGN
    UserThunk   INVERTRECT
;;; UserThunk   ISCHARALPHA         ;LOCALAPI in winlang.asm
;;; UserThunk   ISCHARALPHANUMERIC      ;LOCALAPI in winlang.asm
;;; UserThunk   ISCHARLOWER         ;LOCALAPI in winlang.asm
;;; UserThunk   ISCHARUPPER         ;LOCALAPI in winlang.asm
    DUserThunk  ISCLIPBOARDFORMATAVAILABLE
    UserThunk   ISDIALOGMESSAGE
    UserThunk   ISDLGBUTTONCHECKED
;;;    UserThunk   ISRECTEMPTY	       ; LOCALAPI in winrect.asm
    DUserThunk  ISTWOBYTECHARPREFIX        ;;;;;;
    DUserThunk  ISUSERIDLE
    DUserThunk  KILLSYSTEMTIMER        ;;;;;
    UserThunk   KILLTIMER
    UserThunk   KILLTIMER2
    UserThunk   LBOXCARETBLINKER
;;; UserThunk   LBOXCTLWNDPROC          ;LOCALAPI in wsubcls.c
;;; UserThunk   LOADACCELERATORS        ; localapi in rmload.c

FUN_WOWLOADBITMAP EQU FUN_LOADBITMAP
    DUserThunk	WOWLOADBITMAP, %(size LOADBITMAP16)

FUN_WOWLOADCURSORICON EQU FUN_LOADCURSOR
    DUserThunk	WOWLOADCURSORICON, %(size LOADCURSOR16)

    DUserThunk  LOADCURSORICONHANDLER
    DUserThunk  LOADDIBCURSORHANDLER
    DUserThunk  LOADDIBICONHANDLER

;FUN_WOWLOADICON EQU FUN_LOADICON
;   DUserThunk	WOWLOADICON, %(size LOADICON16)

    DUserThunk  LOADICONHANDLER

FUN_WOWLOADMENU EQU FUN_LOADMENU
    DUserThunk	WOWLOADMENU, %(size LOADMENU16)

    UserThunk   LOADMENUINDIRECT
;;; UserThunk   LOADSTRING          ;LOCALAPI in rmload.c
    DUserThunk  LOCKMYTASK
    DUserThunk  LOOKUPMENUHANDLE

    ; Hack to use original IDs
    FUN_WIN32LSTRCMP equ FUN_LSTRCMP
    FUN_WIN32LSTRCMPI equ FUN_LSTRCMPI

    DUserThunk  WIN32LSTRCMP, %(size LSTRCMP16)
    DUserThunk  WIN32LSTRCMPI, %(size LSTRCMPI16)

sEnd    CODE

end
