/*--------------------------------------------------------------------------\
|                     Test.c - Test a BAR file
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
|                                                                       |
| System Defines                                                        |
|_____________________________________________________________________*/
#define INCL_DOSPROCESS
#define INCL_DOSPROCESS 	/*  Process and thread support		*/
#define INCL_DOSDATETIME	/*  Date & Time info			*/
#define INCL_DOSMODULEMGR	/*  PM Dos Mangager			*/
#define INCL_VIO
#define INCL_DOSFILEMGR

/*______________________________________________________________________
|                                                                       |
| System Includes                                                       |
|_____________________________________________________________________*/
#include "os2.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*______________________________________________________________________
|                                                                       |
| Application Includes                                                  |
|_____________________________________________________________________*/
#include "barch.h"

/*______________________________________________________________________
|                                                                       |
| Global Variables                                                      |
|_____________________________________________________________________*/

/*__________________________________________________________________________
|                                                                           |
|    Function:								    |
|                                                                           |
| Description:								    |
|                                                                           |
|      Return:                                                              |
|                                                                           |
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- |
|                            C H A N G E    L O G                           |
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- |
|                                                                           |
|Date      PRG     Change                                                   |
|--------- ------- -------------------------------------------------------- |
|07/17/95  RATAJIK Initial Development. 				    |
|__________________________________________________________________________*/
int main(INT argc, CHAR *argv[])
{
PBARCH_FILE_LIST pCurEntry;
PBARCH_INFO      pbiInfo;

 if( (pbiInfo = OpenArchiveForRead("test.bar")) != NULL)
    {
     pCurEntry = pbiInfo->pBegEntry;
     while(pCurEntry)
        {
         PSZ pszData;
         printf("Getting File %s\n", pCurEntry->szFileName);


         if( (pszData = GetFileFromArchive(pbiInfo, "test3.txt")) != NULL)
            printf("get test3.txt by reference.\n");


         if( (pszData = ReadFileFromArchive(pbiInfo, pCurEntry)) != NULL)
             printf("Got Data! \n");

         WriteFile(pCurEntry, pszData);

         pCurEntry = pCurEntry->pNextNode;
        }

     CloseArchiveForRead(pbiInfo);
    }

 return(0);
}
