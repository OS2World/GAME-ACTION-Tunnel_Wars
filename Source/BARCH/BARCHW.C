/*--------------------------------------------------------------------------\
|                     Barchw.c - Write a BAR
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
/*______________________________________________________________________
|                                                                       
| System Defines                                                        
|_____________________________________________________________________*/
#define INCL_DOSPROCESS
#define INCL_DOSPROCESS 	/*  Process and thread support		*/
#define INCL_DOSDATETIME	/*  Date & Time info			*/
#define INCL_DOSMODULEMGR	/*  PM Dos Mangager			*/
#define INCL_VIO
#define INCL_DOSFILEMGR

/*______________________________________________________________________
|                                                                       
| System Includes                                                       
|_____________________________________________________________________*/
#include "os2.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*______________________________________________________________________
|                                                                       
| Application Includes                                                  
|_____________________________________________________________________*/
#define DIRECTION
#include "barchu.h"

/*______________________________________________________________________
|                                                                       
| Global Variables                                                      
|_____________________________________________________________________*/
typedef struct _FILE_LIST
{
    CHAR szFileName[CCHMAXPATH];
    
    struct _FILE_LIST *pNextNode;
} *PFILE_LIST, FILE_LIST;

PFILE_LIST pBegFileList = NULL;


USHORT     g_usArchiveVersion    = 0;
USHORT     g_usProductNumber     = 0;
CHAR       g_szArchiveName[50]   = {'\n'};
CHAR       g_szProductName[50]   = {'\n'};
CHAR       g_szArchiveDesc[255]  = {'\n'};

/*______________________________________________________________________
|                                                                       
|  Function Prototypes.                                                 
|______________________________________________________________________*/
ULONG GetNextOffset   ( PBARCH_INFO pbiInfo );
BOOL  WriteDataFile   ( FILE *fpArchive, PSZ pvData, LONG lFileSize, ULONG ulVirtualOffset, ULONG ulTotFiles );
BOOL  CloseArchive    ( FILE *fpArchive );
BOOL  WriteHeader     ( PBARCH_INFO pbiInfo, PBARCH_FILE_LIST pCurEntry );
BOOL  WriteFileHeader ( PBARCH_INFO pbiInfo );

BOOL  GetParm         ( PCHAR pchBuf, PCHAR pchLine, USHORT iParam, CHAR cParmTolken );
BOOL  LoadParm        ( PSZ *ppszDest, CHAR *szInput, USHORT usParmNumber, CHAR cParmTolken);
CHAR  ProcessListFile ( CHAR *szProfileFile);

PFILE_LIST AddNewFile ( CHAR *szFileName);

/*__________________________________________________________________________
|                                                                           
|    Function:								    
|                                                                           
| Description:								    
|                                                                           
|      Return:                                                              
|                                                                           
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- 
|                            C H A N G E    L O G                           
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- 
|                                                                           
|Date      PRG     Change                                                   
|--------- ------- -------------------------------------------------------- 
|07/17/95  RATAJIK Initial Development. 				    
|__________________________________________________________________________*/
int main(INT argc, CHAR *argv[])
{
    PBARCH_INFO  pbiInfo;
    PFILE_LIST   pCurFile;
    CHAR         szFileName[CCHMAXPATH];
    FILE        *fpArchive;
    
    printf("BARCH (Binary ARCHive.  V%u.00, Copyright 1995, Greg Ratajik\n", BARCH_VERSION);
    
    if(argc >= 2)
        strcpy(szFileName, argv[1]);
    else
    {
        printf("BARCHW [ListFile]\n");
        return(1);
    }
    
    if(ProcessListFile(szFileName))
    {
        if( (pbiInfo = OpenArchiveForWrite(g_szArchiveName)) != NULL)
        {
            printf("File Opened: %s\n", g_szArchiveName);
            
            pCurFile = pBegFileList;
            while(pCurFile)
            {
                
                ArchiveFile(pbiInfo, pCurFile->szFileName);
                
                pCurFile = pCurFile->pNextNode;
            }
        }
        
        CloseArchiveForWrite(pbiInfo);
    }         
    
    return(0);
}

PBARCH_INFO OpenArchiveForWrite(CHAR *szFileName)
{
    PBARCH_INFO  pbiFile = (PBARCH_INFO)malloc(sizeof(BARCH_INFO));
    FILE        *fpFile;
    
    memset(pbiFile, 0, sizeof(BARCH_INFO));
    
    if((fpFile = fopen(szFileName, "wb+")) != NULL)
    {
        pbiFile->fpBarchFile = fpFile;
        
        pbiFile->ulFiles = 0;
        
        pbiFile->usArchiveVersion = g_usArchiveVersion;
        pbiFile->usProductNumber  = g_usProductNumber ;
        
        strcpy(pbiFile->szArchiveName, g_szArchiveName);
        strcpy(pbiFile->szProductName, g_szProductName);
        strcpy(pbiFile->szArchiveDesc, g_szArchiveDesc);
        
        return(pbiFile);
    }
    
    return(NULL);
}


BOOL WriteFileHeader( PBARCH_INFO pbiInfo )
{
    BARCH_FILE_HEADER    bfhFile;
    
    INT                  size;
    memset(&bfhFile, 0, sizeof(bfhFile));
    
    bfhFile.ulFiles     = pbiInfo->ulFiles;
    bfhFile.usSignature = BARCH_SIGNATURE;
    
    bfhFile.usArchiveVersion = g_usArchiveVersion;
    bfhFile.usProductNumber  = g_usProductNumber ;
    
    strcpy(bfhFile.szArchiveName, g_szArchiveName);
    strcpy(bfhFile.szProductName, g_szProductName);
    strcpy(bfhFile.szArchiveDesc, g_szArchiveDesc);
    
    
    if(fwrite(&bfhFile, sizeof(bfhFile), 1, pbiInfo->fpBarchFile) == 0)
    {
        printf("ERROR: Can't write file header: %i.\n", ferror(pbiInfo->fpBarchFile));
        return(FALSE);
    }
    else
    {
        printf("File Header written.\n");
        
        printf("Archive Information:\n");
        printf("       Archive Name: %s\n",  bfhFile.szArchiveName   );
        printf("    Archive Version: %u\n",  bfhFile.usArchiveVersion);
        printf("     Product Number: %u\n",  bfhFile.usProductNumber );
        printf("        Total Files: %lu\n", bfhFile.ulFiles         );
        printf("       Product Name: %s\n",  bfhFile.szProductName   );
        printf("        Description: %s\n",  bfhFile.szArchiveDesc   );
        
        
        return(TRUE);
    }
}



BOOL ArchiveFile(PBARCH_INFO pbiInfo , CHAR *szFileName)
{
    PBARCH_HEADER pbhFile;
    FILE	       *fpFile;
    INT	       fhFile;
    LONG	       lFileSize;
    LONG	       lSizeRead;
    PVOID          szData;
    ULONG         rc;
    FILESTATUS3 fs;
    
    DosQPathInfo(szFileName,       /* Query path information           */
        FIL_STANDARD,
        (PVOID)&fs,
        sizeof(fs));
    
    lFileSize = fs.cbFile;
    
    if((fpFile = fopen(szFileName, "rb")) != NULL)
    {
        szData = malloc(lFileSize);
        memset(szData, 55, lFileSize);
        
        if( (lSizeRead = fread(szData, lFileSize, 1, fpFile)) == -1 )
            return(FALSE);
        else
        {
            BARCH_HEADER	   bhNew;
            PBARCH_FILE_LIST pblNew;
            
            bhNew.usFileID   = 1;
            bhNew.ulOffset   = GetNextOffset(pbiInfo);
            bhNew.ulSize	   = lFileSize;
            strcpy(bhNew.szFileName, szFileName);
            
            pblNew = AddNewFileEntry(pbiInfo, &bhNew, szData);
            pbiInfo->ulFiles++;
            
            printf("File Name: %s\n" , pblNew->szFileName);
            printf("     Size: %lu\n", pblNew->ulSize);
        }
    }
    else
    {
        printf("Unable to open file: %s\n", szFileName);
        return(FALSE);
    }
    
    fclose(fpFile);
}

ULONG GetNextOffset( PBARCH_INFO pbiInfo )
{
    PBARCH_FILE_LIST pCurEntry;
    
    if(pbiInfo->pBegEntry)
    {
        for(pCurEntry = pbiInfo->pBegEntry; pCurEntry->pNextNode != NULL; pCurEntry = pCurEntry->pNextNode);
        return(pCurEntry->ulOffset + pCurEntry->ulSize);
    }
    else
        return(0);
}

BOOL WriteDataFile(FILE *fpArchive, PSZ pvData, LONG lFileSize, ULONG ulVirtualOffset, ULONG ulTotFiles)
{
    ULONG ulRealOffset;
    ULONG ulErr;
    fpos_t pos;
    
    //printf("Writting Archive... \n");
    
    ulRealOffset = (sizeof(BARCH_FILE_HEADER) + (sizeof(BARCH_HEADER) * ulTotFiles) + ulVirtualOffset);
    //printf("Real offset=%lu\n", ulRealOffset);
    
    pos.__fpos_elem[0] = ulRealOffset;
    pos.__fpos_elem[1] = ulRealOffset;
    
    if(fsetpos(fpArchive, &pos))
        return(FALSE);
    else
    {
        fgetpos(fpArchive, &pos);
        
        if( (ulErr = fwrite(pvData, lFileSize, 1, fpArchive)) == 0)
            return(FALSE);
        else
            return(TRUE);
    }
}

BOOL CloseArchiveForWrite(PBARCH_INFO pbiInfo)
{
    PBARCH_FILE_LIST     pCurEntry;
    fpos_t 	      pos;
    
    printf("Writting Archive... \n");
    
    pos.__fpos_elem[0] = 0;
    pos.__fpos_elem[1] = 0;
    
    if(fsetpos(pbiInfo->fpBarchFile, &pos))
        return(FALSE);
    
    fgetpos(pbiInfo->fpBarchFile, &pos);
    
    printf("    Total Files: %lu\n", pbiInfo->ulFiles);
    
    WriteFileHeader(pbiInfo);
    
    pos.__fpos_elem[0] = sizeof(BARCH_FILE_HEADER);
    pos.__fpos_elem[1] = sizeof(BARCH_FILE_HEADER);
    
    if(fsetpos(pbiInfo->fpBarchFile, &pos))
        return(FALSE);
    
    fgetpos(pbiInfo->fpBarchFile, &pos);
    
    pCurEntry = pbiInfo->pBegEntry;
    while(pCurEntry)
    {
        WriteHeader(pbiInfo, pCurEntry);
        pCurEntry = pCurEntry->pNextNode;
    }
    
    pCurEntry = pbiInfo->pBegEntry;
    while(pCurEntry)
    {
        WriteDataFile(pbiInfo->fpBarchFile, pCurEntry->szData, pCurEntry->ulSize, pCurEntry->ulOffset, pbiInfo->ulFiles);
        pCurEntry = pCurEntry->pNextNode;
    }
    
    fclose(pbiInfo->fpBarchFile);
    
    return(TRUE);
}

BOOL WriteHeader(PBARCH_INFO pbiInfo, PBARCH_FILE_LIST pCurEntry)
{
    BARCH_HEADER bhCur;
    
    memset(&bhCur, 0, sizeof(bhCur));
    
    bhCur.usFileID = pCurEntry->usFileID;
    bhCur.ulOffset = pCurEntry->ulOffset;
    bhCur.ulSize	= pCurEntry->ulSize  ;
    
    strcpy(bhCur.szFileName, pCurEntry->szFileName);
    
    if(fwrite(&bhCur, sizeof(bhCur), 1, pbiInfo->fpBarchFile) == 0)
    {
        printf("   ERROR writting File Header node: %i.\n", ferror(pbiInfo->fpBarchFile));
        return(FALSE);
    }
    else
    {
        //printf("   File Header node written.\n");
        return(TRUE);
    }
}

/*__________________________________________________________________________
|                                                                           
|    Function:                                                              
|                                                                           
| Description:                                                              
|                                                                           
|      Return:                                                              
|                                                                           
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- 
|                            C H A N G E    L O G                           
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- 
|                                                                           
|Date      PRG     Change                                                   
|--------- ------- -------------------------------------------------------- 
|02/09/95  RATAJIK Initial Development.                                     
|__________________________________________________________________________*/
BOOL   GetParm (PCHAR pchBuf, PCHAR pchLine, USHORT iParam, CHAR cParmTolken)
{
    PCHAR pchBeg;
    PCHAR pchOutLine;
    PCHAR pchNext;
    BOOL  bStop;
    INT   j;
    
    for (pchBeg = pchLine; --iParam;
    pchBeg = (strchr(pchBeg, cParmTolken) + 1))
        //printf("");
        
        if (pchBeg == (PCHAR)1)
            return(FALSE);
        
        pchOutLine = pchBuf;
        /* copy param */
        while ((*pchBeg != cParmTolken) && (*pchBeg != '\0'))
            *pchBuf++ = *pchBeg++;
        
        *pchBuf = '\0';
        
        
        bStop = FALSE;
        for (; *pchOutLine && !bStop; pchOutLine++)
        {
            if (*pchOutLine  == ' ')
            {
                for (pchNext = pchOutLine; *pchNext == ' '; pchNext++);
                
                strcpy(pchOutLine, pchNext);
                bStop = TRUE;
                break;
            }
        }
        
        for(j = strlen(pchOutLine)-1; j >= 0 && *(pchOutLine + j) == ' '; j--)
            if(*(pchOutLine + j) == ' ')
                *(pchOutLine + j) = '\0';
            
            strcpy(pchBuf, pchOutLine);
            if(strlen(pchBuf) == 0)
                return(FALSE);
            else
                return(TRUE);
            
}

/*__________________________________________________________________________
|                                                                           
|    Function:                                                              
|                                                                           
| Description:                                                              
|                                                                           
|      Return:                                                              
|                                                                           
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- 
|                            C H A N G E    L O G                           
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- 
|                                                                           
|Date      PRG     Change                                                   
|--------- ------- -------------------------------------------------------- 
|02/09/95  RATAJIK Initial Development.                                     
|__________________________________________________________________________*/
BOOL LoadParm(PSZ *ppszDest, CHAR *szInput, USHORT usParmNumber, CHAR cParmTolken)
{
    CHAR szReturn[CCHMAXPATH] = { '0' };
    
    /*___________________________________
    |                                    |
    | Load the parm                      |
    |                                    |
    |___________________________________*/
    if(GetParm(szReturn, szInput, usParmNumber, cParmTolken))
    {
    /*___________________________________
    |                                    |
    | Allocate the memory. Copy the parm |
    | to the new memory.                 |
    |___________________________________*/
        *ppszDest = (PSZ)malloc(strlen(szReturn) + 1);
        strcpy(*ppszDest, szReturn);
        return(TRUE);
    }
    else
        return(FALSE);
    
}

/*__________________________________________________________________________
|                                                                           
|    Function:                                                              
|                                                                           
| Description:                                                              
|                                                                           
|      Return:                                                              
|                                                                           
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- 
|                            C H A N G E    L O G                           
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- 
|                                                                           
|Date      PRG     Change                                                   
|--------- ------- -------------------------------------------------------- 
|02/09/95  RATAJIK Initial Development.                                     
|__________________________________________________________________________*/
CHAR ProcessListFile(CHAR *szProfileFile)
{
#define PARM_ARCHIVE_FILENAME        "ARCHIVE_FILENAME"
#define PARM_ARCHIVE_DESC            "ARCHIVE_DESC"
#define PARM_ARCHIVE_VERSION         "ARCHIVE_VERSION"
#define PARM_PRODUCT_NAME            "PRODUCT_NAME"
#define PARM_PRODUCT_NUMBER          "PRODUCT_NUMBER"
#define PARM_FILE_NAME               "FILE_NAME"
    
    FILE * fhIniFile;
    CHAR   szInput[1000];
    CHAR   szParm[200];
    INT    loop;
    CHAR   szTmp[255];
    BOOL   bRet = TRUE;
    
    /*___________________________________
    |                                    |
    | Open the script file.              |
    |                                    |
    |___________________________________*/
    if( (fhIniFile = fopen(szProfileFile, "r")) != NULL)
    {
    /*___________________________________
    |                                    |
    | Read untill EOF.                   |
    |                                    |
    |___________________________________*/
        while(fgets(szInput, sizeof(szInput), fhIniFile) != NULL)
        {
        /*___________________________________
        |                                    |
        | A '#' in as the first char will    |
        | indicate a comment. Skip it.       |
        |___________________________________*/
            if(szInput[0] != '#')
            {
            /*___________________________________
            |                                    |
            | Strip invalid characters out of the|
            | line.                              |
            |___________________________________*/
                for(loop = 0; loop < strlen(szInput); loop++)
                    if(szInput[loop] < 32)
                        szInput[loop] = ' ';
                    
                
                    /*___________________________________
                    |                                    |
                    | Get each parm we need.             |
                    |                                    |
                    |___________________________________*/
                    GetParm(szParm, szInput, 1, '=');
                    
                    strupr(szParm);
                    /*___________________________________
                    |                                    |
                    | Figure out if the which KEY is     |
                    | being used for the parm.  When we  |
                    | get the correct one, load the parm |
                    | into the variable.                 |
                    |___________________________________*/
                    if(!(strcmp(szParm, PARM_FILE_NAME         )))
                    {
                        GetParm(szTmp, szInput, 2 , '=');
                        
                        /*___________________________________
                        |                                    |
                        | Turn off the exception handler.    |
                        |                                    |
                        |___________________________________*/
                        AddNewFile(szTmp);
                        
                    }
                    else
                        if(!(strcmp(szParm, PARM_ARCHIVE_FILENAME      )))
                        {
                            GetParm(g_szArchiveName, szInput, 2 , '=');
                        }
                        else
                            if(!(strcmp(szParm, PARM_ARCHIVE_VERSION   )))
                            {
                                GetParm(szTmp, szInput, 2 , '=');
                                g_usArchiveVersion = atoi(szTmp);
                            }
                            else
                                if(!(strcmp(szParm, PARM_PRODUCT_NAME      )))
                                {
                                    GetParm(g_szProductName, szInput, 2 , '=');
                                }
                                else
                                    if(!(strcmp(szParm, PARM_PRODUCT_NUMBER       )))
                                    {
                                        GetParm(szTmp, szInput, 2 , '=');
                                        g_usProductNumber = atoi(szTmp);
                                        
                                    }
                                    else
                                        if(!(strcmp(szParm, PARM_ARCHIVE_DESC      )))
                                        {
                                            GetParm(g_szArchiveDesc, szInput, 2 , '=');
                                        }
                                        
            }
        }
        
        
        fclose(fhIniFile);
    }
    else
    {
        printf("Unable to open profile file: %s", szProfileFile);
        bRet = FALSE;
    }
    
    
    if(!g_szArchiveName)
    {
        bRet = FALSE;
        printf("The Name of the Archive must be entered.\n");
    }
    else
    {
        if(!pBegFileList)
        {
            bRet = FALSE;
            printf("At least one file name must be entered.\n");
        }
    }
    
    return(bRet);
}










PFILE_LIST AddNewFile(CHAR *szFileName)
{
    static PFILE_LIST pLastEntry;
    PFILE_LIST pNewEntry;
    PFILE_LIST pCurEntry;
    
    /*___________________________________
    |                                    |
    | Allocate the new node.             |
    |                                    |
    |___________________________________*/
    pNewEntry = (PFILE_LIST)malloc(sizeof(FILE_LIST));
    
    /*___________________________________
    |                                    |
    | Clear the new memory out.          |
    |                                    |
    |___________________________________*/
    memset(pNewEntry, 0, sizeof(FILE_LIST));
    
    /*___________________________________
    |                                    |
    | Find the last node, and add the    |
    | new node ot the end of the list.   |
    |___________________________________*/
    if(pBegFileList == NULL)
    {
        pCurEntry = pBegFileList = pNewEntry;
    }
    else
    {
        /*___________________________________
        |                                    |
        | Find the last node in the list     |
        |                                    |
        |___________________________________*/
        for(pCurEntry = pBegFileList; pCurEntry->pNextNode != NULL; pCurEntry = pCurEntry->pNextNode);
        
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
    strcpy(pCurEntry->szFileName, szFileName);
    
    return(pCurEntry);
}
#include "barchu.c" //.. Yeah, I Know.. LAME!
