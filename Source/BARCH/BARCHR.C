/*--------------------------------------------------------------------------\
|                     Barchr.c - Read a BAR file
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
PBARCH_INFO  OpenArchiveForRead ( CHAR *szFileName);
BOOL         CloseArchiveForRead( PBARCH_INFO pbiInfo);

/*______________________________________________________________________
|                                                                       
|  Function Prototypes.                                                 
|______________________________________________________________________*

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
#ifndef NO_MAIN
int main(INT argc, CHAR *argv[])
{
    FILE *fpArchive;
    PBARCH_FILE_LIST pCurEntry;
    PBARCH_INFO pbiInfo;
    
    if( (pbiInfo = OpenArchiveForRead("test.bar")) != NULL )
    {
        printf("File Opened.\n");
        
        pCurEntry = pbiInfo->pBegEntry;
        while(pCurEntry)
        {
            PSZ pszData;
            
            printf("Getting File %s\n", pCurEntry->szFileName);
            
            pszData = ReadFileFromArchive(pbiInfo, pCurEntry);
            
            WriteFile(pCurEntry, pszData);
            
            pCurEntry = pCurEntry->pNextNode;
            
        }
        
        CloseArchiveForRead(pbiInfo);
    }
    return(0);
}
#endif

BOOL CloseArchiveForRead(PBARCH_INFO pbiInfo)
{
    PBARCH_FILE_LIST     pCurEntry;
    
    fclose(pbiInfo->fpBarchFile);
    return(TRUE);
}

PBARCH_INFO OpenArchiveForRead(CHAR *szFileName)
{
    PBARCH_INFO pbiFile = (PBARCH_INFO)malloc(sizeof(BARCH_INFO));
    ULONG ulFiles;
    
    memset(pbiFile, 0, sizeof(BARCH_INFO));
    
    if( (pbiFile->fpBarchFile = fopen(szFileName, "rb+")) != NULL)
    {
        if( (ulFiles = ReadFileHeader(pbiFile)) != 0)
        {
            ReadFileList(pbiFile);
            
            return(pbiFile);
        }
        else
        {
            printf("Invalid file: %s", szFileName);
            return(NULL);
        }
    }
    else
        return(NULL);
    
}

PSZ GetFileFromArchive(PBARCH_INFO pbiInfo, CHAR *szFileName)
{
    PBARCH_FILE_LIST pCurEntry;
    PBARCH_FILE_LIST pBegEntry;
    
    pCurEntry = pbiInfo->pBegEntry;
    while(pCurEntry)
    {
        if(!stricmp(pCurEntry->szFileName, szFileName))
        {
            return(ReadFileFromArchive(pbiInfo, pCurEntry));
        }
        else
            pCurEntry = pCurEntry->pNextNode;
    }
    
    return(NULL);
}

ULONG GetFileSizeFromArchive(PBARCH_INFO pbiInfo, CHAR *szFileName)
{
    PBARCH_FILE_LIST pCurEntry;
    PBARCH_FILE_LIST pBegEntry;
    
    pCurEntry = pbiInfo->pBegEntry;
    while(pCurEntry)
    {
        if(!stricmp(pCurEntry->szFileName, szFileName))
        {
            return(pCurEntry->ulSize);
        }
        else
            pCurEntry = pCurEntry->pNextNode;
    }
    
    return(0);
}


PSZ ReadFileFromArchive(PBARCH_INFO pbiInfo, PBARCH_FILE_LIST pbhEntry)
{
    ULONG ulRealPos;
    PSZ   pszData;
    INT   x;
    
    ulRealPos = sizeof(BARCH_FILE_HEADER) +
        (sizeof(BARCH_HEADER) * pbiInfo->ulFiles) +
        pbhEntry->ulOffset;
    
    x = fseek(pbiInfo->fpBarchFile, ulRealPos, SEEK_SET);
    
#if 0
    printf("  **Reading File Data: \n");
    printf("        File Name: %s\n", pbhEntry->szFileName);
    printf("      File Offset: %lu\n", pbhEntry->ulOffset);
    printf("      Real Offset: %lu\n", ulRealPos);
    printf("         File Size: %lu\n", pbhEntry->ulSize);
#endif
    pbhEntry->szData = (PSZ)malloc(pbhEntry->ulSize);
    if( (x = fread(pbhEntry->szData, pbhEntry->ulSize, 1, pbiInfo->fpBarchFile)) == 0 )
    {
        printf("Error Reading file.\n");
        return(FALSE);
    }
    else
        return(pbhEntry->szData);
    
}

ULONG ReadFileHeader(PBARCH_INFO pbiInfo)
{
    fpos_t pos;
    INT x;
    BARCH_FILE_HEADER bfhFile;
    
    x = fseek(pbiInfo->fpBarchFile, 0, SEEK_SET);
    if( (fread(&bfhFile, sizeof(bfhFile), 1, pbiInfo->fpBarchFile)) == 0 )
        return(FALSE);
    else
    {
        if(bfhFile.usSignature != BARCH_SIGNATURE)
        {
            printf("Invalid signature in file header.\n");
            return(FALSE);
        }
        else
        {
            pbiInfo->ulFiles          = bfhFile.ulFiles;
            pbiInfo->usArchiveVersion = bfhFile.usArchiveVersion;
            pbiInfo->usProductNumber  = bfhFile.usProductNumber ;
            
            strcpy(pbiInfo->szArchiveName, bfhFile.szArchiveName);
            strcpy(pbiInfo->szProductName, bfhFile.szProductName);
            strcpy(pbiInfo->szArchiveDesc, bfhFile.szArchiveDesc);
            
            printf("Archive Information:\n");
            printf("       Archive Name: %s\n",  pbiInfo->szArchiveName   );
            printf("    Archive Version: %u\n",  pbiInfo->usArchiveVersion);
            printf("     Product Number: %u\n",  pbiInfo->usProductNumber );
            printf("        Total Files: %lu\n", pbiInfo->ulFiles         );
            printf("       Product Name: %s\n",  pbiInfo->szProductName   );
            printf("        Description: %s\n",  pbiInfo->szArchiveDesc   );
            
#if 0
            printf("Valid signature in file header. %lu files in archive.\n", pbiInfo->ulFiles);
#endif
            return(bfhFile.ulFiles);
        }
    }
}


BOOL ReadFileList ( PBARCH_INFO pbiInfo)
{
    BARCH_HEADER bhNewEntry;
    INT          j;
    fpos_t	     pos;
    size_t         x;
    
    x = fseek(pbiInfo->fpBarchFile, sizeof(BARCH_FILE_HEADER), SEEK_SET);
    
    for(j = 0; j < pbiInfo->ulFiles; j++)
    {
        if( (fread(&bhNewEntry, sizeof(bhNewEntry), 1, pbiInfo->fpBarchFile)) == 0)
        {
            printf("Error Reading File Table: %i\n", ferror(pbiInfo->fpBarchFile));
            return(FALSE);
        }
        else
        {
            AddNewFileEntry(pbiInfo, &bhNewEntry, NULL);
        }
    }
    
    
}

#include "barchu.c" //.. Yeah, I Know.. LAME!
