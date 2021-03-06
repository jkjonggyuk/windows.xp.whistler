#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>

#include "slm.h"
#include "sys.h"
#include "util.h"
#include "stfile.h"
#include "ad.h"
#include "script.h"
#include "proto.h"

EnableAssert

char rgchIn[]  = "(stdin)";
char rgchOut[] = "(stdout)";
char rgchErr[] = "(stderr)";
char rgchLog[] = "(log)";
MF mfStdin  = { (PTH *)rgchIn,  "", 0, fdNil, fxNil, fFalse, mmNil ,0L };
MF mfStdout = { (PTH *)rgchOut, "", fdNil, 1, fxNil, fFalse, mmNil ,0L };
MF mfStderr = { (PTH *)rgchErr, "", fdNil, 2, fxNil, fFalse, mmNil ,0L };
MF mfStdlog = { (PTH *)rgchLog, "", fdNil, fdNil, fxNil, fFalse, mmNil ,0L };
static AD *padLog;

/* four mf to allocate; pthReal==0 indicates availability; other fields
   always valid (i.e. so FreeMf and CloseMf do nothing).
*/
MF mf1 = { (PTH *)0, "", fdNil, fdNil, fxNil, fFalse, mmNil, -1L };
MF mf2 = { (PTH *)0, "", fdNil, fdNil, fxNil, fFalse, mmNil, -1L };
MF mf3 = { (PTH *)0, "", fdNil, fdNil, fxNil, fFalse, mmNil, -1L };
MF mf4 = { (PTH *)0, "", fdNil, fdNil, fxNil, fFalse, mmNil, -1L };


void
InitLogHandle(
    AD *pad,
    char *pszLogName)
{
    if (!stricmp(pszLogName, "nul"))
        return;

    mfStdlog.pthReal = malloc(strlen(pszLogName) + 1);
    if (mfStdlog.pthReal)
    {
        strcpy(mfStdlog.pthReal, pszLogName);
        mfStdlog.fdWrite = open(mfStdlog.pthReal, O_APPEND|O_WRONLY|O_CREAT,
                                S_IREAD|S_IWRITE);
    }

    if (mfStdlog.fdWrite == fdNil)
        FatalError("Unable to open log file (%s) for write access\n",
                   pszLogName );

    padLog = pad;
}


void
CloseLogHandle(
    void)
{
    if (mfStdlog.fdWrite != fdNil)
    {
        if (mfStdlog.pos != 0L)
        {
            if (padLog->flags & flagStScript)
                Warn("Backup script written to %s\n", mfStdlog.pthReal);
            else
                Warn("SSync log written to %s\n", mfStdlog.pthReal);
        }

        close(mfStdlog.fdWrite);
        mfStdlog.fdWrite = fdNil;
    }
}


void
AssertNoMf(
    void)
{
    AssertF(mf1.pthReal == 0);
    AssertF(mf2.pthReal == 0);
    AssertF(mf3.pthReal == 0);
    AssertF(mf4.pthReal == 0);
    AssertF(FEmptyNm(mf1.nmTemp));
    AssertF(FEmptyNm(mf2.nmTemp));
    AssertF(FEmptyNm(mf3.nmTemp));
    AssertF(FEmptyNm(mf4.nmTemp));
    AssertF(mf1.fdRead == fdNil);
    AssertF(mf2.fdRead == fdNil);
    AssertF(mf3.fdRead == fdNil);
    AssertF(mf4.fdRead == fdNil);
    AssertF(mf1.fdWrite == fdNil);
    AssertF(mf2.fdWrite == fdNil);
    AssertF(mf3.fdWrite == fdNil);
    AssertF(mf4.fdWrite == fdNil);
    AssertF(!mf1.fFileLock);
    AssertF(!mf2.fFileLock);
    AssertF(!mf3.fFileLock);
    AssertF(!mf4.fFileLock);
    AssertF(mf1.mm == mmNil);
    AssertF(mf2.mm == mmNil);
    AssertF(mf3.mm == mmNil);
    AssertF(mf4.mm == mmNil);
}


/* return fTrue if the mf is allocated and otherwise valid */
F
FIsValidMf(
    MF *pmf)
{
    if (pmf != &mfStdin &&
        pmf != &mfStdout &&
        pmf != &mfStderr &&
        pmf != &mfStdlog &&
        pmf != &mf1 &&
        pmf != &mf2 &&
        pmf != &mf3 &&
        pmf != &mf4)
            return fFalse;

    if (pmf->pthReal == 0)
            return fFalse;

    /* check for proper configuration */
    switch(pmf->mm)
    {
        default:
            AssertF(fFalse);

        case mmNil:
            break;

        case mmDelTemp:
        case mmInstall:
        case mmInstall1Ed:
        case mmRenTemp:
        case mmRenTempRO:
        case mmRenReal:
        case mmAppToReal:
        case mmCreate:
            if (FEmptyNm(pmf->nmTemp))
                return fFalse;
            break;

        case mmDelReal:
        case mmSetRO:
        case mmSetRW:
            if (!FEmptyNm(pmf->nmTemp))
                return fFalse;
        break;
    }

    if (pmf->fFileLock && pmf->fdWrite < 0)
        return fFalse;

    return fTrue;
}


/* return fTrue if the mf is valid and open */
F
FIsOpenMf(
    MF *pmf)
{
    return FIsValidMf(pmf) && (pmf->fdRead >= 0 || pmf->fdWrite >= 0);
}


/* return fTrue if the mf is valid and closed */
F
FIsClosedMf(
    MF *pmf)
{
    return FIsValidMf(pmf) && pmf->fdRead < 0 && pmf->fdWrite < 0;
}


void
AbortMf(
    void)
{
    if (mf1.pthReal != 0)
        CloseMf(&mf1);
    if (mf2.pthReal != 0)
        CloseMf(&mf2);
    if (mf3.pthReal != 0)
        CloseMf(&mf3);
    if (mf4.pthReal != 0)
        CloseMf(&mf4);
}


/* allocate new MF; retains pointer to pthReal!  Must free via FreeMf (or
   CloseMf which calls FreeMf).  mm set to mmNil.
*/
MF *
PmfAlloc(
    PTH pthReal[],
    char *szTemp,    /* 0 -> use base of pthReal */
    FX fx)
{
    register MF *pmf;

    AssertF(pthReal != 0);

    if (mf1.pthReal == 0)
        pmf = &mf1;
    else if (mf2.pthReal == 0)
        pmf = &mf2;
    else if (mf3.pthReal == 0)
        pmf = &mf3;
    else if (mf4.pthReal == 0)
        pmf = &mf4;
    else
        AssertF(fFalse);

    /* initialize contents which are not already initialized */
    pmf->fx = fx;
    pmf->pthReal = pthReal;
    if (szTemp != 0)
        NmCopySz(pmf->nmTemp, szTemp, cchFileMax);

    AssertF(FIsClosedMf(pmf));

    return pmf;
}


/* Records the mf (if not already) and makes the mf available for use; we
   ensure that the mf is back the initial state
*/
void
FreeMf(
    MF *pmf)
{
    static char *mpmmsz[] =
    {
        "nil\n",                /* mmNil */
        "delete %@T\n",         /* mmDelTemp */
        "clear %@R\n",          /* mmDelReal */
        "rename %@T %@R\n",     /* mmRenTemp */
        "install %@T %@R\n",    /* mmInstall */
        "renreal %@T %@R\n",    /* mmRenReal */
        "append %@T %@R\n",     /* mmAppToReal */
        "makero %@R\n",         /* mmSetRO */
        "makerw %@R\n",         /* mmSetRW */
        "rename %@T %@R\n",     /* mmRenTempRO ( + makero) */
        "link %@R %@N\n",       /* mmLinkReal */
        "create %@R\n",         /* mmCreate */
        "install1Ed %@T %@R\n"  /* mmInstall1Ed */
    };

    AssertF(FIsClosedMf(pmf));

    if (pmf->mm != mmNil)
    {
        AppendScript(pmf->fx, mpmmsz[pmf->mm], pmf, pmf);
        if (pmf->mm == mmRenTempRO)
            AppendScript(pmf->fx, mpmmsz[mmSetRO], pmf, pmf);
    }

    pmf->mm         = mmNil;
    pmf->fx         = fxNil;
    pmf->pthReal    = 0;
    *pmf->nmTemp    = '\0';
    pmf->fdRead     = fdNil;
    pmf->fdWrite    = fdNil;
    pmf->fFileLock  = fFalse;
}
