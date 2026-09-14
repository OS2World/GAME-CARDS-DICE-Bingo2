/******************************************************************************/
/*                       (c) IBM Corp. 1994                                   */
/*                                                                            */
/* Program: OS/2 PM Bingo - Print Support                                     */
/*                                                                            */
/* Author : E. L. Zapanta                                                     */
/* Date   : 29Jul94                                                           */
/*                                                                            */
/******************************************************************************/

#define INCL_WIN
#define INCL_BASE
#define INCL_GPI
#define INCL_SPL
#define INCL_SPLDOSPRINT
#define INCL_DEV
#define INCL_DOS

#define PROGRESSBARLENGTH  30
#define DEVICENAME_LENGTH  32
#define DRIVERNAME_LENGTH 128
#define MAX_PRINT_FORMS    20

#include <os2.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <os2me.h>                     /* OS/2 multimedia extensions          */
#include "bingowav.h"                  /* WAVE file definitions               */
#include "bingo.h"                     /* main include file                   */
#include "bingodlg.h"                  /* include file for dialog windows     */

VOID fnProcessPrint( HWND hwnd, LONG page );
LONG fnPrint( LONG page );
#pragma linkage (fnPrint, system)

/******************************************************************************/
/*                                                                            */
/* wpPrintDlgProc: Window procedure for Print dialog window.                  */
/*                                                                            */
/******************************************************************************/

MRESULT EXPENTRY wpPrintDlgProc (HWND hwnd, ULONG msg, MPARAM mp1,
                                 MPARAM mp2)

{

 MRESULT mr = (MRESULT) FALSE;

 switch (msg)
    {
     case WM_INITDLG:                  /* Dialog box just created. Initialize */
        {                              /* list box, etc.                      */

          ULONG     cbReturned;
          ULONG     cbTotal;
          ULONG     cbNeeded;
          ULONG     cbBuf;
          PBYTE     pBuf;
          SPLERR    splErr;
          PPRQINFO3 prq;
          USHORT    i;
          CHAR      achDeviceName[DEVICENAME_LENGTH];
          PSZ       pszTemp;

          if(debug)
            fprintf(wptrDebug, "WM_INITDLG for Print dialog.\n");

          /* First find all the available printer queues in the system */
          /* which will be place in a listbox of the Select Printer    */
          /* dialog window.                                            */
          splErr = SplEnumQueue((PSZ)NULL, 3, pBuf, 0L,
                                &cbReturned, &cbTotal, &cbNeeded, NULL);
          if(splErr == ERROR_MORE_DATA)
          {
            if(!DosAllocMem((PVOID)&pBuf, cbNeeded,
                            PAG_READ | PAG_WRITE | PAG_COMMIT))
            {
              cbBuf = cbNeeded;
              splErr = SplEnumQueue((PSZ)NULL, 3, pBuf, cbBuf,
                                    &cbReturned, &cbTotal, &cbNeeded, NULL);
            }
            else
            {
              CHAR title[21];
              CHAR text[81];
              CHAR szError[200];

              WinLoadString (hab, 0L, IDS_PRINTERROR, 20, title );
              WinLoadString (hab, 0L, IDS_ERROR_PRINT_1, 80, text );
              sprintf(szError, "%s %d.", text, splErr);
              WinMessageBox (HWND_DESKTOP, 0L, szError,
                             title, 1, MB_ERROR | MB_OK | MB_MOVEABLE);

              if(debug)
                fprintf(wptrDebug, "%s %s\n", title, szError);

            }
          }

          if(splErr == NO_ERROR && cbReturned > 0)
          {
            pbd->prq = (PPRQINFO3)pBuf; /* save this address, use later       */
            prq      = (PPRQINFO3)pBuf; /* temporary, will increment          */
                                        /* get the default printer            */
            PrfQueryProfileString (HINI_PROFILE, "PM_SPOOLER", "PRINTER", NULL,
                                   (PSZ)achDeviceName, sizeof(achDeviceName));
            for(i = 0; i < cbReturned; i++)
            {                           /* insert printer queues in listbox   */
              pszTemp = strchr(prq->pszPrinters, ',');
              if( pszTemp )
              {
                /* Strip off comma and trailing printer names */
                *pszTemp = '\0' ;
              }
              WinSendDlgItemMsg(hwnd, IDL_PRINT, LM_INSERTITEM,
                                MPFROM2SHORT(LIT_END, 0),
                                MPFROMP(prq->pszComment));
              if(debug)
                fprintf(wptrDebug, "Printer queue found %s.\n", prq->pszName);

              /* Select the default print queue here */
              if(!strncmp(achDeviceName, prq->pszName, strlen(prq->pszName)))
                WinSendDlgItemMsg(hwnd, IDL_PRINT, LM_SELECTITEM,
                                  MPFROMLONG(i), MPFROMSHORT(TRUE));
              prq++;
            }
          }
          else
          {
            CHAR title[21];
            CHAR text[81];
            CHAR szError[200];

            DosFreeMem(pBuf);
            WinLoadString (hab, 0L, IDS_PRINTERROR, 20, title );
            WinLoadString (hab, 0L, IDS_ERROR_PRINT_2, 80, text );
            sprintf(szError, "%s %d.", text, splErr);

            WinMessageBox (HWND_DESKTOP, 0L, szError,
                           title, 1, MB_ERROR | MB_OK | MB_MOVEABLE);

            if(debug)
              fprintf(wptrDebug, "%s %s\n", title, szError);

          }
        }
        break;                         /* end case (WM_INITDLG)               */

     case WM_COMMAND:
        switch (SHORT1FROMMP(mp1))
           {
            case DID_OK:               /* User clicked on OK push button      */
               {
                 LONG      i;
                 LONG      l;
                 PPRQINFO3 prq;
                 LONG      page;

                 /* Determine which printer queue the user selected in listbox*/
                 /* and position prq pointer to that address                  */
                 prq = pbd->prq;
                 i = (LONG) WinSendDlgItemMsg( hwnd, IDL_PRINT,
                                               LM_QUERYSELECTION, 0L, 0L);
                 for(l=0; l< i; l++)
                   prq++;
                 pbd->prq = prq;
                 pbd->pDriverData = prq->pDriverData;
                                       /* 4 cards per page option checkbox    */
                 page = (LONG) WinSendDlgItemMsg( hwnd, IDC_4PAGE,
                               BM_QUERYCHECK, 0L, 0L );

                 if(debug)
                   fprintf(wptrDebug, "DID_OK: Printer selected is %s %d.\n",
                                      prq->pszName, page);

                 fnProcessPrint(hwnd, page);        /* Main print processing  */

                 WinDismissDlg( hwnd, TRUE );       /* Remove the dialog box  */
               }                       /* end case (DID_OK)                   */
               break;

            case IDP_JOBPROP:          /* User clicked on Job Properties      */
               {
                 CHAR      achDriverName[DRIVERNAME_LENGTH];
                 CHAR      achDeviceName[DEVICENAME_LENGTH];
                 LONG      i;
                 LONG      l;
                 PSZ       pszTemp;
                 PPRQINFO3 prq;

                 i = (LONG) WinSendDlgItemMsg( hwnd, IDL_PRINT,
                                               LM_QUERYSELECTION, 0L, 0L);
                 prq = pbd->prq;
                 for(l=0; l< i; l++)
                   prq++;

                 if(debug)
                   fprintf(wptrDebug, "IDP_JOBPROP: Printer selected is %s.\n",
                                      prq->pszName);

                 pbd->pDriverData = prq->pDriverData;

                 /* The pszDriverName is of the form DRIVER.DEVICE (e.g.,     */
                 /* LASERJET.HP LaserJet IID) so we need to separate it       */
                 /* at the dot                                                */
                 i = strcspn(prq->pszDriverName, ".");
                 if (i)
                 {
                   strncpy(achDriverName, prq->pszDriverName, i);
                   achDriverName[i] = '\0';
                   strcpy(achDeviceName, &prq->pszDriverName[i + 1]);
                 }
                 else
                 {
                   strcpy(achDriverName, prq->pszDriverName);
                   *achDeviceName = '\0';
                 }

                 /* There may be more than one printer assigned to this print */
                 /* queue. We will use the first in the comma separated list. */
                 /* We would need an expanded dialog for the user to be       */
                 /* more specific.                                            */
                 pszTemp = strchr(prq->pszPrinters, ',');
                 if( pszTemp )
                 {
                   /* Strip off comma and trailing printer names */
                   *pszTemp = '\0' ;
                 }

                 /* Post the job properties dialog for the printer to allow   */
                 /* the user to modify the options                            */
                 l = DevPostDeviceModes( hab,
                                         pbd->pDriverData,
                                         achDriverName,
                                         achDeviceName,
                                         prq->pszPrinters,
                                         DPDM_POSTJOBPROP );
               }
               break;

            case DID_CANCEL:           /* User clicked on Cancel push button  */
               DosFreeMem(pbd->prq);
               WinDismissDlg( hwnd, FALSE );
               break;

            case IDP_HELPPRINT:
               break;

            default:                   /* Let PM handle rest of messages      */
               mr = WinDefDlgProc( hwnd, msg, mp1, mp2 );
               break;
           }
        break;

     default:                          /* Let PM handle rest of messages      */
        mr = WinDefDlgProc( hwnd, msg, mp1, mp2 );
        break;
    }

 return( mr );

}

/******************************************************************************/
/*                                                                            */
/* wpPrtStatDlgProc: Window procedure for Print Status dialog window.         */
/*                                                                            */
/******************************************************************************/

MRESULT EXPENTRY wpPrtStatDlgProc (HWND hwnd, ULONG msg, MPARAM mp1,
                                MPARAM mp2)

{

 MRESULT mr = (MRESULT) FALSE;

 switch (msg)
    {
     case WM_INITDLG:                  /* Dialog box just created. Initialize */
        {                              /* list box, etc.                      */
           LONG page;

           page = * (PLONG) PVOIDFROMMP (mp2);

           pbd->hwndPrint = hwnd;      /* Save the window handle, print thread*/
                                       /* will send status msgs during process*/
           bingostatus = PRINTING;     /* Change bingo status to PRINTING     */
                                       /* Disable appropriate menu options    */
           WinSendMsg(hwndBoard, UM_PRINT, MPFROMLONG(FALSE), 0L);
                                       /* Create thread to perform actual work*/
           DosCreateThread( &pbd->threadID, (PFNTHREAD)fnPrint,
                            (ULONG)page, 0L, STACKSIZE );

        }
        break;                         /* end case (WM_INITDLG)               */

     case UM_STATUS:                   /* Status message received from print  */
        {                              /* thread. Instead info in listbox     */
          PCARDLIST cardNo;
          LONG      i;
          LONG      card;
          CHAR      text[81];
          CHAR      szStatus[81];
          CHAR      barIndicator[PROGRESSBARLENGTH+1];
          LONG      barCounter;
          PCARDLIST item;
                                       /* Parameter passed in card address    */
          cardNo = (PCARDLIST)LONGFROMMP(mp1);

          card = 1;                    /* Determine which card number we are  */
          item = start;                /* printing now                        */
          while(item)
          {
            if(item == cardNo)         /* Found it, number is stored in i     */
              break;
            card++;
            item = (PCARDLIST) item->nextcard;
          }
                                       /* Insert status information in listbox*/
          WinLoadString(hab, 0L, IDS_PRINT_STATUS, 80, text);
          sprintf(szStatus, "%s %d (%d of %d)",
                            text, cardNo, card, numberofcards);
          WinSendDlgItemMsg(hwnd, IDL_STATUS, LM_INSERTITEM,
                                  MPFROM2SHORT(LIT_END, 0),
                                  MPFROMP(szStatus));

                                       /* Update progress indicator           */
          strcpy( barIndicator, "лллллллллллллллллллллллллллллл");
          /*                     123456789012345678901234567890 */
          barCounter = PROGRESSBARLENGTH * card / numberofcards;
          for(i = PROGRESSBARLENGTH; i > barCounter; i--)
            barIndicator[i-1] = ' ';
          if(i>0)                      /* Update the entry field with bar     */
            WinSetWindowText(WinWindowFromID(hwnd, IDE_PROGRESS), barIndicator);

        }
        break;                         /* end case UM_STATUS                  */

     case UM_STOPPRINT:                /* User clicked on Stop button or      */
        {                              /* the print thread has completed,     */
                                       /* let's go some cleanup work here     */
          DosKillThread(pbd->threadID);

          bingostatus = BINGOSHOWN;    /* Change bingostatus to BINGOSHOWN    */
                                       /* Can only print during if not INPROG.*/
                                       /* Enable appropriate menu options     */
          WinSendMsg(hwndBoard, UM_PRINT, MPFROMLONG(TRUE), 0L);

          DosFreeMem(pbd->prq);        /* Free memory allocated earlier       */

          WinDismissDlg( hwnd, TRUE ); /* Remove the dialog box               */
        }
        break;                         /* end case (UM_STOPPRINT)             */

     case WM_COMMAND:
        switch (SHORT1FROMMP(mp1))
           {
            case DID_CANCEL:           /* User clicked on Stop button.        */
               {
                 CHAR title[21];
                 CHAR text[81];

                 WinLoadString (hab, 0L, IDS_PRINTTEXT, 20, title );
                 WinLoadString (hab, 0L, IDS_STOPPRINT, 80, text );
                 if( WinMessageBox( HWND_DESKTOP, hwndBoard, text, title,
                                    IDD_STOPPRINT,
                                    MB_MOVEABLE | MB_YESNO | MB_HELP |
                                    MB_DEFBUTTON1 ) == MBID_YES )
                   WinSendMsg(hwnd, UM_STOPPRINT, 0L, 0L);
               }                       /* end case (DID_OK)                   */
               break;

            default:                   /* Let PM handle rest of messages      */
               mr = WinDefDlgProc( hwnd, msg, mp1, mp2 );
               break;
           }
        break;

     default:                          /* Let PM handle rest of messages      */
        mr = WinDefDlgProc( hwnd, msg, mp1, mp2 );
        break;
    }

 return( mr );

}

/******************************************************************************/
/*                                                                            */
/* fnProcessPrint: Main print process. Just display Print dialog box.         */
/*                                                                            */
/******************************************************************************/

VOID fnProcessPrint( HWND hwnd, LONG page )
{

  WinLoadDlg(HWND_DESKTOP, 0L, wpPrtStatDlgProc, 0L, IDD_PRINTSTATUS,
             (PVOID)&page);

}

/******************************************************************************/
/*                                                                            */
/* fnPrint: Print thread processing.                                          */
/*                                                                            */
/******************************************************************************/

LONG fnPrint( LONG page )
{

  CHAR         achDriverName[DRIVERNAME_LENGTH];
  CHAR         achDeviceName[DEVICENAME_LENGTH];
  LONG         i;
  LONG         l;
  DEVOPENSTRUC dos;
  HDC          hdc;
  HPS          hps;
  HMQ          hmq;
  APIRET       rc;
  HAB          habPrn;

  if(debug)
    fprintf(wptrDebug, "Begin thread fnPrint().\n");

  habPrn = WinInitialize (0);          /* initialization procedures, PM and   */
  hmq = WinCreateMsgQueue (habPrn, 0); /* application message queue           */

  i = strcspn(pbd->prq->pszDriverName, ".");
  if (i)
  {
    strncpy(achDriverName, pbd->prq->pszDriverName, i);
    achDriverName[i] = '\0';
    strcpy(achDeviceName, &pbd->prq->pszDriverName[i + 1]);
  }
  else
  {
    strcpy(achDriverName, pbd->prq->pszDriverName);
    *achDeviceName = '\0';
  }

  /* Build the device context data for DevOpenDC */
  memset((PVOID)&dos, 0, sizeof(dos));
  dos.pszLogAddress = pbd->prq->pszName;
  dos.pszDriverName = (PSZ)achDriverName;
  dos.pdriv = pbd->pDriverData;
  dos.pszDataType = (PSZ)"PM_Q_STD";
  dos.pszComment = "Bingo for OS/2";
  dos.pszQueueProcName   = (PSZ)NULL;
  dos.pszQueueProcParams = "XFM=0";
  dos.pszSpoolerParams = (PSZ)NULL;
  dos.pszNetworkParams = (PSZ)NULL;

  /* Create DC for the printer */
  hdc = DevOpenDC( habPrn, OD_QUEUED, "*", 9L, (PDEVOPENDATA)&dos, (HDC)NULLHANDLE );

  /* Create PS for the printer */
  if (hdc != DEV_ERROR)
  {
    SIZEL sizel;

    sizel.cx = 0;
    sizel.cy = 0;
    hps = GpiCreatePS(habPrn, hdc, &sizel, PU_ARBITRARY | GPIA_ASSOC );

    if (hps != GPI_ERROR)
    {
      POINTL    point;
      APIRET    rc;
      PCARDLIST item;
//    HCINFO    hcInfo[MAX_PRINT_FORMS];
//
//    /* Determine the hard copy capabilities of the printer, set width and*/
//    /* height to pass to fnDrawCard routine.                             */
//    l = DevQueryHardcopyCaps(hdc, 0L, i, hcInfo);
//    l = 0;
//    while((hcInfo[l].flAttributes != HCAPS_CURRENT) && l < MAX_PRINT_FORMS)
//      l++;
      GpiQueryPS(hps, &sizel);

      /* Issue STARTDOC to begin printing */
      DevEscape( hdc, DEVESC_STARTDOC, 8L, (PBYTE)"Bingo\0",
                 (PLONG)NULL, (PBYTE)NULL );

      /* Print the bingo cards */
      item = start;
      while(item)
      {
        WinSendMsg(pbd->hwndPrint, UM_STATUS, MPFROMLONG(item), 0L);

        if(page)                       /* Option: 1 or 4 cards per page       */
        {
          /* Print 4 cards per page, first print card 1, top left */
          fnDrawCard(hps, item->pcd, 0, sizel.cy/2, sizel.cx/2, sizel.cy/2,
                     FALSE);
          item = (PCARDLIST) item->nextcard;
          if(item)
          {
            /* Print card 2, top right */
            WinSendMsg(pbd->hwndPrint, UM_STATUS, MPFROMLONG(item), 0L);
            fnDrawCard(hps, item->pcd, sizel.cx/2, sizel.cy/2,
                       sizel.cx/2, sizel.cy/2, FALSE);
            item = (PCARDLIST) item->nextcard;
            if(item)
            {
              /* Print card 3, bottom left */
              WinSendMsg(pbd->hwndPrint, UM_STATUS, MPFROMLONG(item), 0L);
              fnDrawCard(hps, item->pcd, 0, 0,
                         sizel.cx/2, sizel.cy/2, FALSE);
              item = (PCARDLIST) item->nextcard;
              if(item)
              {
                /* Print card 4, bottom right */
                WinSendMsg(pbd->hwndPrint, UM_STATUS, MPFROMLONG(item), 0L);
                fnDrawCard(hps, item->pcd, sizel.cx/2, 0,
                           sizel.cx/2, sizel.cy/2, FALSE);
              }
            }
          }
        }
        else
          /* Print one card per page, ok simple */
          fnDrawCard(hps, item->pcd, 0, 0, sizel.cx, sizel.cy, FALSE);

        if(debug)
          fprintf(wptrDebug, "Printing bingo card %d.\n", item->pcd);

        if(item)
          item = (PCARDLIST) item->nextcard;

        DevEscape( hdc, DEVESC_NEWFRAME, 0L, (PBYTE)NULL,
                   (PLONG)NULL, (PBYTE)NULL );
      }

      /* Issue ABORTDOC if user cancelled print job, or ENDDOC */
      /* to for normal job termination.                        */
      DevEscape( hdc, DEVESC_ENDDOC, 0L, (PBYTE)NULL,
                 (PLONG)NULL, (PBYTE)NULL );

      /* Release and destroy the PS */
      GpiAssociate( hps, (HDC)NULLHANDLE );
      GpiDestroyPS( hps );
    }
    else
    {
      CHAR title[21];
      CHAR text[81];
      CHAR szError[200];

      WinLoadString (hab, 0L, IDS_PRINTERROR, 20, title );
      WinLoadString (hab, 0L, IDS_ERROR_PRINT_3, 80, text );
      sprintf(szError, "%s %d.", text, hps);

      WinMessageBox (HWND_DESKTOP, 0L, szError,
                     title, 1, MB_ERROR | MB_OK | MB_MOVEABLE);

      if(debug)
        fprintf(wptrDebug, "%s %s\n", title, szError);
    }
  }
  else
  {
    CHAR title[21];
    CHAR text[81];
    CHAR szError[200];

    WinLoadString (hab, 0L, IDS_PRINTERROR, 20, title );
    WinLoadString (hab, 0L, IDS_ERROR_PRINT_4, 80, text );
    sprintf(szError, "%s %d.", text, hdc);
    WinMessageBox (HWND_DESKTOP, 0L, szError,
                   title, 1, MB_ERROR | MB_OK | MB_MOVEABLE);
    if(debug)
      fprintf(wptrDebug, "%s\n", title, szError);
  }

  /* Clean up the DC */
  if (hdc != DEV_ERROR)
    DevCloseDC(hdc);

  /* Send message to Print dialog window that we are finished */
  WinSendMsg(pbd->hwndPrint, UM_STOPPRINT, 0L, 0L);

  WinDestroyMsgQueue (hmq);
  WinTerminate (habPrn);

  if(debug)
    fprintf(wptrDebug, "End thread fnPrint().\n");

  DosExit( EXIT_THREAD, 0L );

}
