/*******************************************************************
** f i c l . c
** Forth Inspired Command Language - external interface
** Author: John Sadler (john_sadler@alum.mit.edu)
** Created: 19 July 1997
** 
*******************************************************************/
/*
** This is an ANS Forth interpreter written in C.
** Ficl uses Forth syntax for its commands, but turns the Forth 
** model on its head in other respects.
** Ficl provides facilities for interoperating
** with programs written in C: C functions can be exported to Ficl,
** and Ficl commands can be executed via a C calling interface. The
** interpreter is re-entrant, so it can be used in multiple instances
** in a multitasking system. Unlike Forth, Ficl's outer interpreter
** expects a text block as input, and returns to the caller after each
** text block, so the data pump is somewhere in external code. This
** is more like TCL than Forth.
**
** Code is written in ANSI C for portability. 
*/

#ifdef TESTMAIN
#include <stdlib.h>
#else
#include <stand.h>
#endif
#include <string.h>
#include "ficl.h"

#ifdef FICL_TRACE
int ficl_trace = 0;
#endif


/*
** Local prototypes
*/


/*
** System statics
** The system builds a global dictionary during its start
** sequence. This is shared by all interpreter instances.
** Therefore only one instance can update the dictionary
** at a time. The system imports a locking function that
** you can override in order to control update access to
** the dictionary. The function is stubbed out by default,
** but you can insert one: #define FICL_MULTITHREAD 1
** and supply your own version of ficlLockDictionary.
*/
static FICL_DICT *dp     = NULL;
static FICL_DICT *envp   = NULL;
#if FICL_WANT_LOCALS
static FICL_DICT *localp = NULL;
#endif
static FICL_VM   *vmList = NULL;

static int defaultStack = FICL_DEFAULT_STACK;
static int defaultDict  = FICL_DEFAULT_DICT;


/**************************************************************************
                        f i c l I n i t S y s t e m
** Binds a global dictionary to the interpreter system. 
** You specify the address and size of the allocated area.
** After that, ficl manages it.
** First step is to set up the static pointers to the area.
** Then write the "precompiled" portion of the dictionary in.
** The dictionary needs to be at least large enough to hold the
** precompiled part. Try 1K cells minimum. Use "words" to find
** out how much of the dictionary is used at any time.
**************************************************************************/
void ficlInitSystem(int nDictCells)
{
    if (dp)
        dictDelete(dp);

    if (envp)
        dictDelete(envp);

#if FICL_WANT_LOCALS
    if (localp)
        dictDelete(localp);
#endif

    if (nDictCells <= 0)
        nDictCells = defaultDict;

    dp     = dictCreateHashed((unsigned)nDictCells, HASHSIZE);
    envp   = dictCreate(      (unsigned)FICL_DEFAULT_ENV);
#if FICL_WANT_LOCALS
    /*
    ** The locals dictionary is only searched while compiling,
    ** but this is where speed is most important. On the other
    ** hand, the dictionary gets emptied after each use of locals
    ** The need to balance search speed with the cost of the empty
    ** operation led me to select a single-threaded list...
    */
    localp = dictCreate(      (unsigned)FICL_MAX_LOCALS * CELLS_PER_WORD);
#endif

    ficlCompileCore(dp);

    return;
}


/**************************************************************************
                        f i c l N e w V M
** Create a new virtual machine and link it into the system list
** of VMs for later cleanup by ficlTermSystem. If this is the first
** VM to be created, use it to compile the words in softcore.c
**************************************************************************/
FICL_VM *ficlNewVM(void)
{
    FICL_VM *pVM = vmCreate(NULL, defaultStack, defaultStack);
    pVM->link = vmList;

    /*
    ** Borrow the first vm to build the soft words in softcore.c
    */
    if (vmList == NULL)
        ficlCompileSoftCore(pVM);

    vmList = pVM;
    return pVM;
}


/**************************************************************************
                        f i c l B u i l d
** Builds a word into the dictionary.
** Preconditions: system must be initialized, and there must
** be enough space for the new word's header! Operation is
** controlled by ficlLockDictionary, so any initialization
** required by your version of the function (if you overrode
** it) must be complete at this point.
** Parameters:
** name  -- duh, the name of the word
** code  -- code to execute when the word is invoked - must take a single param
**          pointer to a FICL_VM
** flags -- 0 or more of F_IMMEDIATE, F_COMPILE, use bitwise OR!
** 
**************************************************************************/
int ficlBuild(char *name, FICL_CODE code, char flags)
{
	int err = ficlLockDictionary(TRUE);
	if (err) return err;

    dictAppendWord(dp, name, code, flags);

	ficlLockDictionary(FALSE);
	return 0;
}


/**************************************************************************
                        f i c l E x e c
** Evaluates a block of input text in the context of the
** specified interpreter. Emits any requested output to the
** interpreter's output function.
**
** Contains the "inner interpreter" code in a tight loop
**
** Returns one of the VM_XXXX codes defined in ficl.h:
** VM_OUTOFTEXT is the normal exit condition
** VM_ERREXIT means that the interp encountered a syntax error
**      and the vm has been reset to recover (some or all
**      of the text block got ignored
** VM_USEREXIT means that the user executed the "bye" command
**      to shut down the interpreter. This would be a good
**      time to delete the vm, etc -- or you can ignore this
**      signal.
**************************************************************************/
int ficlExec(FICL_VM *pVM, char *pText, INT32 size)
{
    int        except;
    FICL_WORD *tempFW;
    jmp_buf    vmState;
    TIB        saveTib;
    FICL_VM         VM;
    FICL_STACK      rStack;

    assert(pVM);

    vmPushTib(pVM, pText, size, &saveTib);

    /*
    ** Save and restore pVM and pVM->rStack to enable nested calls to ficlExec 
    */
    memcpy((void*)&VM, (void*)pVM, sizeof(FICL_VM));
    memcpy((void*)&rStack, (void*)pVM->rStack, sizeof(FICL_STACK));

    pVM->pState = &vmState; /* This has to come before the setjmp! */
    except = setjmp(vmState);

    switch (except)
    {
    case 0:
        if (pVM->fRestart)
        {
            pVM->fRestart = 0;
            pVM->runningWord->code(pVM);
        }

        /*
        ** the mysterious inner interpreter...
        ** vmThrow gets you out of this loop with a longjmp()
        */
        for (;;)
        {
#ifdef FICL_TRACE
	    CELL c;
            char buffer[40];
#endif
            tempFW = *pVM->ip++;
#ifdef FICL_TRACE
            if (ficl_trace && isAFiclWord(tempFW))
            {
	        extern void literalParen(FICL_VM*);
	        extern void stringLit(FICL_VM*);
	        extern void ifParen(FICL_VM*);
	        extern void branchParen(FICL_VM*);
	        extern void qDoParen(FICL_VM*);
	        extern void doParen(FICL_VM*);
	        extern void loopParen(FICL_VM*);
	        extern void plusLoopParen(FICL_VM*);
    
                if      (tempFW->code == literalParen)
                {
               	    c = *(pVM->ip);
               	    if (isAFiclWord(c.p))
               	    {
                   	    FICL_WORD *pLit = (FICL_WORD *)c.p;
                   	    sprintf(buffer, "    literal %.*s (%#lx)",
                       	    pLit->nName, pLit->name, c.u);
               	    }
               	    else
                   	    sprintf(buffer, "    literal %ld (%#lx)", c.i, c.u);
                }
                else if (tempFW->code == stringLit)
                {
               	    FICL_STRING *sp = (FICL_STRING *)(void *)pVM->ip;
               	    sprintf(buffer, "    s\" %.*s\"", sp->count, sp->text);
                }
                else if (tempFW->code == ifParen)
                {
               	    c = *pVM->ip;
               	    if (c.i > 0)
                   	    sprintf(buffer, "    if / while (branch rel %ld)", c.i);
               	    else
                   	    sprintf(buffer, "    until (branch rel %ld)", c.i);
                }
                else if (tempFW->code == branchParen)
                {
               	    c = *pVM->ip;
               	    if (c.i > 0)
                   	    sprintf(buffer, "    else (branch rel %ld)", c.i);
               	    else
                   	    sprintf(buffer, "    repeat (branch rel %ld)", c.i);
                }
                else if (tempFW->code == qDoParen)
                {
               	    c = *pVM->ip;
               	    sprintf(buffer, "    ?do (leave abs %#lx)", c.u);
                }
                else if (tempFW->code == doParen)
                {
               	    c = *pVM->ip;
               	    sprintf(buffer, "    do (leave abs %#lx)", c.u);
                }
                else if (tempFW->code == loopParen)
                {
               	    c = *pVM->ip;
               	    sprintf(buffer, "    loop (branch rel %#ld)", c.i);
                }
                else if (tempFW->code == plusLoopParen)
                {
               	    c = *pVM->ip;
               	    sprintf(buffer, "    +loop (branch rel %#ld)", c.i);
                }
                else /* default: print word's name */
                {
               	    sprintf(buffer, "    %.*s", tempFW->nName, tempFW->name);
                }
    
                vmTextOut(pVM, buffer, 1);
            }
            else if (ficl_trace) /* probably not a word 
				  * - punt and print value
				  */
            {
           	    sprintf(buffer, "    %ld (%#lx)", ((CELL*)pVM->ip)->i, ((CELL*)pVM->ip)->u);
           	    vmTextOut(pVM, buffer, 1);
            }
#endif FICL_TRACE
            /*
            ** inline code for
            ** vmExecute(pVM, tempFW);
            */
            pVM->runningWord = tempFW;
            tempFW->code(pVM);
        }

        break;

    case VM_RESTART:
        pVM->fRestart = 1;
        except = VM_OUTOFTEXT;
        break;

    case VM_OUTOFTEXT:
#ifdef TESTMAIN
        if ((pVM->state != COMPILE) && (pVM->sourceID.i == 0))
            ficlTextOut(pVM, FICL_PROMPT, 0);
#endif
        break;

    case VM_USEREXIT:
        break;

    case VM_QUIT:
        if (pVM->state == COMPILE)
            dictAbortDefinition(dp);

        memcpy((void*)pVM, (void*)&VM, sizeof(FICL_VM));
        memcpy((void*)pVM->rStack, (void*)&rStack, sizeof(FICL_STACK));
        break;

    case VM_ERREXIT:
    case VM_ABORT:
    case VM_ABORTQ:
    default:    /* user defined exit code?? */
        if (pVM->state == COMPILE)
        {
            dictAbortDefinition(dp);
#if FICL_WANT_LOCALS
            dictEmpty(localp, localp->pForthWords->size);
#endif
        }
        dictResetSearchOrder(dp);
        memcpy((void*)pVM, (void*)&VM, sizeof(FICL_VM));
        memcpy((void*)pVM->rStack, (void*)&rStack, sizeof(FICL_STACK));
	stackReset(pVM->pStack);
	pVM->base = 10;
        break;
   }

    pVM->pState = VM.pState;
    vmPopTib(pVM, &saveTib);
    return (except);
}

/**************************************************************************
                        f i c l E x e c F D
** reads in text from file fd and passes it to ficlExec()
 * returns VM_OUTOFTEXT on success or the ficlExec() error code on
 * failure.
 */ 
#define nLINEBUF 256
int ficlExecFD(FICL_VM *pVM, int fd)
{
    char    cp[nLINEBUF];
    int     i, nLine = 0, rval = VM_OUTOFTEXT;
    char    ch;
    CELL    id;

    id = pVM->sourceID;
    pVM->sourceID.i = fd;

    /* feed each line to ficlExec */
    while (1) {
	int status, i;

	i = 0;
	while ((status = read(fd, &ch, 1)) > 0 && ch != '\n')
	    cp[i++] = ch;
        nLine++;
	if (!i) {
	    if (status < 1)
		break;
	    continue;
	}
        rval = ficlExec(pVM, cp, i);
	if(rval != VM_QUIT && rval != VM_USEREXIT && rval != VM_OUTOFTEXT)
        {
            pVM->sourceID = id;
            return rval; 
        }
    }
    /*
    ** Pass an empty line with SOURCE-ID == 0 to flush
    ** any pending REFILLs (as required by FILE wordset)
    */
    pVM->sourceID.i = -1;
    ficlExec(pVM, "", 0);

    pVM->sourceID = id;
    return rval;
}

/**************************************************************************
                        f i c l L o o k u p
** Look in the system dictionary for a match to the given name. If
** found, return the address of the corresponding FICL_WORD. Otherwise
** return NULL.
**************************************************************************/
FICL_WORD *ficlLookup(char *name)
{
    STRINGINFO si;
    SI_PSZ(si, name);
    return dictLookup(dp, si);
}


/**************************************************************************
                        f i c l G e t D i c t
** Returns the address of the system dictionary
**************************************************************************/
FICL_DICT *ficlGetDict(void)
{
    return dp;
}


/**************************************************************************
                        f i c l G e t E n v
** Returns the address of the system environment space
**************************************************************************/
FICL_DICT *ficlGetEnv(void)
{
    return envp;
}


/**************************************************************************
                        f i c l S e t E n v
** Create an environment variable with a one-CELL payload. ficlSetEnvD
** makes one with a two-CELL payload.
**************************************************************************/
void ficlSetEnv(char *name, UNS32 value)
{
    STRINGINFO si;
    FICL_WORD *pFW;

    SI_PSZ(si, name);
    pFW = dictLookup(envp, si);

    if (pFW == NULL)
    {
        dictAppendWord(envp, name, constantParen, FW_DEFAULT);
        dictAppendCell(envp, LVALUEtoCELL(value));
    }
    else
    {
        pFW->param[0] = LVALUEtoCELL(value);
    }

    return;
}

void ficlSetEnvD(char *name, UNS32 hi, UNS32 lo)
{
    FICL_WORD *pFW;
    STRINGINFO si;
    SI_PSZ(si, name);
    pFW = dictLookup(envp, si);

    if (pFW == NULL)
    {
        dictAppendWord(envp, name, twoConstParen, FW_DEFAULT);
        dictAppendCell(envp, LVALUEtoCELL(lo));
        dictAppendCell(envp, LVALUEtoCELL(hi));
    }
    else
    {
        pFW->param[0] = LVALUEtoCELL(lo);
        pFW->param[1] = LVALUEtoCELL(hi);
    }

    return;
}


/**************************************************************************
                        f i c l G e t L o c
** Returns the address of the system locals dictionary. This dict is
** only used during compilation, and is shared by all VMs.
**************************************************************************/
#if FICL_WANT_LOCALS
FICL_DICT *ficlGetLoc(void)
{
    return localp;
}
#endif


/**************************************************************************
                        f i c l T e r m S y s t e m
** Tear the system down by deleting the dictionaries and all VMs.
** This saves you from having to keep track of all that stuff.
**************************************************************************/
void ficlTermSystem(void)
{
    if (dp)
        dictDelete(dp);
    dp = NULL;

    if (envp)
        dictDelete(envp);
    envp = NULL;

#if FICL_WANT_LOCALS
    if (localp)
        dictDelete(localp);
    localp = NULL;
#endif

    while (vmList != NULL)
    {
        FICL_VM *pVM = vmList;
        vmList = vmList->link;
        vmDelete(pVM);
    }

    return;
}
