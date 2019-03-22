/******************************************************************************/
/*                          (c) IBM Corp. 1994                                */
/*                                                                            */
/* Program: OS/2 PM Bingo2 - Sound Support                                    */
/*                                                                            */
/* Author : E. L. Zapanta                                                     */
/* Date   : 11Aug94                                                           */
/*                                                                            */
/******************************************************************************/

#define INCL_WIN                       /* OS/2 window procedures              */
#define INCL_BASE                      /* OS/2 control program (CP)           */
#define INCL_SPL
#define INCL_SPLDOSPRINT
#define INCL_OS2MM                     /* OS/2 multimedia                     */

#include <os2.h>
#include <string.h>
#include <stdio.h>
#include <os2me.h>                     /* OS/2 multimedia extensions          */
#include "bingowav.h"                  /* WAVE file definitions               */
#include "bingo.h"                     /* main include file                   */
#include "bingodlg.h"

#define AUDIO_WAIT  200      /* milliseconds, DosSleep() called by fnSound()  */

HWND  hwndSound;

ULONG fnAudioPlay( HMMIO wavefile, LONG mciOption );
ULONG fnGetWaveID( ULONG num );

MRESULT EXPENTRY wpSoundDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
#pragma linkage (wpSoundDlgProc, system)

/******************************************************************************/
/*                                                                            */
/* fnSoundThread: Main threaded sound routine.                                */
/*                                                                            */
/******************************************************************************/

VOID fnSoundThread( VOID )
{

 HAB  habSnd;
 HMQ  hmqSnd;
 QMSG qmsg;

 habSnd = WinInitialize (0);           /* initialization procedures, PM and   */
 hmqSnd = WinCreateMsgQueue (habSnd, 0); /* application message queue         */

 hwndSound = WinLoadDlg( HWND_DESKTOP, 0L, wpSoundDlgProc, 0L,
                         IDD_SOUND, NULL );

 if(debug)
   WinShowWindow(hwndSound, TRUE);

 /* Standard message loop. Get messages from the queue and dispatch them to   */
 /* the approprite windows.                                                   */

 while (WinGetMsg (habSnd, &qmsg, 0L, 0, 0))
    WinDispatchMsg (habSnd, &qmsg);

 /* Main loop has terminated. Destroy all windows, help instance and message  */
 /* queue and then terminate the application.                                 */

 WinDestroyWindow (hwndSound);
 WinDestroyMsgQueue (hmqSnd);
 WinTerminate (habSnd);

 DosExit (EXIT_THREAD, 0L);

}

/******************************************************************************/
/*                                                                            */
/* fnSound: Sound routine.                                                    */
/*                                                                            */
/******************************************************************************/

VOID fnSound( USHORT option, ULONG letter, ULONG number )
{

   PSND   pSoundData;

   /* Allocate memory for input, will be freed in fnSound() routine later */
   pSoundData = (PSND)malloc(sizeof(SND));
   pSoundData->option = option;
   pSoundData->letter = letter;
   pSoundData->number = number;

   /* Post message to Sound window (invisible) for processing */
   WinPostMsg( hwndSound, UM_SOUND, MPFROMLONG(pSoundData), 0L );

}


/******************************************************************************/
/*                                                                            */
/* wpSoundDlgProc: Window procedure for Sound dialog window. This window      */
/*                 is not normally shown unless Bingo/2 started with debug.   */
/*                                                                            */
/******************************************************************************/

MRESULT EXPENTRY wpSoundDlgProc (HWND hwnd, ULONG msg, MPARAM mp1,
                                 MPARAM mp2)

{

 MRESULT mr = (MRESULT) FALSE;

 switch (msg)
    {
     case WM_INITDLG:                  /* Dialog box just created. Initialize */
        break;                         /* end case (WM_INITDLG)               */

     case UM_SOUND:
        {
          PSND pSnd;

          pSnd = (PSND)LONGFROMMP(mp1);

          if(debug)
          {
            CHAR szSound[TEXTSIZE];

            sprintf(szSound, "Option %d, number %d, letter %d",
                    pSnd->option, pSnd->number, pSnd->letter);
            WinSendDlgItemMsg( hwnd, IDL_SOUND, LM_INSERTITEM,
                               MPFROMLONG( LIT_END ),
                               MPFROMP( szSound ) );
          }

          fnSoundProcess( pSnd );

        }
        break;

     case MM_MCINOTIFY:
       break;

     case WM_COMMAND:
        switch (SHORT1FROMMP(mp1))
           {
            case DID_CANCEL:           /* User clicked on Cancel push button  */
               WinDismissDlg( hwnd, FALSE );
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
/* fnSoundProcess: Sound processing.                                          */
/*                                                                            */
/******************************************************************************/

LONG fnSoundProcess( PSND pSoundData )
{

  USHORT option;
  ULONG  letter;
  ULONG  number;

  option = pSoundData->option;
  letter = pSoundData->letter;
  number = pSoundData->number;

  if(debug)
    fprintf(wptrDebug, "fnSoundProcess() %d %d %d.\n", option, letter, number);

  switch( option )
  {
    case( SOUND_CHIP ):
      {
//      if(audiotype == AUDIO_CARD && audioDevId)
//        fnAudioPlay( hmmioWave[ID_WAVE_POP], MCI_WAIT );
      }
      break;

    case( SOUND_ERROR ):
      {
        if(audiotype == AUDIO_CARD && audioDevId)
          fnAudioPlay( hmmioWave[ID_WAVE_TWIP], MCI_WAIT );
        else
          DosBeep( 100, 100 );
      }
      break;

    case( SOUND_BINGO ):
      {
        if(audiotype == AUDIO_CARD && audioDevId)
        {
          fnAudioPlay( hmmioWave[ID_WAVE_BINGO], MCI_WAIT );
          fnAudioPlay( hmmioWave[ID_WAVE_APPLAUSE], MCI_NOTIFY );
        }
        else
        {
          DosBeep( 200, 200 );
          DosBeep( 200, 200 );
          DosBeep( 200, 200 );
        }
      }
      break;

    case( SOUND_NOBINGO ):
      {
        if(audiotype == AUDIO_CARD && audioDevId)
        {
          fnAudioPlay( hmmioWave[ID_WAVE_NOBINGO], MCI_WAIT );
          fnAudioPlay( hmmioWave[ID_WAVE_BOO], MCI_NOTIFY );
        }
        else
        {
          DosBeep( 200, 200 );
          DosBeep( 200, 200 );
          DosBeep( 200, 200 );
        }
      }
      break;

    case( SOUND_DRAWNUM ):
      {
        if(audiotype == AUDIO_CARD && audioDevId)
        {

          switch(letter)
          {
            case(0):
              fnAudioPlay( hmmioWave[ID_WAVE_B], MCI_WAIT );
              break;
            case(1):
              fnAudioPlay( hmmioWave[ID_WAVE_I], MCI_WAIT );
              break;
            case(2):
              fnAudioPlay( hmmioWave[ID_WAVE_N], MCI_WAIT );
              break;
            case(3):
              fnAudioPlay( hmmioWave[ID_WAVE_G], MCI_WAIT );
              break;
            case(4):
              fnAudioPlay( hmmioWave[ID_WAVE_O], MCI_WAIT );
              break;
            default:
              break;
          }

          DosSleep(AUDIO_WAIT);
          if(number < 21)
            fnAudioPlay( hmmioWave[number], MCI_NOTIFY );
          else
          {
            /* Compound number, say 30, 40, 50, 60 or 70 (highnum) first */
            /* then, say the single (lownum) digit next                  */
            ULONG highnum;
            ULONG lownum;
            ULONG playnum;
            ULONG base = 10;

            highnum = (number/base) * 10;
            playnum = fnGetWaveID(highnum);
            lownum  = number % base;

            if(playnum > 0 && playnum > 0)
              fnAudioPlay( hmmioWave[playnum], MCI_WAIT );
            else
              fnAudioPlay( hmmioWave[playnum], MCI_NOTIFY );

            /* Delay, seem to need it otherwise MCI_PLAY hangs */
            /* between the two numbers                         */
            DosSleep(AUDIO_WAIT);

            if(lownum > 0)
              fnAudioPlay( hmmioWave[lownum], MCI_NOTIFY );
          }
        }
        else
          DosBeep( 400, 400 );
      }
      break;

    case( SOUND_NEWCARDS ):
      {
        if(audiotype == AUDIO_CARD && audioDevId)
          fnAudioPlay( hmmioWave[ID_WAVE_NEWCARDS], MCI_NOTIFY );
      }
      break;

    case( SOUND_NEWGAME ):
      {
        if(audiotype == AUDIO_CARD && audioDevId)
          fnAudioPlay( hmmioWave[ID_WAVE_NEWGAME], MCI_NOTIFY );
      }
      break;

    case( SOUND_WELCOME ):
      {
        if(audiotype == AUDIO_CARD && audioDevId)
          fnAudioPlay( hmmioWave[ID_WAVE_ABOUT], MCI_NOTIFY );
      }
      break;

    case( SOUND_EXIT ):
      {
        if(audiotype == AUDIO_CARD && audioDevId)
          fnAudioPlay( hmmioWave[ID_WAVE_THANKYOU], MCI_WAIT );
      }
      break;

    case( SOUND_QUIT ):
      {
        if(audiotype == AUDIO_CARD && audioDevId)
          fnAudioPlay( hmmioWave[ID_WAVE_QUIT], MCI_NOTIFY );
      }
      break;

    case( SOUND_REGISTER ):
      {
        if(audiotype == AUDIO_CARD && audioDevId)
          fnAudioPlay( hmmioWave[ID_WAVE_REGISTER], MCI_NOTIFY );
      }
      break;

    /* Chatter */
    case( SOUND_CLOSE ):
      {
        if(audiotype == AUDIO_CARD && audioDevId && chatter)
          fnAudioPlay( hmmioWave[ID_WAVE_CLOSE], MCI_NOTIFY );
      }
      break;

    case( SOUND_WAIT ):
      {
        if(audiotype == AUDIO_CARD && audioDevId && chatter)
          fnAudioPlay( hmmioWave[ID_WAVE_WAIT], MCI_NOTIFY );
      }
      break;

    case( SOUND_NUMBER ):
      {
        if(audiotype == AUDIO_CARD && audioDevId && chatter)
        {
          ULONG playnum;

          playnum = fnGetWaveID(letter);
          if(playnum > 0)
            fnAudioPlay( hmmioWave[playnum], MCI_WAIT );
          fnAudioPlay( hmmioWave[ID_WAVE_NUMBER], MCI_NOTIFY );
        }
      }
      break;

    case( SOUND_LETTER ):
      {
        if(audiotype == AUDIO_CARD && audioDevId && chatter)
        {
          fnAudioPlay( hmmioWave[ID_WAVE_LETTER], MCI_WAIT );
          switch(letter)
          {
            case(0):
              fnAudioPlay( hmmioWave[ID_WAVE_B], MCI_NOTIFY );
              break;
            case(1):
              fnAudioPlay( hmmioWave[ID_WAVE_I], MCI_NOTIFY );
              break;
            case(2):
              fnAudioPlay( hmmioWave[ID_WAVE_N], MCI_NOTIFY );
              break;
            case(3):
              fnAudioPlay( hmmioWave[ID_WAVE_G], MCI_NOTIFY );
              break;
            case(4):
              fnAudioPlay( hmmioWave[ID_WAVE_O], MCI_NOTIFY );
              break;
            default:
              break;
          }
        }
      }
      break;

    default:
      DosBeep( 100, 100 );
      break;
  }

  free(pSoundData);

  return(0);

}


/******************************************************************************/
/*                                                                            */
/* fnGetWaveID: Return the resource id of the wave file.                      */
/*                                                                            */
/******************************************************************************/

ULONG fnGetWaveID( ULONG num )
{
  ULONG id;

  switch( num )
  {
    case( 20 ):
      id = ID_WAVE_20;
      break;
    case( 30 ):
      id = ID_WAVE_30;
      break;
    case( 40 ):
      id = ID_WAVE_40;
      break;
    case( 50 ):
      id = ID_WAVE_50;
      break;
    case( 60 ):
      id = ID_WAVE_60;
      break;
    case( 70 ):
      id = ID_WAVE_70;
      break;
    default:
      id = 0;
  }
  return( id );
}


/******************************************************************************/
/*                                                                            */
/* fnAudioCardInit: Initialize audio device.                                  */
/*                                                                            */
/******************************************************************************/

VOID fnAudioCardInit( HWND hwnd )

{

  MCI_OPEN_PARMS   mciOpenParms;
  ULONG            ulError;

  memset(&mciOpenParms, '\0', sizeof(MCI_OPEN_PARMS));
//mciOpenParms.hwndCallback   = hwndBoard;
  mciOpenParms.pszDeviceType  = "waveaudio";

  ulError = (*mciSendCmd)( 0, MCI_OPEN,
                           MCI_WAIT | MCI_OPEN_SHAREABLE,
                           &mciOpenParms, 0);
  if(ulError)
  {
    CHAR szError[TEXTSIZE+AUDIOERRSIZE+1];
    CHAR szErrStr[AUDIOERRSIZE+1];

    WinLoadString( hab, 0L, IDS_AUDIOERROR,  TITLESIZE, szTitle);
    WinLoadString( hab, 0L, IDS_AUDIOOPENERR, TEXTSIZE, szText);

    audioDevId = 0;

    (*mciGetError)(ulError, szErrStr, AUDIOERRSIZE);
    sprintf(szError, "%s %d. %s", szText, ulError, szErrStr);
    WinMessageBox (HWND_DESKTOP, hwnd, szError, szTitle, 1,
                   MB_MOVEABLE | MB_OK | MB_ERROR);
  }
  else
    audioDevId = mciOpenParms.usDeviceID;
}

/******************************************************************************/
/*                                                                            */
/* fnAudioPlay: Play audio wave file.                                         */
/*                                                                            */
/******************************************************************************/

ULONG fnAudioPlay( HMMIO wavefile, LONG mciOption )
{
  ULONG            ulError = 0;
  APIRET           rc;
  CHAR             szChat[CHATSIZE];
  CHAR             szError[200];
  CHAR             szErrStr[128];

  MCI_LOAD_PARMS    mciLoadParms;
  MCI_PLAY_PARMS    mciPlayParms;
  MCI_GENERIC_PARMS mciGenericParms;

  memset(&mciLoadParms,    '\0', sizeof(MCI_LOAD_PARMS));
  memset(&mciPlayParms,    '\0', sizeof(MCI_PLAY_PARMS));
  memset(&mciGenericParms, '\0', sizeof(MCI_GENERIC_PARMS));

  /* Load the wave file */
  mciLoadParms.pszElementName = (PSZ)wavefile;
  ulError = (*mciSendCmd)( audioDevId,
                           MCI_LOAD,
                           MCI_WAIT | MCI_OPEN_MMIO | MCI_READONLY,
                           &mciLoadParms, 0);
  if(ulError)
  {
    if(ulError == 70570 || ulError == 201642)
    {
      ulError = (*mciSendCmd)( audioDevId,
                               MCI_ACQUIREDEVICE,
                               MCI_WAIT,
                               &mciGenericParms, 0);
    }
    else
    {
      WinLoadString( hab, 0L, IDS_AUDIOERROR,  20, szTitle);
      WinLoadString( hab, 0L, IDS_AUDIOLOADERR, 80, szText);

      (*mciGetError)(ulError, szErrStr, 128);
      sprintf(szError, "%s %d. %s", szText, ulError, szErrStr);
    }
  }

  if(debug)
  {
    fprintf(wptrDebug, "fnAudioPlay() <load> of %d.", ulError);
    if(ulError)
      fprintf(wptrDebug, "%s.", szErrStr);
    fprintf(wptrDebug, "\n");
  }

  /* Play the wave file */
  mciPlayParms.ulFrom = 0L;
  mciPlayParms.hwndCallback = hwndSound;
  ulError = (*mciSendCmd)( audioDevId,
                           MCI_PLAY,
                           mciOption | MCI_FROM,
                           &mciPlayParms, 0);
//if(ulError)
  {
//
//  if(ulError == 70570 || ulError == 201642)
//  {
//    ulError = (*mciSendCmd)( audioDevId,
//                             MCI_ACQUIREDEVICE,
//                             MCI_WAIT,
//                             &mciGenericParms, 0);
//  }
//  else
//  {
      WinLoadString( hab, 0L, IDS_AUDIOERROR,  20, szTitle);
      WinLoadString( hab, 0L, IDS_AUDIOPLAYERR, 80, szText);

      (*mciGetError)(ulError, szErrStr, 128);
      sprintf(szError, "%s %d. %s", szText, ulError, szErrStr);
//    WinMessageBox (HWND_DESKTOP, hwndBoard, szError, szTitle, 1,
//                   MB_MOVEABLE | MB_OK | MB_ERROR);
//  }
  }

  if(debug)
  {
    fprintf(wptrDebug, "fnAudioPlay() <play> of %d.", ulError);
    if(ulError)
      fprintf(wptrDebug, "%s.", szErrStr);
    fprintf(wptrDebug, "\n");
  }

  return( ulError );
}

/******************************************************************************/
/*                                                                            */
/* fnAudioCardClose: Close the audio device.                                  */
/*                                                                            */
/******************************************************************************/

VOID fnAudioCardClose( VOID )

{

  (*mciSendCmd)(audioDevId, MCI_CLOSE, MCI_WAIT, 0, 0);

}

/******************************************************************************/
/*                                                                            */
/* fnLoadModules: Load the MMPM/2 DLLs (MDM.DLL and MMIO.DLL) and the         */
/*                resource DLL containing the .WAV files (BINGOWAV.DLL) at    */
/*                run time. Then determine the address of the functions and   */
/*                the resource. Finally, open all the .WAV files using MMIO   */
/*                and save the MMIO handles (needed to play .WAV file).       */
/*                                                                            */
/******************************************************************************/

VOID fnLoadAudioModules( VOID )
{

  CHAR szBuf[BUFFERSIZE];

  os2mm = DosLoadModule(szBuf, sizeof(szBuf), "MDM", &hModMDM);
  if(!os2mm)
  {                                    /* MMPM/2 installed, query func addr   */
    DosQueryProcAddr(hModMDM, 0L, "mciSendCommand", &mciSendCmd);
    DosQueryProcAddr(hModMDM, 0L, "mciGetErrorString", &mciGetError);
    os2mm = DosLoadModule(szBuf, sizeof(szBuf), "MMIO", &hModMMIO);
    if(!os2mm)
    {                                  /* MMPM/2 installed, query func addr   */
      DosQueryProcAddr(hModMMIO, 0L, "mmioOpen", &ioOpen);
      DosQueryProcAddr(hModMMIO, 0L, "mmioClose", &ioClose);
      os2mm = 1;
      os2wave = DosLoadModule(szBuf, sizeof(szBuf), "BINGOWAV", &hModWAVE);
      if(!os2wave)
      {
        ULONG    i;
        PVOID    pWave;
        MMIOINFO mmioInfo;
        ULONG    size;
        FOURCC   fccIOProc;
        APIRET   rc;

        fccIOProc = mmioFOURCC ('W','A','V','E');

        for(i = 1; i < TOTAL_WAVES; i++)
        {
          rc = DosGetResource(hModWAVE, RT_RIFF, ID_WAVE+i, &pWave);

          if(!rc)
          {
            memset(&mmioInfo, '\0', sizeof(MMIOINFO));
            mmioInfo.fccChildIOProc  = FOURCC_MEM;
            mmioInfo.fccIOProc  = fccIOProc;
            mmioInfo.pchBuffer = (PCHAR)pWave;
            DosQueryResourceSize(hModWAVE, RT_RIFF, ID_WAVE+i, &size);
            mmioInfo.cchBuffer = size;
            hmmioWave[i] = (*ioOpen)(NULL, &mmioInfo,
                           MMIO_READ | MMIO_DENYNONE);
            if(debug)
              fprintf(wptrDebug, "mmioOpen HHMIO %d of %d with size of %d.\n",
                                 hmmioWave[i], ID_WAVE+i, size);
          }
          else
          {
            /* error loading WAVE file */
          }
        }
        os2wave = 1;
      }
      else
      {
        if(debug)
          fprintf(wptrDebug, "DosLoadModule of BINGOWAV.DLL failed with %d.\n",
                              os2wave);
        os2wave = 0;                   /* Cannot load BINGOWAV.DLL            */

      }
    }
    else
    {
      if(debug)
        fprintf(wptrDebug, "DosLoadModule of MMIO.DLL failed with %d.\n",
                           os2mm);
      os2mm = 0;                       /* MMPM/2 not installed, MMIO.DLL      */
    }
  }
  else
  {
    if(debug)
      fprintf(wptrDebug, "DosLoadModule of MDM.DLL failed with %d.\n",
                           os2mm);
    os2mm = 0;                         /* MMPM/2 not installed, MDM.DLL       */
  }

}


/******************************************************************************/
/*                                                                            */
/* fnFreeAudioModules: Free all audio DLLs and close sound files.             */
/*                                                                            */
/******************************************************************************/

VOID fnFreeAudioModules( VOID )

{

 if(audiotype == AUDIO_CARD)           /* Close audio card device             */
 fnAudioCardClose( );
 if(os2mm)
 {
   DosFreeModule(hModMDM);
   DosFreeModule(hModMMIO);
 }
 if(os2wave)
 {
   ULONG i;

   for(i=1; i<TOTAL_WAVES+1; i++)
     (*ioClose)(hmmioWave[i], 0);

   DosFreeModule(hModWAVE);
 }

}
