/*--------------------------------------------------------------------------\
|                     Barchu.c - Barch util files 
|
|               For the latest version of the code, go to:
|                    http://www.ratajik.net/TunnelWars
|
|               For any questions or comments, send mail to:
|                       TunnelWars@ratajik.net
|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
|                         Dive (aka TunnelWars)
|                    Copyright (c) 1995, 98 Greg Ratajik
|                            All rights reserved.
|
|   Redistribution and use in source and binary forms, with or without
|   modification, are permitted provided that the following conditions
|   are met:
|       1. Redistributions of source code must retain the above copyright
|          notice, this list of conditions and the following disclaimer.
|       2. Redistributions in binary form must reproduce the above copyright
|          notice, this list of conditions and the following disclaimer in
|          the documentation and/or other materials provided with the 
|          distribution.
|       3. All advertising materials mentioning features or use of this 
|          software must display the following acknowledgement:
|              This product includes software developed by Greg Ratajik
|       4. The name of the author may not be used to endorse or promote 
|          products derived from this software without specific prior
|          written permission.
|
|   THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
|   IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
|   OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
|   IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
|   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
|   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
|   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
|   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
|   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
|   THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
|
|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
|Ver   Date      PRG     Change
|----- --------- ------- ----------------------------------------
|1.00  04/05/95  RATAJIK Initial Development
|1.00  05/09/98  Ratajik Released.
\_________________________________________________________________________*/
PBARCH_FILE_LIST AddNewFileEntry(PBARCH_INFO pbiInfo, PBARCH_HEADER pEntry, CHAR *szData )
{
    static PBARCH_FILE_LIST pLastEntry;
    PBARCH_FILE_LIST pNewEntry;
    PBARCH_FILE_LIST pCurEntry;
    PBARCH_FILE_LIST pBegEntry;
    
    /*___________________________________
    |                                    |
    | Allocate the new node.             |
    |                                    |
    |___________________________________*/
    pNewEntry = (PBARCH_FILE_LIST)malloc(sizeof(BARCH_FILE_LIST));
    
    /*___________________________________
    |                                    |
    | Clear the new memory out.          |
    |                                    |
    |___________________________________*/
    memset(pNewEntry, 0, sizeof(BARCH_FILE_LIST));
    
    /*___________________________________
    |                                    |
    | Find the last node, and add the    |
    | new node ot the end of the list.   |
    |___________________________________*/
    if(pbiInfo->pBegEntry == NULL)
    {
        pCurEntry = pbiInfo->pBegEntry = pNewEntry;
    }
    else
    {
    /*___________________________________
    |                                    |
    | Find the last node in the list     |
    |                                    |
        |___________________________________*/
        for(pCurEntry = pbiInfo->pBegEntry; pCurEntry->pNextNode != NULL; pCurEntry = pCurEntry->pNextNode);
        
        /*___________________________________
        |                                    |
        | Set pPrevNode in the current node  |
        | to the last node.                  |
        |___________________________________*/
        pNewEntry->pPrevNode = pCurEntry;
        
        /*___________________________________
        |                                    |
        | Set pNextNode in the current node  |
        | to the next node.                  |
        |___________________________________*/
        pCurEntry->pNextNode = pNewEntry;
        pCurEntry = pNewEntry;
    }
    
    /*___________________________________
    |                                    |
    | Allocate enough room for the       |
    | strings.                           |
    |___________________________________*/
    pCurEntry->usFileID	  = pEntry->usFileID  ;
    pCurEntry->ulOffset	  = pEntry->ulOffset  ;
    pCurEntry->ulSize        = pEntry->ulSize    ;
    
    strcpy(pCurEntry->szFileName, pEntry->szFileName);
    
    if(szData != NULL)
    {
        pCurEntry->szData = (CHAR *)malloc(pEntry->ulSize);
        memcpy(pCurEntry->szData, szData, pEntry->ulSize);
    }
    
#if 0
    printf("File Table: \n");
    printf("   File Name   = %s\n", pCurEntry->szFileName);
    printf("   File Size   = %lu\n", pCurEntry->ulSize);
    printf("   File Offset = %lu\n", pCurEntry->ulSize);
    printf("   File Number = %lu\n", pbiInfo->ulFiles+1);
#endif
    
    return(pCurEntry);
}

BOOL WriteFile(PBARCH_FILE_LIST pbhEntry, PSZ pszData)
{
    FILE *fpFile;
    
    if( (fpFile = fopen(pbhEntry->szFileName, "wb+")) != NULL)
    {
        if(fwrite(pszData, pbhEntry->ulSize, 1, fpFile) == 0)
        {
            printf("ERROR: Can't write file.\n");
            return(FALSE);
        }
        else
        {
            printf("File written.\n");
            fclose(fpFile);
            return(TRUE);
        }
    }
    
    return(FALSE);  
}
