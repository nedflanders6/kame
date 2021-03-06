/*******************************************************************
** d i c t . c
** Forth Inspired Command Language - dictionary methods
** Author: John Sadler (john_sadler@alum.mit.edu)
** Created: 19 July 1997
** 
*******************************************************************/
/*
** This file implements the dictionary -- FICL's model of 
** memory management. All FICL words are stored in the
** dictionary. A word is a named chunk of data with its
** associated code. FICL treats all words the same, even
** precompiled ones, so your words become first-class
** extensions of the language. You can even define new 
** control structures.
**
** 29 jun 1998 (sadler) added variable sized hash table support
*/

/* $FreeBSD: src/sys/boot/ficl/dict.c,v 1.6 1999/11/27 18:31:57 dcs Exp $ */

#ifdef TESTMAIN
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#else
#include <stand.h>
#endif
#include <string.h>
#include "ficl.h"

static char *dictCopyName(FICL_DICT *pDict, STRINGINFO si);

/**************************************************************************
                        d i c t A b o r t D e f i n i t i o n
** Abort a definition in process: reclaim its memory and unlink it
** from the dictionary list. Assumes that there is a smudged 
** definition in process...otherwise does nothing.
** NOTE: this function is not smart enough to unlink a word that
** has been successfully defined (ie linked into a hash). It
** only works for defs in process. If the def has been unsmudged,
** nothing happens.
**************************************************************************/
void dictAbortDefinition(FICL_DICT *pDict)
{
    FICL_WORD *pFW;
    ficlLockDictionary(TRUE);
    pFW = pDict->smudge;

    if (pFW->flags & FW_SMUDGE)
        pDict->here = (CELL *)pFW->name;

    ficlLockDictionary(FALSE);
    return;
}


/**************************************************************************
                        a l i g n P t r
** Aligns the given pointer to FICL_ALIGN address units.
** Returns the aligned pointer value.
**************************************************************************/
void *alignPtr(void *ptr)
{
#if FICL_ALIGN > 0
    char *cp;
    CELL c;
    cp = (char *)ptr + FICL_ALIGN_ADD;
    c.p = (void *)cp;
    c.u = c.u & (~FICL_ALIGN_ADD);
    ptr = (CELL *)c.p;
#endif
    return ptr;
}


/**************************************************************************
                        d i c t A l i g n
** Align the dictionary's free space pointer
**************************************************************************/
void dictAlign(FICL_DICT *pDict)
{
    pDict->here = alignPtr(pDict->here);
}


/**************************************************************************
                        d i c t A l l o t
** Allocate or remove n chars of dictionary space, with
** checks for underrun and overrun
**************************************************************************/
int dictAllot(FICL_DICT *pDict, int n)
{
    char *cp = (char *)pDict->here;
#if FICL_ROBUST
    if (n > 0)
    {
        if ((unsigned)n <= dictCellsAvail(pDict) * sizeof (CELL))
            cp += n;
        else
            return 1;       /* dict is full */
    }
    else
    {
        n = -n;
        if ((unsigned)n <= dictCellsUsed(pDict) * sizeof (CELL))
            cp -= n;
        else                /* prevent underflow */
            cp -= dictCellsUsed(pDict) * sizeof (CELL);
    }
#else
    cp += n;
#endif
    pDict->here = PTRtoCELL cp;
    return 0;
}


/**************************************************************************
                        d i c t A l l o t C e l l s
** Reserve space for the requested number of cells in the
** dictionary. If nCells < 0 , removes space from the dictionary.
**************************************************************************/
int dictAllotCells(FICL_DICT *pDict, int nCells)
{
#if FICL_ROBUST
    if (nCells > 0)
    {
        if (nCells <= dictCellsAvail(pDict))
            pDict->here += nCells;
        else
            return 1;       /* dict is full */
    }
    else
    {
        nCells = -nCells;
        if (nCells <= dictCellsUsed(pDict))
            pDict->here -= nCells;
        else                /* prevent underflow */
            pDict->here -= dictCellsUsed(pDict);
    }
#else
    pDict->here += nCells;
#endif
    return 0;
}


/**************************************************************************
                        d i c t A p p e n d C e l l
** Append the specified cell to the dictionary
**************************************************************************/
void dictAppendCell(FICL_DICT *pDict, CELL c)
{
    *pDict->here++ = c;
    return;
}


/**************************************************************************
                        d i c t A p p e n d C h a r
** Append the specified char to the dictionary
**************************************************************************/
void dictAppendChar(FICL_DICT *pDict, char c)
{
    char *cp = (char *)pDict->here;
    *cp++ = c;
    pDict->here = PTRtoCELL cp;
    return;
}


/**************************************************************************
                        d i c t A p p e n d W o r d
** Create a new word in the dictionary with the specified
** name, code, and flags. Name must be NULL-terminated.
**************************************************************************/
FICL_WORD *dictAppendWord(FICL_DICT *pDict, 
                          char *name, 
                          FICL_CODE pCode, 
                          UNS8 flags)
{
    STRINGINFO si;
    SI_SETLEN(si, strlen(name));
    SI_SETPTR(si, name);
    return dictAppendWord2(pDict, si, pCode, flags);
}


/**************************************************************************
                        d i c t A p p e n d W o r d 2
** Create a new word in the dictionary with the specified
** STRINGINFO, code, and flags. Does not require a NULL-terminated
** name.
**************************************************************************/
FICL_WORD *dictAppendWord2(FICL_DICT *pDict, 
                           STRINGINFO si, 
                           FICL_CODE pCode, 
                           UNS8 flags)
{
    FICL_COUNT len  = (FICL_COUNT)SI_COUNT(si);
    char *pName;
    FICL_WORD *pFW;

    ficlLockDictionary(TRUE);

    /*
    ** NOTE: dictCopyName advances "here" as a side-effect.
    ** It must execute before pFW is initialized.
    */
    pName         = dictCopyName(pDict, si);
    pFW           = (FICL_WORD *)pDict->here;
    pDict->smudge = pFW;
    pFW->hash     = hashHashCode(si);
    pFW->code     = pCode;
    pFW->flags    = (UNS8)(flags | FW_SMUDGE);
    pFW->nName    = (char)len;
    pFW->name     = pName;
    /*
    ** Point "here" to first cell of new word's param area...
    */
    pDict->here   = pFW->param;

    if (!(flags & FW_SMUDGE))
        dictUnsmudge(pDict);

    ficlLockDictionary(FALSE);
    return pFW;
}


/**************************************************************************
                        d i c t A p p e n d U N S 3 2
** Append the specified UNS32 to the dictionary
**************************************************************************/
void dictAppendUNS(FICL_DICT *pDict, UNS32 u)
{
    *pDict->here++ = LVALUEtoCELL(u);
    return;
}


/**************************************************************************
                        d i c t C e l l s A v a i l
** Returns the number of empty cells left in the dictionary
**************************************************************************/
int dictCellsAvail(FICL_DICT *pDict)
{
    return pDict->size - dictCellsUsed(pDict);
}


/**************************************************************************
                        d i c t C e l l s U s e d
** Returns the number of cells consumed in the dicionary
**************************************************************************/
int dictCellsUsed(FICL_DICT *pDict)
{
    return pDict->here - pDict->dict;
}


/**************************************************************************
                        d i c t C h e c k
** Checks the dictionary for corruption and throws appropriate
** errors
**************************************************************************/
void dictCheck(FICL_DICT *pDict, FICL_VM *pVM, int nCells)
{
    if ((nCells >= 0) && (dictCellsAvail(pDict) < nCells))
    {
        vmThrowErr(pVM, "Error: dictionary full");
    }

    if ((nCells <= 0) && (dictCellsUsed(pDict) < -nCells))
    {
        vmThrowErr(pVM, "Error: dictionary underflow");
    }

    if (pDict->nLists > FICL_DEFAULT_VOCS)
    {
        dictResetSearchOrder(pDict);
        vmThrowErr(pVM, "Error: search order overflow");
    }
    else if (pDict->nLists < 0)
    {
        dictResetSearchOrder(pDict);
        vmThrowErr(pVM, "Error: search order underflow");
    }

    return;
}


/**************************************************************************
                        d i c t C o p y N a m e
** Copy up to nFICLNAME characters of the name specified by si into
** the dictionary starting at "here", then NULL-terminate the name,
** point "here" to the next available byte, and return the address of
** the beginning of the name. Used by dictAppendWord.
** N O T E S :
** 1. "here" is guaranteed to be aligned after this operation.
** 2. If the string has zero length, align and return "here"
**************************************************************************/
static char *dictCopyName(FICL_DICT *pDict, STRINGINFO si)
{
    char *oldCP    = (char *)pDict->here;
    char *cp       = oldCP;
    char *name     = SI_PTR(si);
    int   i        = SI_COUNT(si);

    if (i == 0)
    {
        dictAlign(pDict);
        return (char *)pDict->here;
    }

    if (i > nFICLNAME)
        i = nFICLNAME;
    
    for (; i > 0; --i)
    {
        *cp++ = *name++;
    }

    *cp++ = '\0';

    pDict->here = PTRtoCELL cp;
    dictAlign(pDict);
    return oldCP;
}


/**************************************************************************
                        d i c t C r e a t e
** Create and initialize a dictionary with the specified number
** of cells capacity, and no hashing (hash size == 1).
**************************************************************************/
FICL_DICT  *dictCreate(unsigned nCells)
{
    return dictCreateHashed(nCells, 1);
}


FICL_DICT  *dictCreateHashed(unsigned nCells, unsigned nHash)
{
    FICL_DICT *pDict;
    size_t nAlloc;

    nAlloc =  sizeof (FICL_DICT) + nCells      * sizeof (CELL)
            + sizeof (FICL_HASH) + (nHash - 1) * sizeof (FICL_WORD *);

    pDict = ficlMalloc(nAlloc);
    assert(pDict);
    memset(pDict, 0, sizeof (FICL_DICT));
    pDict->size = nCells;
    dictEmpty(pDict, nHash);
    return pDict;
}


/**************************************************************************
                        d i c t D e l e t e 
** Free all memory allocated for the given dictionary 
**************************************************************************/
void dictDelete(FICL_DICT *pDict)
{
    assert(pDict);
    ficlFree(pDict);
    return;
}


/**************************************************************************
                        d i c t E m p t y
** Empty the dictionary, reset its hash table, and reset its search order.
** Clears and (re-)creates the main hash table (pForthWords) with the
** size specified by nHash.
**************************************************************************/
void dictEmpty(FICL_DICT *pDict, unsigned nHash)
{
    FICL_HASH *pHash;

    pDict->here = pDict->dict;

    dictAlign(pDict);
    pHash = (FICL_HASH *)pDict->here;
    dictAllot(pDict, 
              sizeof (FICL_HASH) + (nHash - 1) * sizeof (FICL_WORD *));

    pHash->size = nHash;
    hashReset(pHash);

    pDict->pForthWords = pHash;
    pDict->smudge = NULL;
    dictResetSearchOrder(pDict);
    return;
}


/**************************************************************************
                        d i c t I n c l u d e s
** Returns TRUE iff the given pointer is within the address range of 
** the dictionary.
**************************************************************************/
int dictIncludes(FICL_DICT *pDict, void *p)
{
    return ((p >= (void *) &pDict->dict)
        &&  (p <  (void *)(&pDict->dict + pDict->size)) 
           );
}


/**************************************************************************
                        d i c t L o o k u p
** Find the FICL_WORD that matches the given name and length.
** If found, returns the word's address. Otherwise returns NULL.
** Uses the search order list to search multiple wordlists.
**************************************************************************/
FICL_WORD *dictLookup(FICL_DICT *pDict, STRINGINFO si)
{
    FICL_WORD *pFW = NULL;
    FICL_HASH *pHash;
    int i;
    UNS16 hashCode   = hashHashCode(si);

    assert(pDict);

    ficlLockDictionary(1);

    for (i = (int)pDict->nLists - 1; (i >= 0) && (!pFW); --i)
    {
        pHash = pDict->pSearch[i];
        pFW = hashLookup(pHash, si, hashCode);
    }

    ficlLockDictionary(0);
    return pFW;
}


/**************************************************************************
                        d i c t L o o k u p L o c
** Same as dictLookup, but looks in system locals dictionary first...
** Assumes locals dictionary has only one wordlist...
**************************************************************************/
#if FICL_WANT_LOCALS
FICL_WORD *dictLookupLoc(FICL_DICT *pDict, STRINGINFO si)
{
    FICL_WORD *pFW = NULL;
    FICL_HASH *pHash = ficlGetLoc()->pForthWords;
    int i;
    UNS16 hashCode   = hashHashCode(si);

    assert(pHash);
    assert(pDict);

    ficlLockDictionary(1);
    /* 
    ** check the locals dict first... 
    */
    pFW = hashLookup(pHash, si, hashCode);

    /* 
    ** If no joy, (!pFW) --------------------------v
    ** iterate over the search list in the main dict 
    */
    for (i = (int)pDict->nLists - 1; (i >= 0) && (!pFW); --i)
    {
        pHash = pDict->pSearch[i];
        pFW = hashLookup(pHash, si, hashCode);
    }

    ficlLockDictionary(0);
    return pFW;
}
#endif


/**************************************************************************
                    d i c t R e s e t S e a r c h O r d e r
** Initialize the dictionary search order list to sane state
**************************************************************************/
void dictResetSearchOrder(FICL_DICT *pDict)
{
    assert(pDict);
    pDict->pCompile = pDict->pForthWords;
    pDict->nLists = 1;
    pDict->pSearch[0] = pDict->pForthWords;
    return;
}


/**************************************************************************
                        d i c t S e t F l a g s
** Changes the flags field of the most recently defined word:
** Set all bits that are ones in the set parameter, clear all bits
** that are ones in the clr parameter. Clear wins in case the same bit
** is set in both parameters.
**************************************************************************/
void dictSetFlags(FICL_DICT *pDict, UNS8 set, UNS8 clr)
{
    assert(pDict->smudge);
    pDict->smudge->flags |= set;
    pDict->smudge->flags &= ~clr;
    return;
}


/**************************************************************************
                        d i c t S e t I m m e d i a t e 
** Set the most recently defined word as IMMEDIATE
**************************************************************************/
void dictSetImmediate(FICL_DICT *pDict)
{
    assert(pDict->smudge);
    pDict->smudge->flags |= FW_IMMEDIATE;
    return;
}


/**************************************************************************
                        d i c t U n s m u d g e 
** Completes the definition of a word by linking it
** into the main list
**************************************************************************/
void dictUnsmudge(FICL_DICT *pDict)
{
    FICL_WORD *pFW = pDict->smudge;
    FICL_HASH *pHash = pDict->pCompile;

    assert(pHash);
    assert(pFW);
    /*
    ** :noname words never get linked into the list...
    */
    if (pFW->nName > 0)
        hashInsertWord(pHash, pFW);
    pFW->flags &= ~(FW_SMUDGE);
    return;
}


/**************************************************************************
                        d i c t W h e r e
** Returns the value of the HERE pointer -- the address
** of the next free cell in the dictionary
**************************************************************************/
CELL *dictWhere(FICL_DICT *pDict)
{
    return pDict->here;
}


/**************************************************************************
                        h a s h F o r g e t
** Unlink all words in the hash that have addresses greater than or
** equal to the address supplied. Implementation factor for FORGET
** and MARKER.
**************************************************************************/
void hashForget(FICL_HASH *pHash, void *where)
{
    FICL_WORD *pWord;
    unsigned i;

    assert(pHash);
    assert(where);

    for (i = 0; i < pHash->size; i++)
    {
        pWord = pHash->table[i];

        while ((void *)pWord >= where)
        {
            pWord = pWord->link;
        }

        pHash->table[i] = pWord;
    }

    return;
}


/**************************************************************************
                        h a s h H a s h C o d e
** 
** Generate a 16 bit hashcode from a character string using a rolling
** shift and add stolen from PJ Weinberger of Bell Labs fame. Case folds
** the name before hashing it...
** N O T E : If string has zero length, returns zero.
**************************************************************************/
UNS16 hashHashCode(STRINGINFO si)
{   
    /* hashPJW */
    UNS8 *cp;
    UNS16 code = (UNS16)si.count;
    UNS16 shift = 0;

    if (si.count == 0)
        return 0;

    for (cp = (UNS8 *)si.cp; *cp && si.count; cp++, si.count--)
    {
        code = (UNS16)((code << 4) + tolower(*cp));
        shift = (UNS16)(code & 0xf000);
        if (shift)
        {
            code ^= (UNS16)(shift >> 8);
            code ^= (UNS16)shift;
        }
    }

    return (UNS16)code;
}


/**************************************************************************
                        h a s h I n s e r t W o r d
** Put a word into the hash table using the word's hashcode as
** an index (modulo the table size).
**************************************************************************/
void hashInsertWord(FICL_HASH *pHash, FICL_WORD *pFW)
{
    FICL_WORD **pList;

    assert(pHash);
    assert(pFW);

    if (pHash->size == 1)
    {
        pList = pHash->table;
    }
    else
    {
        pList = pHash->table + (pFW->hash % pHash->size);
    }

    pFW->link = *pList;
    *pList = pFW;
    return;
}


/**************************************************************************
                        h a s h L o o k u p
** Find a name in the hash table given the hashcode and text of the name.
** Returns the address of the corresponding FICL_WORD if found, 
** otherwise NULL.
** Note: outer loop on link field supports inheritance in wordlists.
** It's not part of ANS Forth - ficl only. hashReset creates wordlists
** with NULL link fields.
**************************************************************************/
FICL_WORD *hashLookup(FICL_HASH *pHash, STRINGINFO si, UNS16 hashCode)
{
    FICL_COUNT nCmp = (FICL_COUNT)si.count;
    FICL_WORD *pFW;
    UNS16 hashIdx;

    if (nCmp > nFICLNAME)
        nCmp = nFICLNAME;

    for (; pHash != NULL; pHash = pHash->link)
    {
        if (pHash->size > 1)
            hashIdx = (UNS16)(hashCode % pHash->size);
        else            /* avoid the modulo op for single threaded lists */
            hashIdx = 0;

        for (pFW = pHash->table[hashIdx]; pFW; pFW = pFW->link)
        {
            if ( (pFW->nName == si.count) 
                && (!strincmp(si.cp, pFW->name, nCmp)) )
                return pFW;
#if FICL_ROBUST
            assert(pFW != pFW->link);
#endif
        }
    }

    return NULL;
}


/**************************************************************************
                             h a s h R e s e t
** Initialize a FICL_HASH to empty state.
**************************************************************************/
void hashReset(FICL_HASH *pHash)
{
    unsigned i;

    assert(pHash);

    for (i = 0; i < pHash->size; i++)
    {
        pHash->table[i] = NULL;
    }

    pHash->link = NULL;
    return;
}


