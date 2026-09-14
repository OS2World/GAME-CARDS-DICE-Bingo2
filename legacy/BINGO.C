/******************************************************************************/
/*                          (c) IBM Corp. 1993                                */
/*                                                                            */
/* Program: Bingo/2 (Bingo for OS/2 Presentation Manager)                     */
/*                                                                            */
/* Author : E. L. Zapanta                                                     */
/* Date   : 19Dec92                                                           */
/* Version: 1.4                                                               */
/* Compile: OS/2 2.x C Set/2 Version 1.0                                      */
/*          OS/2 Developer's Toolkit 2.1                                      */
/*          OS/2 MMPM/2 Toolkit 2.1                                           */
/*                                                                            */
/* History: 04Apr93  ELZ   First release.                                     */
/*                                                                            */
/*          01Oct93  1.1   Add new status boxes to right of Bingo board:      */
/*                           - Bingo number drawn                             */
/*                           - Running game time                              */
/*                           - Game type graphical bitmap                     */
/*                         Add new option to show/not show Game options at    */
/*                           startup                                          */
/*                         Rename Menu to File, Options to Games              */
/*                         New menu called Options, Sound and Mark All as     */
/*                           pulldown choices                                 */
/*                                                                            */
/*          28Jan94  1.2   Digitized sound support using MMPM/2.              */
/*                         Fix bug when clicking on Free space (caused trap). */
/*                                                                            */
/*          20Jul94  1.21  New option to allow no prompt for new game.        */
/*                         Use snazzy dialog box instead of message box for   */
/*                           bingo, automatically remove if Ok not pressed.   */
/*                         Mark numbers not manually marked by player, use    */
/*                           transparent marker (UM_MISSMARK)                 */
/*                                                                            */
/*          25Jul94  1.22  Load the MDM.DLL (MMPM/2 support) at run-time.     */
/*                         Display only one Bingo! dialog instead of multiple */
/*                           ones, use listbox to display winning cards.      */
/*                                                                            */
/*          30Jul94  1.3   Add more games (Lucky 7, Hardaway)                 */
/*                         Use slider instead of spin button for Draw Time    */
/*                           in Settings dialog window.                       */
/*                         Use special marker (semi-transparent) to indicate  */
/*                           bingo squares in winning bingo cards.            */
/*                         Customize mouse pointer for bingo cards (value set */
/*                           in Settings).                                    */
/*                         Customize font.                                    */
/*                         Support to print bingo cards.                      */
/*                         Fix problem when last bingo number is drawn and no */
/*                            when no bingo found.                            */
/*                                                                            */
/*          08Aug94  1.4   Bingo hall simulation including:                   */
/*                           - Other bingo players, add Other Cards under     */
/*                               Options menu                                 */
/*                           - Prize money, player starts with $100, add to   */
/*                               Score dialog window                          */
/*                           - Chatter Box, add Chatter under Options menu    */
/*                         Debugging support, invoke using /D=1 parameter     */
/*                         Improve/update help                                */
/*                         Change the bingo card title to "Card No ##"        */
/*                         Package .WAV files into a single resource DLL      */
/*                                                                            */
/*          16Aug94  1.41  Place sound processing in separate thread.         */
/*                         Support printing of 4 cards per page (option)      */
/*                         Fix bug - Board/cards not initialized after print  */
/*                         Fix bug - Bad hotspot for pencil and marker mouse  */
/*                                     pointers                               */
/*                         High Scores (under File menu) support              */
/*                         Automatically start game when Bingo/2 invoked.     */
/*                         Convert sound file from 16-bit to 8-bit.           */
/*                                                                            */
/*          23Aug94  1.42  Fix bug - Bingo sounds after quitting game.        */
/*                         Fix bug - Draw time not changed in Settings.       */
/*                         Save all Game options, # of cards and players, etc.*/
/*                         Remove computer player, combined with hall players.*/
/*                         All/None push buttons in Other Cards dialog window.*/
/*                         Use entry fields in Score dialog window.           */
/*                         Support vertical bingo board, option in Settings   */
/*                                                                            */
/*          28Aug94  1.43  Fix bug - Selecting bottom color row in Settings   */
/*                           selects default bingo pointer.                   */
/*                         Fix bug - Missed square mark results in incorrect  */
/*                           bingo. Bug in fnCheckBingo() routine.            */
/*                         3D effects for bingo card and bingo board.         */
/*                                                                            */
/*          31Aug94  1.44  Re-acquire shared audio device (MCI_ACQUIREDEVICE) */
/*                           if lost (RC=70570 or 201642).                    */
/*                                                                            */
/******************************************************************************/
/******************************************************************************/
/*                               Notice                                       */
/******************************************************************************/
/*                                                                            */
/* IMPORTANT NOTE: To add new bingo game types, change the total number of    */
/* bingo game types in the #define BINGOGAMES statement found in BINGO.H.     */
/* Next add the name and marker squares in fnCreateBingoGames. Finally,       */
/* create a bitmap.                                                           */
/*                                                                            */
/* If pdb->bingogames[i][x][y] = 1 then this square must be marked in the     */
/* bingo card in order to get a bingo.                                        */
/*                                                                            */
/******************************************************************************/
/******************************************************************************/
/*                           Global Variables                                 */
/******************************************************************************/
/*                                                                            */
/*  LONG   bingonumber           Number drawn (see also bingoletter)          */
/*  USHORT bingoletter           Letter of number drawn (0=B, 1=I, 2=N, etc.) */
/*  USHORT bingostatus           Status of game (PREGAME,INPROGRESS,GAMEOVER) */
/*                                                                            */
/*  BOOL   autoplay              Manual play (0) or automated play (1=ON)     */
/*  ULONG  numberofcards         Number of bingo cards played (1-xx)          */
/*  ULONG  numberofplayers       Number of other virtual bingo players (0+)   */
/*  ULONG  prizemoney            Prize money ($, use CARD_COST)               */
/*  ULONG  gametype              Type of bingo game (1-x)                     */
/*  ULONG  drawtime              Time between number drawn (milliseconds)     */
/*  BOOL   markall               Mark all (0=OFF, 1=ON)                       */
/*  BOOL   sound                 Sound (0=OFF, 1=ON)                          */
/*  BOOL   chatter               Chatter (0=OFF, 1=ON)                        */
/*  BOOL   audiotype             Audio type (0=PC speaker, 1=Audio card)      */
/*  ULONG  newcards              Prompt for new cards after game (0=NO, 1=YES)*/
/*  ULONG  chipcolor             Color of bingo chip (0-15)                   */
/*  BOOL   showstart             Show Options dialog at start (0=NO, 1=YES)   */
/*  BOOL   gameprompt            Prompt for new game (0=NO, 1=YES)            */
/*  ULONG  pointer               Pointer number (0-n)                         */
/*  BOOL   boardtype             Board orientation (0=VERTICAL, 1=HORIZONTAL) */
/*  BOOL   effects3d             3D effects (0=OFF, 1=ON)                     */
/*                                                                            */
/*  PBOARDDATA    pbd            Pointer to bingo board data structure        */
/*  PSCOREDATA    pcd            Pointer to score data structure              */
/*  PHISCORESDATA phsd           Pointer to high scores data structure        */
/*  PCARDLIST     start, last    Linked list starting and ending addresses    */
/*  HWND          hwndBoard      Handle of Bingo Board client window          */
/*  HWND          hwndChat       Handle of Chatter dialog window              */
/*  HWND          hwndHelp       Handle of help window                        */
/*                                                                            */
/******************************************************************************/

#define INCL_WIN                       /* OS/2 window procedures              */
#define INCL_BASE                      /* OS/2 control program (CP)           */
#define INCL_GPI                       /* OS/2 graphics programming interface */
#define INCL_OS2MM                     /* OS/2 multimedia                     */
#define INCL_SPL
#define INCL_SPLDOSPRINT

#include <os2.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <os2me.h>                     /* OS/2 multimedia extensions          */
#include "bingowav.h"                  /* WAVE file definitions               */
#include "bingo.h"                     /* main include file                   */
#include "bingodlg.h"                  /* include file for dialog windows     */

VOID  fnChatDrawNumber( LONG bingoNumber, USHORT bingoLetter,
                        LONG numbersleft );
VOID  fnChatProcess( LONG chatType, LONG number, LONG lp1, LONG lp2 );
VOID  fnDrawBingoNumber( HWND hwnd );
ULONG fnGetNextNumber( VOID );
VOID  fnGetCardNumbers( PCARDDATA pcd, SHORT newcard );
VOID  fnGetBoardNumbers( VOID );
VOID  fnDrawBoard( HWND hwnd );
VOID  fnPositionBoard( HWND hwnd );
VOID  fnGetBoardSizes( RECTL rect, LONG *xspace, LONG *yspace, LONG *xl,
                       LONG *yl );
VOID  fnDrawBitmap( HPS hps, RECTL rect, LONG xspace, LONG yspace, LONG xl,
                    LONG yl, PRECTL outrect );
VOID  fnDrawStatusLine( HAB hab, HWND hwnd );
VOID  fnUpdateScores( BOOL type );
VOID  fnGetXYValues( USHORT num, USHORT *x, USHORT *y );
VOID  fnProcessBingoSquare( HWND hwnd, PCARDDATA pcd, LONG x, LONG y );
LONG  fnCheckBingo( PCARDDATA pcd );
VOID  fnDisplayBingo( HWND hwnd, BOOL bFound );
USHORT fnCheckCard( PCARDDATA pcd, USHORT x, LONG number );
VOID  fnCreateBingoGames( VOID );
VOID  fnFreeCardList( PCARDLIST startItem, PCARDLIST lastItem );
VOID  fnSound( USHORT option, ULONG letter, ULONG number );
CHAR  fnGetBingoLetter( ULONG number );
CHAR  fnGetBingoLetterFromBingoNumber( ULONG number );
VOID  fnConvertTime( ULONG timer, CHAR *szOuttime );
VOID  fnFindAllFonts(HWND hwnd);
VOID  fnFindFont(HWND hwnd, CHAR *szFontname);
BOOL  fnHighScores( VOID );
BOOL  fnRedrawWindows( HWND hwnd );

time_t startime;                       /* start time of game                  */
ULONG  gametime;                       /* length of game played in seconds    */
ULONG  startbox;                       /* flag to show New Game? message box  */
BOOL   boolTime;                       /* flag to avoid ID_TIMER processing   */

int main( int argc, char *argv[] )     /* main routine, optional arguments    */
                                       /* passed are: /D and /N=xxxxx         */
{

 TID           threadID;               /* Thread id for Sound thread          */
 HMQ           hmq;                    /* message queue handle                */
 QMSG          qmsg;                   /* PM message                          */
 HWND          hwndFrame;              /* various window handles              */
 HELPINIT      hiMain;                 /* help initialization structure       */
 RECTL         rectlDesktop;           /* rectangle structure                 */
 SWP           swp;                    /* set window position structure       */
 ULONG         ulSize;                 /* size of SWP structure               */
 LONG          x, y, cx, cy;           /* window position coordinates and size*/
 LONG          lLogoDisplay;           /* number of seconds to display logo   */
 PROFILEINFO   setinfo;
 HISCORESDATA  hsData;
 ULONG         flFrameFlags = FCF_TITLEBAR      | /* frame creation flags       */
                              FCF_SYSMENU       |
                              FCF_SIZEBORDER    |
                              FCF_MINMAX        |
                              FCF_SHELLPOSITION |
                              FCF_TASKLIST      |
                              FCF_MENU          |
                              FCF_ICON          |
                              FCF_ACCELTABLE;

 hab = WinInitialize (0);              /* Initialization procedures, PM and   */
 hmq = WinCreateMsgQueue (hab, 0);     /* application message queue           */

 /* Process arguments passed, either /D for debug or /N for network play      */
 /* Set global variable flags, debug and networkPlay, to ON or OFF            */

 debug       = OFF;
 networkPlay = OFF;
 if( argc > 1 )
 {
   for( x=1; x < argc; x++ )
   {
     strcpy( szText, argv[x]);         /* copy argument to work string        */
     fnToUpperString( szText );        /* convert to upper case               */

     if( szText[0] == '/' && szText[2] == '=' )
     {
       switch( szText[1] )
       {
         case('D'):
           debug = szText[3];
           break;
         case('N'):
           networkPlay = ON;
           break;
         default:
           break;
       }                               /* end switch                          */
     }
   }                                   /* end for( x=1; ....)                 */
 }                                     /* end if argc > 1                     */

 /* Debugging /D=1 parameter. Open debug file in write mode and place the     */
 /* Bingo/2 version number first.                                             */
 if(debug)
 {
   wptrDebug = fopen(szDebugFile, "w");
   WinLoadString( hab, 0L, IDS_VERSION, TEXTSIZE, szText);
   fprintf(wptrDebug, "Bingo/2 %s\n", szText);
 }
 WinLoadString( hab, 0L, IDS_MONEY, MONEYSIZE, szMoney );

 /* Allocate the memory for the two global variables containing bingo board   */
 /* and score data.                                                           */
 pbd  = (PBOARDDATA)    malloc ( sizeof( BOARDDATA ) );
 psd  = (PSCOREDATA)    malloc ( sizeof( SCOREDATA ) );
 phsd = (PHISCORESDATA) malloc ( sizeof( HISCORESDATA ) );
 if( !pbd || !psd || !phsd )
 {
   WinLoadString( hab, 0L, IDS_PROFILEERROR, TITLESIZE, szTitle );
   WinLoadString( hab, 0L, IDS_ERROR_MEMORY, TEXTSIZE, szText );
   WinMessageBox( HWND_DESKTOP, 0L, szText, szTitle,
                  1, MB_ERROR | MB_OK | MB_MOVEABLE );
 }

 fnLoadAudioModules( );

 DosCreateThread( &threadID, (PFNTHREAD)fnSoundThread, 0L, 0L, STACKSIZE );

 InitRandom( );                        /* Initialize (seed) randomizer        */
                                       /* Register class for client window    */
 WinRegisterClass( hab, szClientClass, wpClientWndProc,
                   CS_SIZEREDRAW, 0);
 WinRegisterClass( hab, szBingoClass, wpBingoWndProc,
                   CS_SIZEREDRAW, sizeof(PCARDDATA) );

 /* Get previous session information from the profile and perform and         */
 /* required application customization to restore previous environment        */

 fnSetDefaultOptions( );               /* default options values              */

 WinLoadString( hab, 0L, IDS_ININAME, TEXTSIZE, szText );
 pbd->hini = PrfOpenProfile( hab, szText );
 if( pbd->hini == 0L )
 {
   WinLoadString( hab, 0L, IDS_PROFILEERROR, TITLESIZE, szTitle );
   WinLoadString( hab, 0L, IDS_PROFILE_SETTING, TEXTSIZE, szText );
   WinMessageBox( HWND_DESKTOP, 0L, szText, szTitle,
                  1, MB_ERROR | MB_OK | MB_MOVEABLE );
 }

 ulSize = sizeof( PROFILEINFO );
 if( PrfQueryProfileData( pbd->hini, szAppName, szSettings, &setinfo,
     &ulSize ) )
 {
   /* Settings */
   sound           = setinfo.bool1;
   markall         = setinfo.bool2;
   showstart       = setinfo.bool3;
   gameprompt      = setinfo.bool4;
   chatter         = setinfo.bool5;
   boardtype       = setinfo.bool6;
   audiotype       = setinfo.bool7;
   autoplay        = setinfo.bool8;
   effects3d       = setinfo.bool9;
   chipcolor       = setinfo.ulong1;
   drawtime        = setinfo.ulong2;
   newcards        = setinfo.ulong3;
   pointer         = setinfo.ulong5;
   /* Game options */
   gametype        = setinfo.ulong4;
   numberofcards   = setinfo.ulong6;
   numberofplayers = setinfo.ulong7;
   strcpy( szName, setinfo.char20 );
   /* Font */
   strcpy( pbd->selFont, setinfo.char80 );
 }
 else                                  /* new, then use default settings      */
 {
   fnSetDefaultSettings( );
   fnSetDefaultOptions( );
   WinLoadString( hab, 0L, IDS_HUMAN, TITLESIZE, szName );
 }

 WinLoadString( hab, 0L, IDS_INIHISCORES, TEXTSIZE, szText );
 phsd->hini = PrfOpenProfile( hab, szText );
 if( phsd->hini == 0L )
 {
   WinLoadString( hab, 0L, IDS_PROFILEERROR, TITLESIZE, szTitle );
   WinLoadString( hab, 0L, IDS_PROFILE_HISCORE, TEXTSIZE, szText );
   WinMessageBox( HWND_DESKTOP, 0L, szText, szTitle,
                  1, MB_ERROR | MB_OK | MB_MOVEABLE );
 }

 ulSize = sizeof( HISCORESDATA );
 if( PrfQueryProfileData( phsd->hini, szAppName, szHiScores, &hsData,
     &ulSize ) )
 {
   for(x=0; x < MAXHISCORES; x++)
   {
     strcpy( phsd->player[x], hsData.player[x] );
     phsd->score[x] = hsData.score[x];
   }
 }
 else
   fnResetHiScores( );

 /* Initialize the audio card, if one is present.                             */
 audioDevId = 0;
 if(audiotype == AUDIO_CARD && os2mm && os2wave)
   fnAudioCardInit(HWND_DESKTOP);
 else
   audiotype = PC_SPEAKER;

 if(debug)
 {
   fprintf(wptrDebug, "sound     : %d\n", sound);
   fprintf(wptrDebug, "markall   : %d\n", markall);
   fprintf(wptrDebug, "chatter   : %d\n", chatter);
   fprintf(wptrDebug, "showstart : %d\n", showstart);
   fprintf(wptrDebug, "gameprompt: %d\n", gameprompt);
   fprintf(wptrDebug, "chipcolor : %d\n", chipcolor);
   fprintf(wptrDebug, "boardtype : %d\n", boardtype);
   fprintf(wptrDebug, "drawtime  : %d\n", drawtime);
   fprintf(wptrDebug, "newcards  : %d\n", newcards);
   fprintf(wptrDebug, "audiotype : %d\n", audiotype);
   fprintf(wptrDebug, "pointer   : %d\n", pointer);
   fprintf(wptrDebug, "autoplay  : %d\n", autoplay);
   fprintf(wptrDebug, "gametype  : %d\n", gametype);
   fprintf(wptrDebug, "effects3d : %d\n", effects3d);
   fprintf(wptrDebug, "noofcards : %d\n", numberofcards);
   fprintf(wptrDebug, "noofplayer: %d\n", numberofplayers);
   fprintf(wptrDebug, "szName    : %s\n", szName);
   fprintf(wptrDebug, "selFont   : %s\n", pbd->selFont);
   fprintf(wptrDebug, "audioDevId: %d\n", audioDevId);
   for(x=0; x < MAXHISCORES; x++)
     fprintf( wptrDebug, "High Score: %s %d\n", phsd->player[x],
                                                phsd->score[x]);
 }

 /* Query the system profile for the user-configured value of logo display    */
 /* time. If no time found, set default to indefinite (-1) display time. If   */
 /* non-zero, call dialog box to display the logo 'about' box.                */

 lLogoDisplay = PrfQueryProfileInt (HINI_PROFILE, "PM_ControlPanel",
                                    "LogoDisplayTime", -1L);

 if (lLogoDisplay != 0)
   WinDlgBox (HWND_DESKTOP, 0L, wpAboutDlgProc,
              0L, IDD_ABOUT, (PVOID) &lLogoDisplay);

 /* Window for bingo board. Get title bar text in resource string table.      */
 /* Then position the window depending on the board type.                     */

 hwndFrame = WinCreateStdWindow (HWND_DESKTOP, 0, &flFrameFlags,
                                 szClientClass, NULL, 0L, 0L,
                                 idFrameWindow, &hwndBoard );

 WinLoadString (hab, 0L, IDS_TITLEBAR, TITLESIZE, szTitle);
 WinSetWindowText (hwndFrame, szTitle);

 fnPositionBoard( hwndFrame );

 /* Help manager initialization & preparation. Set up the HELPINIT structure  */
 /* Create help instance, if unable, display error message.                   */

 hiMain.cb = sizeof (HELPINIT);        /* Standard size of structure          */
 hiMain.ulReturnCode = 0L;             /* rc - if non-zero after -> error!    */
 hiMain.pszTutorialName = (PSZ)NULL;   /* tutorial program, if none, set NULL */
                                       /* Help table ID in resources          */
 hiMain.phtHelpTable = (PVOID)( 0xffff0000 | IDH_TABLE_MAIN );
 hiMain.hmodHelpTableModule = 0L;      /* Help not in DLL                     */
 hiMain.hmodAccelActionBarModule = 0L; /* No add'l help resources             */
 hiMain.idAccelTable = 0L;             /* No add'l help accelerator           */
 hiMain.idActionBar = 0L;              /* No add'l help menu                  */

 WinLoadString (hab, 0L, IDS_HELPTITLE, TITLESIZE, szTitle);
 hiMain.pszHelpWindowTitle = szTitle;     /* Help manager window title        */

 hiMain.fShowPanelId = FALSE;             /* Don't show panel numbers         */

 WinLoadString (hab, 0L, IDS_HELPNAME, 13, szText );
 hiMain.pszHelpLibraryName = szText;      /* Name of .HLP file                */

 hwndHelp = WinCreateHelpInstance (hab, &hiMain); /* create help instance     */
 if (!hwndHelp)
   WinMessageBox (HWND_DESKTOP, 0L, szText,
                  "Help Error", 1, MB_OK | MB_MOVEABLE );
 else
 {
   if (hiMain.ulReturnCode)
   {
     WinLoadString( hab, 0L, IDS_HELPERROR, TITLESIZE, szTitle );
     WinLoadString( hab, 0L, IDS_NO_HELP, TEXTSIZE, szText );
     WinMessageBox(HWND_DESKTOP, 0L, szText, szTitle, 1, MB_OK | MB_MOVEABLE);
     WinDestroyHelpInstance (hwndHelp);
     hwndHelp = 0L;
   }
   else
     WinAssociateHelpInstance (hwndHelp, hwndFrame);
 }

 /* Standard message loop. Get messages from the queue and dispatch them to   */
 /* the approprite windows.                                                   */

 while (WinGetMsg (hab, &qmsg, 0L, 0, 0))
    WinDispatchMsg (hab, &qmsg);

 /* Main loop has terminated. Destroy all windows, help instance and message  */
 /* queue and then terminate the application.                                 */

 if (hwndHelp)
    WinDestroyHelpInstance (hwndHelp);
 WinDestroyWindow (hwndFrame);
 WinDestroyMsgQueue (hmq);
 WinTerminate (hab);

 DosExit (EXIT_PROCESS, 0L);

}

/******************************************************************************/
/*                                                                            */
/*                      W I N D O W   P R O C E D U R E S                     */
/*                                                                            */
/******************************************************************************/
/******************************************************************************/
/*                                                                            */
/* wpClientWndProc: Window procedure for Bingo Board client window. WM_CREATE */
/*                  posts a UM_CREATECARD user message to create the bingo    */
/*                  cards (handled by wpBingoWndProc). The WM_PAINT message   */
/*                  calls fnDrawBoard which draws the bingo board. WM_TIMER   */
/*                  handles the drawing of random numbers at regular intervals*/
/*                  which is fired by WinStartTimer.                          */
/*                                                                            */
/******************************************************************************/

MRESULT EXPENTRY wpClientWndProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)

{

 MRESULT mr = (MRESULT) FALSE;
 static  BOOL firsttime, sharetimecount;

 switch (msg)
    {

     case MM_MCIPASSDEVICE:
       {
         WinMessageBox( HWND_DESKTOP, 0L, "MM_MCIPASSDEVICE", "Debug",
                        1, MB_ERROR | MB_OK | MB_MOVEABLE );
       }
       break;

     case WM_CREATE:                   /* Perform one-time initialization     */
                                       /* here.                               */
        {
          ULONG i;

          fnInitializeScores();        /* Initialize scores to 0              */

          fnFindAllFonts(hwnd);        /* Font processing, find all outline   */
          fnFindFont(hwnd, pbd->selFont);  /* fonts, select appropriate one   */
                                       /* Load bingo card pointers, get handle*/
          for(i=0; i<MAXPOINTERS; i++)
            pbd->hptr[i] = WinLoadPointer( HWND_DESKTOP, 0L, IDP_BINGO+i );

          fnCreateBingoGames( );       /* Initialize games, pbd->bingogames   */

          fnGetBoardNumbers( );        /* Initialize numbers, pbd->board      */
                                       /* More initialization of variables    */
          bingostatus  = PREGAME;
          bingonumber  = 0;
          numbersdrawn = 0;
          firsttime    = 0;
                                       /* Disable some menu items             */
          WinEnableMenuItem( WinWindowFromID( WinQueryWindow( hwnd, QW_PARENT ),
                             FID_MENU ), IDM_PAUSE, FALSE );
          WinEnableMenuItem( WinWindowFromID( WinQueryWindow( hwnd, QW_PARENT ),
                             FID_MENU ), IDM_NEXTNUM, FALSE );
          WinEnableMenuItem( WinWindowFromID( WinQueryWindow( hwnd, QW_PARENT ),
                             FID_MENU ), IDM_STOP, FALSE );

          if( SHAREWARE )
          {
            WinEnableMenuItem( WinWindowFromID( WinQueryWindow(hwnd, QW_PARENT),
                               FID_MENU ), IDM_SCORE, FALSE );
            WinStartTimer( hab, hwnd, ID_TIMERSHARE, 60000L );
            sharetimecount = SHARETIMER;
          }

          WinPostMsg( hwnd, UM_CREATECHAT, 0L, 0L); /* Post message to create */
                                                    /* Chatter box dialog     */
          WinPostMsg( hwnd, UM_CREATECARDS, 0L, 0L);/* Post message to create */
                                                    /* number of cards        */
          if(debug)
           fprintf(wptrDebug, "WM_CREATE completed.\n");
        }                              /* end case WM_CREATE                  */
        break;

     case WM_PAINT:                    /* Redraw the client window here ...   */
        fnDrawBoard( hwnd );
        break;

     case WM_ERASEBACKGROUND:
        mr = (MRESULT) TRUE;
        break;

     case UM_GAMETYPE:                 /* Game type has been changed, so      */
        {                              /* we must draw the bitmap in the      */
          HPS   hps;                   /* Bingo board to the one selected     */
          RECTL rcl, orcl;
          LONG  xspace, yspace, xl, yl;

          hps = WinBeginPaint (hwnd, 0L, 0L);
          WinQueryWindowRect( hwnd, &rcl );
          fnGetBoardSizes( rcl, &xspace, &yspace, &xl, &yl );
          fnDrawBitmap( hps, rcl, xspace, yspace, xl, yl, &orcl);
          WinEndPaint( hwnd );

          if( firsttime != 0 || showstart != YES )
            WinInvalidateRect( hwnd, &orcl, FALSE );
          else
            WinInvalidateRect( hwnd, &rcl, FALSE );
        }                              /* end case UM_GAMETYPE                */
        break;

     case WM_TIMER:                    /* invoked at timer intervals for the  */
        {                              /* regular bingo number drawing        */

          USHORT timerid;              /* get timer id from mp1 first         */
          ULONG  x, y;
          timerid = SHORT1FROMMP( mp1 );

          switch( timerid )
          {
            case ID_TIMER:             /* Refresh timer procedure here        */
              {                        /* If game in progress, draw next no   */
                if(boolTime)           /* Only process timer if flag is TRUE  */
                {
                  switch(bingostatus)
                  {
                    case INPROGRESS:
                    {
                      fnDrawBingoNumber( hwnd );
                    }                  /* end case INPROGRESS                 */
                    break;

                    case GAMEOVER:
                    {
                      PCARDLIST item;
                      BOOL      cardCheck = NO;
                      item = start;
                      while( item )
                      {
                        if(item->pcd->status == CARD_CHECK)
                        {
                          cardCheck = YES;
                          break;
                        }
                        item = (PCARDLIST) item->nextcard;
                      }
                      if(cardCheck == NO)
                        fnDisplayBingo(hwnd, TRUE);

                      if(debug)
                        fprintf(wptrDebug, "WM_TIMER - GAMEOVER.\n");

                    }                  /* end case GAMEOVER                   */
                    break;

                    case BINGOSHOWN:
                    case PREGAME:      /* game not in progress, show msg box  */
                    {                  /* box once only to ask to start game  */

                      if( startbox != OFF )/* message box flag ON,show msg box*/
                      {
                        if(sound == ON && gameprompt == YES && startbox == ON)
                          fnSound( SOUND_NEWGAME, 0, 0 );
                        if(gameprompt == YES && startbox == ON)
                        {
                          startbox = OFF;

                          WinLoadString(hab, 0L, IDS_TIMERBOX,  TITLESIZE,
                                        szTitle);
                          WinLoadString(hab, 0L, IDS_TIMERTEXT, TEXTSIZE,
                                        szText);
                          if( WinMessageBox( HWND_DESKTOP, hwnd, szText, szTitle,
                                             IDD_TIMER,
                                             MB_MOVEABLE | MB_YESNO | MB_HELP |
                                             MB_DEFBUTTON1 ) == MBID_YES )
                             WinSendMsg( hwndBoard, WM_COMMAND,
                                         MPFROM2SHORT( IDM_START, TRUE ), 0L );
                        }
                        else
                          WinSendMsg( hwndBoard, WM_COMMAND,
                                      MPFROM2SHORT( IDM_START, TRUE ), 0L );
                        startbox = OFF;
                      }
                    }                  /* end case PREGAME                    */
                    break;

                    case PRINTING:     /* Bingo cards are being printed, we   */
                      break;           /* can't do much until finished        */

                    default:
                      break;
                  }                    /* end switch                          */
                }                      /* end if(boolTime)                    */
              }                        /* end case ID_TIMER                   */
              break;

            case ID_TIMERSHARE:        /* Shareware version - reminder timer  */
              {
                sharetimecount--;
                if( sharetimecount == 0 )
                {
                  CHAR szTitle1[40];

                  if( bingostatus == INPROGRESS )
                    WinStopTimer( hab, hwnd, ID_TIMER );
                  WinStopTimer( hab, hwnd, ID_TIMERSHARE );

                  WinLoadString (hab, 0L, IDS_SHARETITLE, 40, szTitle1 );
                  WinLoadString (hab, 0L, IDS_SHAREREGISTER, TEXTSIZE, szText );
                  WinMessageBox (HWND_DESKTOP, hwnd, szText, szTitle1, 1,
                                 MB_MOVEABLE | MB_OK );

                  if( bingostatus == INPROGRESS )
                    WinStartTimer( hab, hwnd, ID_TIMER, drawtime );
                  WinStartTimer( hab, hwnd, ID_TIMERSHARE, 60000L );

                  sharetimecount = SHARETIMER;
                }
              }                        /* end case ID_TIMERSHARE              */
              break;

            default:
              break;
          }
        }                              /* end case WM_TIMER                   */
        break;

     case WM_COMMAND:                  /* Application command processing here */
        {
         switch (SHORT1FROMMP (mp1))   /* Message contained in mp1 variable   */
            {
             case IDM_START:           /* Start menu item selected            */
                {

                  BOOL newgame;        /* If in progress, warn the user that  */
                                       /* current game will be lost!          */
                  newgame = 1;
                  if( bingostatus == INPROGRESS )
                  {

                    boolTime = FALSE;

                    if(sound == ON)
                      fnSound( SOUND_QUIT, 0, 0 );
                    WinLoadString (hab, 0L, IDS_START, TITLESIZE, szTitle);
                    WinLoadString (hab, 0L, IDS_STARTTEXT, TEXTSIZE, szText);
                    if( WinMessageBox (HWND_DESKTOP, hwnd, szText, szTitle,
                                       IDD_START,
                                       MB_MOVEABLE | MB_OKCANCEL | MB_HELP |
                                       MB_DEFBUTTON2 ) == MBID_CANCEL )
                    {                  /* User cancel, start timer and        */
                      newgame = 0;     /* continue with the game              */
                      boolTime = TRUE;
                    }
                    else               /* Ok, game prematurely ended, change  */
                      fnUpdateScores(1); /* the scores to ignore last game    */
                  }
                                       /* Ok, new game is really desired so   */
                                       /* create brand new bingo cards        */
                  if( newgame == 1 && bingostatus != PREGAME )
                  {
                    BOOL      xnewcards = NO;
                    PCARDLIST temp;

                    if( newcards == PROMPT )
                    {
                      if(sound == ON)
                        fnSound( SOUND_NEWCARDS, 0, 0 );
                      WinLoadString(hab, 0L, IDS_NEWCARDS,  TITLESIZE,
                                    szTitle);
                      WinLoadString(hab, 0L, IDS_NEWCARDSTEXT, TEXTSIZE,
                                    szText);
                      if( WinMessageBox( HWND_DESKTOP, hwnd, szText, szTitle,
                                         IDD_NEWCARDS,
                                         MB_MOVEABLE | MB_YESNO | MB_HELP |
                                         MB_DEFBUTTON1 ) == MBID_YES )
                         xnewcards = YES;
                    }
                    else
                    {
                      if( newcards == YES )
                        xnewcards = YES;
                      else
                        xnewcards = NO;
                    }

                    fnGetBoardNumbers( );   /* reinitialize the bingo board   */
                    bingonumber  = 0;       /* and other variables since this */
                    numbersdrawn = 0;
                    bingostatus  = PREGAME; /* is the start of new game       */

                    WinSendMsg(hwndChat, UM_NEWGAME, 0L, 0L);

                    temp = start;      /* create new bingo cards (actually    */
                    while( temp )      /* just new numbers of cards)          */
                    {
                      if( xnewcards == YES )
                        WinSendMsg( temp->hwndCard, UM_NEWGAME, 0L, 0L );
                      else
                        WinSendMsg( temp->hwndCard, UM_NEWGAME,
                                    MPFROM2SHORT( 1, 0 ), 0L );
                      temp = (PCARDLIST)temp->nextcard;
                    }
                    temp = startOther; /* Now process the virtual bingo cards */
                    while( temp )      /* just reset pcd->bingoit to 0.       */
                    {
                      fnGetCardNumbers(temp->pcd, 1);
                      WinDestroyWindow(temp->hwndCard);
                      temp->hwndCard   = 0L;
                      temp->hwndClient = 0L;
                      temp->pcd->waitnumber = NO_BINGO;
                      temp = (PCARDLIST)temp->nextcard;
                    }
                  }

                  if( bingostatus == PREGAME )
                  {                    /* Do some window stuff here, force to */
                                       /* repaint bingo board (client window) */
                    RECTL rcl;         /* and enable/disable menu items also  */

                    time(&startime);

                    bingostatus = INPROGRESS;

                    fnUpdateScores(0);
                    if(psd->moneyleft < 0)
                    {
                      boolTime = FALSE;
                      WinLoadString(hab, 0L, IDS_NOMONEY,  TITLESIZE,
                                    szTitle);
                      WinLoadString(hab, 0L, IDS_NOMONEYTEXT, TEXTSIZE,
                                    szText);
                      if( WinMessageBox (HWND_DESKTOP, hwnd,
                                         szText, szTitle,
                                         IDD_NOMONEY,
                                         MB_MOVEABLE | MB_YESNO | MB_HELP |
                                         MB_DEFBUTTON1 ) == MBID_YES )
                      {
                        fnInitializeScores();
                        fnUpdateScores(0);
                      }
                      else
                        WinSendMsg(hwnd, WM_CLOSE, 0L, 0L);
                    }

                    WinQueryWindowRect( hwnd, &rcl );
                    WinInvalidateRect( hwnd, &rcl, FALSE );

                    WinEnableMenuItem( WinWindowFromID( WinQueryWindow( hwnd,
                                       QW_PARENT ), FID_MENU ), IDM_STOP,
                                       TRUE );
                    WinEnableMenuItem( WinWindowFromID( WinQueryWindow( hwnd,
                                       QW_PARENT ), FID_MENU ), IDM_PAUSE,
                                       TRUE );
                    WinEnableMenuItem( WinWindowFromID( WinQueryWindow( hwnd,
                                       QW_PARENT ), FID_MENU ), IDM_NEXTNUM,
                                       TRUE );
                    WinEnableMenuItem( WinWindowFromID( WinQueryWindow( hwnd,
                                       QW_PARENT ), FID_MENU ), IDM_GAMES,
                                       FALSE );
                    WinEnableMenuItem( WinWindowFromID( WinQueryWindow( hwnd,
                                       QW_PARENT ), FID_MENU ), IDM_PRINT,
                                       FALSE );
                                       /* finally, begin by starting timer to */
                                       /* have the bingo numbers drawn        */
                    boolTime = TRUE;

                    if(debug)
                    {
                      fprintf(wptrDebug, "IDM_START completed. Game start.\n");
                      fprintf(wptrDebug, "gametype       : %d\n", gametype);
                      fprintf(wptrDebug, "numberofcards  : %d\n", numberofcards);
                      fprintf(wptrDebug, "numberofplayers: %d\n", numberofplayers);
                    }
                  }
                }                      /* end case IDM_START                  */
                break;

             case IDM_PAUSE:           /* Game paused, but only if it is in   */
                {                      /* progress, display nice message box  */
                  if( bingostatus == INPROGRESS )
                  {

                    boolTime = FALSE;

                    WinLoadString (hab, 0L, IDS_PAUSE, TITLESIZE, szTitle);
                    WinLoadString (hab, 0L, IDS_PAUSETEXT, TEXTSIZE, szText);
                    WinMessageBox (HWND_DESKTOP, hwnd, szText, szTitle, 0,
                                   MB_MOVEABLE | MB_OK | MB_DEFBUTTON1 );

                    boolTime = TRUE;
                  }
                }                      /* end case IDM_PAUSE                  */
                break;

             case IDM_STOP:            /* User wants to stop game, ask user   */
                {                      /* first to ensure they want to        */
                  if( bingostatus == INPROGRESS )
                  {

                    boolTime = FALSE;

                    if(sound == ON)
                      fnSound( SOUND_QUIT, 0, 0 );
                    WinLoadString (hab, 0L, IDS_STOP, TITLESIZE, szTitle);
                    WinLoadString (hab, 0L, IDS_STOPTEXT, TEXTSIZE, szText);

                    if( WinMessageBox (HWND_DESKTOP, hwnd, szText, szTitle,
                                       IDD_STOP,
                                       MB_MOVEABLE | MB_YESNO | MB_HELP |
                                       MB_DEFBUTTON2 ) == MBID_YES )
                    {                  /* Game ended prematurely!             */
                      bingostatus = GAMEOVER;
                      fnUpdateScores(1);
                      WinSendMsg( hwnd, UM_GAMEOVER, 0L, 0L );
                    }

                    boolTime = TRUE;
                  }
                }                      /* end case IDM_STOP                   */
                break;

             case IDM_SCORE:           /* Process dialog box for Bingo score  */
                {
                  boolTime = FALSE;
                  WinDlgBox( HWND_DESKTOP, hwnd, wpScoreDlgProc, 0L,
                             IDD_SCORE, NULL );
                  boolTime = TRUE;
                }                      /* end case IDM_SCORE                  */
                break;

             case IDM_HISCORES:        /* Process dialog box for High Scores  */
                {
                  boolTime = FALSE;
                  WinDlgBox( HWND_DESKTOP, hwnd, wpHiScoresDlgProc, 0L,
                             IDD_HISCORES, NULL );
                  boolTime = TRUE;
                }                      /* end case IDM_HISCORES               */
                break;

             case IDM_NEXTNUM:         /* Draw next number without waiting    */
                {                      /* draw time to expire.                */
                                       /* Send a WM_TIMER message to client   */
                                       /* window to force next number drawing */
                  if( bingostatus == INPROGRESS )
                  {
                    boolTime = FALSE;
                    WinStopTimer( hab, hwnd, ID_TIMER );
                    fnDrawBingoNumber( hwnd );
                    WinStartTimer( hab, hwnd, ID_TIMER, drawtime );
                    boolTime = TRUE;
                  }
                }                      /* end case IDM_NEXTNUM                */
                break;

             case IDM_AUTOPLAY:        /* Toggle auto play mode by toggling   */
                {                      /* variable and then put/remove check  */
                  autoplay = !autoplay;/*mark according to bool variable      */
                  WinSendDlgItemMsg( WinQueryWindow( hwnd,QW_PARENT ),
                                     (ULONG) FID_MENU,
                                     (ULONG) MM_SETITEMATTR,
                                     MPFROM2SHORT( IDM_AUTOPLAY,TRUE ),
                                     MPFROM2SHORT(MIA_CHECKED, autoplay ?
                                     MIA_CHECKED : 0));
                }                      /* end case IDM_AUTOPLAY               */
                break;

             case IDM_MARKALL:         /* Toggle mark all mode by toggling    */
                {                      /* variable and then put/remove check  */
                  markall = !markall;  /* Mark according to bool variable     */
                  WinSendDlgItemMsg( WinQueryWindow( hwnd,QW_PARENT ),
                                     (ULONG) FID_MENU,
                                     (ULONG) MM_SETITEMATTR,
                                     MPFROM2SHORT( IDM_MARKALL,TRUE ),
                                     MPFROM2SHORT(MIA_CHECKED, markall ?
                                     MIA_CHECKED : 0));
                }                      /* end case IDM_MARKALL                */
                break;

             case IDM_CHATTER:         /* Toggle chatter mode by toggling     */
                {                      /* variable and then put/remove check  */
                  SWP swp;             /* mark according to bool variable     */

                  chatter = !chatter;
                  WinSendDlgItemMsg( WinQueryWindow( hwnd,QW_PARENT ),
                                     (ULONG) FID_MENU,
                                     (ULONG) MM_SETITEMATTR,
                                     MPFROM2SHORT( IDM_CHATTER,TRUE ),
                                     MPFROM2SHORT(MIA_CHECKED, chatter ?
                                     MIA_CHECKED : 0));
                  WinQueryWindowPos(hwndChat, &swp);
                  WinSetWindowPos(hwndChat, HWND_TOP,
                                  swp.x, swp.y, swp.cx, swp.cy,
                                  SWP_MOVE | SWP_ACTIVATE);
                  WinShowWindow( hwndChat, chatter );
                }                      /* end case IDM_CHATTER                */
                break;

             case IDM_SOUND:           /* Toggle sound mode by toggling       */
                {                      /* variable and then put/remove check  */
                  sound = !sound;      /* mark according to bool variable     */
                  WinSendDlgItemMsg( WinQueryWindow( hwnd,QW_PARENT ),
                                     (ULONG) FID_MENU,
                                     (ULONG) MM_SETITEMATTR,
                                     MPFROM2SHORT( IDM_SOUND,TRUE ),
                                     MPFROM2SHORT(MIA_CHECKED, sound ?
                                     MIA_CHECKED : 0));
                }                      /* end case IDM_SOUND                  */
                break;

             case IDM_GAMES:           /* ok, display Options dialog box here */
                                       /* processing handled wpOptionsDlgProc */
                {                      /* disabled when game in progress      */
                  boolTime = FALSE;
                  if( bingostatus != INPROGRESS )
                  {
                    RECTL rcl;
                    ULONG oldnumcards = numberofcards;
                    ULONG oldnumplayers = numberofplayers;

                                       /* show modal dialog box for Options ..*/
                    WinDlgBox (HWND_DESKTOP, hwnd, wpOptionsDlgProc, 0L,
                               IDD_OPTIONS, NULL );
                    /* Check if number of cards or players were changed       */
                    /* If yes, then we have some work to manage the changes   */
                    if( oldnumcards   != numberofcards ||
                        oldnumplayers != numberofplayers )
                    {
                      fnFreeCardList( start, last );
                      fnFreeCardList( startOther, lastOther );

                      WinSendMsg( hwnd, UM_CREATECARDS, 0L, 0L );
                    }

                    WinQueryWindowRect( hwnd, &rcl );
                    WinInvalidateRect( hwnd, &rcl, FALSE );
                  }
                  boolTime = TRUE;
                }                      /* end case IDM_GAMES                  */
                break;

             case IDM_SETTINGS:        /* Display Setting dialog box here ... */
                {
                  BOOL  oldboardtype = boardtype;
                  ULONG oldpointer   = pointer;
                  BOOL  oldchatter   = chatter;
                  ULONG olddrawtime  = drawtime;
                  BOOL  oldeffects3d = effects3d;

                  boolTime = FALSE;

                  if(WinDlgBox (HWND_DESKTOP, hwnd, wpSettingsDlgProc, 0L,
                             IDD_SETTINGS, NULL) == TRUE)
                  {
                    PCARDLIST item;

                    /* If board type changed, redraw the Bingo board          */
                    if(oldboardtype != boardtype)
                      fnPositionBoard(WinQueryWindow(hwnd, QW_PARENT));

                    /* If draw time changed, stop and start timer with new tim*/
                    if(olddrawtime != drawtime)
                    {
                      WinStopTimer( hab, hwnd, ID_TIMER );
                      WinStartTimer( hab, hwnd, ID_TIMER, drawtime );
                    }

                    /* Put/remove check mark for auto play in case of change  */
                    WinSendDlgItemMsg( WinQueryWindow( hwnd, QW_PARENT ),
                                       (ULONG) FID_MENU,
                                       (ULONG) MM_SETITEMATTR,
                                       MPFROM2SHORT( IDM_AUTOPLAY, TRUE ),
                                       MPFROM2SHORT(MIA_CHECKED, autoplay ?
                                       MIA_CHECKED : 0));

                    /* Put/remove check mark for Mark All in case of change   */
                    WinSendDlgItemMsg( WinQueryWindow( hwnd, QW_PARENT ),
                                       (ULONG) FID_MENU,
                                       (ULONG) MM_SETITEMATTR,
                                       MPFROM2SHORT( IDM_MARKALL, TRUE ),
                                       MPFROM2SHORT(MIA_CHECKED, markall ?
                                       MIA_CHECKED : 0));

                    /* Put/remove check mark for Sound in case of change      */
                    WinSendDlgItemMsg( WinQueryWindow( hwnd, QW_PARENT ),
                                       (ULONG) FID_MENU,
                                       (ULONG) MM_SETITEMATTR,
                                       MPFROM2SHORT( IDM_SOUND, TRUE ),
                                       MPFROM2SHORT(MIA_CHECKED, sound ?
                                       MIA_CHECKED : 0));

                    /* Put/remove check mark for Chatter in case of change    */
                    if(oldchatter != chatter)
                    {
                      WinSendDlgItemMsg( WinQueryWindow( hwnd, QW_PARENT ),
                                         (ULONG) FID_MENU,
                                         (ULONG) MM_SETITEMATTR,
                                         MPFROM2SHORT( IDM_CHATTER, TRUE ),
                                         MPFROM2SHORT(MIA_CHECKED, chatter ?
                                         MIA_CHECKED : 0));
                      WinShowWindow( hwndChat, chatter );
                    }

                    /* New mouse pointer has been selected, let bingo cards   */
                    /* know by sending user message called UM_POINTER         */
                    if(oldpointer != pointer)
                    {
                      item = start;
                      while( item )
                      {
                        WinSendMsg( item->hwndCard, UM_POINTER, 0L, 0L );
                        item = (PCARDLIST)item->nextcard;
                      }
                    }
                    /* If 3d effects has changes, redraw the windows          */
                    if(oldeffects3d != effects3d)
                      fnRedrawWindows( hwnd );

                  }

                  boolTime = TRUE;
                }
                break;                 /* end case IDM_SETTINGS               */

             case IDM_PRINT:           /* Display Printer dialog box here ... */
                {
                  boolTime = FALSE;
                  WinDlgBox (HWND_DESKTOP, hwnd, wpPrintDlgProc, 0L,
                             IDD_PRINT, NULL);
                  boolTime = TRUE;
                }
                break;                 /* end case IDM_PRINT                  */

             case IDM_FONT:            /* Display Font dialog box here ...    */
                {
                  boolTime = FALSE;
                  if(WinDlgBox (HWND_DESKTOP, hwnd, wpFontDlgProc,
                                0L, IDD_FONT, NULL) == TRUE)
                  {

                    fnFindFont(hwnd, pbd->selFont);

                    if(debug)
                    {
//                    WinMessageBox( HWND_DESKTOP, 0L, pbd->selFont, "Debug",
//                                   1, MB_OK | MB_MOVEABLE );
                      fprintf(wptrDebug, "Font changed to %s.\n", pbd->selFont);
                    }
                    /* Redraw all the windows since the font has changed */
                    fnRedrawWindows( hwnd );

                  }
                  boolTime = TRUE;
                }
                break;                 /* end case IDM_FONT                   */

             case IDM_CARDS:           /* Display Other Card dialog box     . */
                {
                  boolTime = FALSE;
                  WinDlgBox (HWND_DESKTOP, hwnd, wpCardDlgProc, 0L,
                             IDD_CARDS, NULL);
                  boolTime = TRUE;
                }
                break;                 /* end case IDM_PRINT                  */

             case IDM_EXIT:            /* Exit, just post WM_CLOSE message    */
                WinPostMsg (hwnd, WM_CLOSE, 0L, 0L);
                break;

             case IDM_USINGHELP:       /* Display Using Help Facility window  */
                                       /* Just post HM_DISPLAY_HELP message   */
               WinSendMsg (hwndHelp, HM_DISPLAY_HELP, 0L, 0L);
               break;

             case IDM_HELPPRODINFO:    /* Display Product Information window  */
               {
                 LONG lLogoDisplay = -1;

                 WinDlgBox (HWND_DESKTOP, hwnd, wpAboutDlgProc,
                            0L, IDD_ABOUT, (PVOID) &lLogoDisplay);
               }
               break;

             case IDM_BINGO:           /* Debug to show Bingo! dialog anytime */
               {                       /* using Ctrl-B hidden key combination */
                 LONG bingoFound = TEST;
                 WinDlgBox(HWND_DESKTOP, hwnd, wpBingoDlgProc, 0L,
                           IDD_BINGO, (PVOID)&bingoFound);
               }
               break;

             default:
                break;
            }                          /* end switch                          */
        }                              /* end case (WM_COMMAND)               */
        break;

     case HM_QUERY_KEYS_HELP:          /* User requested keys help            */

       mr = (MRESULT) IDH_HELPKEYS;    /* Return keys help panel id           */
       break;

     case WM_ACTIVATE:                 /* When Bingo Board is brought to the  */
       {                               /* foreground (activated), bring all   */
         if( SHORT1FROMMP(mp1)==TRUE && /* the bingo cards to the foreground  */
             HWNDFROMMP(mp2)==WinQueryWindow(hwnd,QW_PARENT) )
         {
           PCARDLIST pCard;

           pCard = start;              /* Loop thru all bingo cards and bring */
           while( pCard )              /* then to the foreground by activating*/
           {
             WinSetActiveWindow( HWND_DESKTOP, pCard->hwndCard );
             pCard = (PCARDLIST)pCard->nextcard;
           }
           WinSetActiveWindow( HWND_DESKTOP, WinQueryWindow(hwnd,QW_PARENT) );
         }
         mr = WinDefWindowProc (hwnd, msg, mp1, mp2);
       }                               /* end case WM_ACTIVATE                */
       break;

     case WM_CLOSE:                    /* User requested to exit application  */
       {                               /* so just post a WM_QUIT message      */
                                       /* if game in progress, ask to confirm */
         BOOL answer = YES;
         if( bingostatus == INPROGRESS )
         {
                                       /* Confirmation message box is used in */
                                       /* Get text from resource string table */
           boolTime = FALSE;
           if(sound == ON)
             fnSound( SOUND_QUIT, 0, 0 );
           WinLoadString (hab, 0L, IDS_EXIT, 20, szTitle );
           WinLoadString (hab, 0L, IDS_EXITPROMPT, 80, szText );
           if (WinMessageBox (HWND_DESKTOP, hwnd, szText, szTitle, 1,
                              MB_MOVEABLE | MB_YESNO |
                              MB_DEFBUTTON2 ) == MBID_NO)
           {
             answer = NO;
             boolTime = TRUE;
           }
         }
         if(answer == YES)
         {
           if(sound == ON)
           {
             PSND   pSoundData;

             pSoundData = (PSND)malloc(sizeof(SND));
             pSoundData->option = SOUND_EXIT;
             pSoundData->letter = 0;
             pSoundData->number = 0;

             fnSoundProcess(pSoundData);
           }

           if( SHAREWARE )
           {
             CHAR szTitle1[40];

             if(sound == ON)
               fnSound( SOUND_REGISTER, 0, 0 );
             WinLoadString (hab, 0L, IDS_SHARETITLE, 40, szTitle1 );
             WinLoadString (hab, 0L, IDS_SHAREORDER, 80, szText );
             WinMessageBox (HWND_DESKTOP, hwnd, szText, szTitle1, 1,
                            MB_MOVEABLE | MB_OK );
           }

           WinPostMsg (hwnd, WM_QUIT, 0L, 0L);
         }
       }                               /* end case WM_CLOSE                   */
       break;

     case WM_DESTROY:                  /* Window about to be destroyed.       */
        {
          WinDestroyWindow( hwndChat );/* Destroy Chatter dialog window       */

          fnFreeCardList( start, last );/* free memory for all bingo card data*/
          fnFreeCardList( startOther, lastOther );

          free ( pbd->pFontnames );    /* free memory for list of fonts       */
          free ( pbd );                /* free memory for bingo board data    */
          free ( psd );                /* free memory for bingo scores data   */
          free ( phsd );               /* free memory for high scores data    */

          fnFreeAudioModules();

          if(debug)
          {
            fprintf(wptrDebug, "WM_DESTROY completed.\n");
            fclose(wptrDebug);
          }
        }
        break;                         /* end case WM_DESTROY                 */

     case WM_SAVEAPPLICATION:          /* Application about to terminate.     */
       {                               /* Save information in profile data.   */
         PROFILEINFO  setinfo;
         HISCORESDATA hsData;
         LONG         i;

         /* Settings and game options */
         setinfo.bool1  = sound;
         setinfo.bool2  = markall;
         setinfo.bool3  = showstart;
         setinfo.bool4  = gameprompt;
         setinfo.bool5  = chatter;
         setinfo.bool6  = boardtype;
         setinfo.bool7  = audiotype;
         setinfo.bool8  = autoplay;
         setinfo.bool9  = effects3d;
         setinfo.ulong1 = chipcolor;
         setinfo.ulong2 = drawtime;
         setinfo.ulong3 = newcards;
         setinfo.ulong4 = gametype;
         setinfo.ulong5 = pointer;
         setinfo.ulong6 = numberofcards;
         setinfo.ulong7 = numberofplayers;
         strcpy( setinfo.char20, szName );
         strcpy( setinfo.char80, pbd->selFont );
                                       /* Write profile data in BINGO.INI     */
         PrfWriteProfileData(pbd->hini, szAppName, szSettings,
                             &setinfo, sizeof( PROFILEINFO ));

         /* High scores */
         fnHighScores( );
         if( pbd->hini != 0L )
           PrfCloseProfile( pbd->hini );

         for(i=0; i < MAXHISCORES; i++)
         {
           strcpy( hsData.player[i], phsd->player[i] );
           hsData.score[i] = phsd->score[i];
         }
                                       /* Write High Scores in BINGOHS.INI    */
         PrfWriteProfileData(phsd->hini, szAppName, szHiScores,
                             &hsData, sizeof( HISCORESDATA ));

         if( phsd->hini != 0L )
           PrfCloseProfile( phsd->hini );

         mr = WinDefWindowProc (hwnd, msg, mp1, mp2);

         if(debug)
          fprintf(wptrDebug, "WM_SAVEAPPLICATION completed.\n");
       }                               /* end case (WM_SAVEAPPLICATION)       */
       break;

     case UM_CREATECHAT:               /* Create Chatter dialog box           */
        {
          RECTL rectl;
          RECTL rectlDesktop;

          hwndChat = WinLoadDlg(HWND_DESKTOP, 0, wpChatDlgProc, 0L,
                                IDD_CHATTER, NULL);

          WinQueryWindowRect (hwndChat, &rectl);
          WinQueryWindowRect (HWND_DESKTOP, &rectlDesktop);

          WinSetWindowPos (hwndChat, HWND_TOP,
                           rectlDesktop.xRight-rectl.xRight, 0, 0, 0,
                           SWP_SHOW | SWP_MOVE | SWP_ACTIVATE);
          WinShowWindow( hwndChat, chatter );
        }                              /* end case (UM_CREATECHAT)            */
        break;

     case UM_CREATECARDS:              /* User message to create number of    */
        {                              /* bingo cards. Called by WM_CREATE    */
          RECTL      rectl;            /* and IDM_GAMES                       */
          LONG       x, y, cx, cy;
          ULONG      i;
          CHAR       szCard[TITLESIZE+1];
          CHAR       szVCard[TITLESIZE+1];
          PCARDDATA  pcd;
          PCARDLIST  pCardList;
          HWND       hwndClient;
          FRAMECDATA fcData;

          start = NULL;                /* initialize linked list bingo cards  */
          last  = NULL;
          startOther = NULL;
          lastOther  = NULL;

          WinLoadString (hab, 0L, IDS_BINGOTITLE, TITLESIZE, szCard );
          WinLoadString (hab, 0L, IDS_HALLTITLE,  TITLESIZE, szVCard );

          fcData.cb = sizeof (FRAMECDATA);
          fcData.flCreateFlags =  FCF_TITLEBAR      |
                                  FCF_SYSMENU       |
                                  FCF_SIZEBORDER    |
                                  FCF_SHELLPOSITION;
          fcData.hmodResources = 0L;
          fcData.idResources = idFrameWindow;

                                       /* Display Options dialog box only when*/
          if( (firsttime == 0) &&      /* application first invoked           */
              (showstart == YES) )
          {
            BOOL oldboardtype = boardtype;
            BOOL oldeffects3d = effects3d;

            WinDlgBox (HWND_DESKTOP, hwnd, wpOptionsDlgProc, 0L, IDD_OPTIONS,
                     NULL );
            WinShowWindow( hwndChat, chatter );
            if(oldboardtype != boardtype)
              fnPositionBoard(WinQueryWindow(hwnd, QW_PARENT));
            if(oldeffects3d != effects3d)
              fnRedrawWindows( hwnd );
          }
          firsttime = 1;

          WinSendDlgItemMsg( WinQueryWindow( hwnd, QW_PARENT ),
                             (ULONG) FID_MENU,
                             (ULONG) MM_SETITEMATTR,
                             MPFROM2SHORT( IDM_AUTOPLAY, TRUE ),
                             MPFROM2SHORT(MIA_CHECKED, autoplay ?
                             MIA_CHECKED : 0));

          WinSendDlgItemMsg( WinQueryWindow( hwnd, QW_PARENT ),
                             (ULONG) FID_MENU,
                             (ULONG) MM_SETITEMATTR,
                             MPFROM2SHORT( IDM_MARKALL, TRUE ),
                             MPFROM2SHORT(MIA_CHECKED, markall ?
                             MIA_CHECKED : 0));

          WinSendDlgItemMsg( WinQueryWindow( hwnd, QW_PARENT ),
                             (ULONG) FID_MENU,
                             (ULONG) MM_SETITEMATTR,
                             MPFROM2SHORT( IDM_SOUND, TRUE ),
                             MPFROM2SHORT(MIA_CHECKED, sound ?
                             MIA_CHECKED : 0));

          WinSendDlgItemMsg( WinQueryWindow( hwnd, QW_PARENT ),
                             (ULONG) FID_MENU,
                             (ULONG) MM_SETITEMATTR,
                             MPFROM2SHORT( IDM_CHATTER, TRUE ),
                             MPFROM2SHORT(MIA_CHECKED, chatter ?
                             MIA_CHECKED : 0));

          WinQueryWindowRect (HWND_DESKTOP, &rectl);
                                       /* obtain data to be used in position- */
                                       /* the bingo cards as they are created */
          cx = rectl.xRight / 5;
          cy = rectl.yTop / 3;
          if(boardtype == HORIZONTAL)
          {
            x  = -cx;
            y  = cy;
          }
          else
          {
            x = 0;
            y = 2*cy;
          }
                                       /* Create bingo cards by allocating    */
                                       /* memory & calling WinCreateStdWindow */
          for( i = 0; i < numberofcards; i++ )
          {
            pCardList = (PCARDLIST) malloc ( sizeof( CARDLIST ) );
            pcd = (PCARDDATA) malloc (sizeof (CARDDATA));

            if(debug)
              fprintf(wptrDebug, "UM_CREATECARDS: Created card %d.\n", pcd);

            pCardList->pcd = pcd;
            pCardList->pcd->whichPlayer = PLAYER;
            pCardList->pcd->status = CARD_NONE;
            fnGetCardNumbers( pcd, 0 );/* Randomly generate bingo card        */
            pCardList->hwndCard = WinCreateWindow (HWND_DESKTOP, WC_FRAME,
                                  NULL, 0L, 0, 0, 0, 0,
                                  0L, HWND_TOP, IDD_BINGOCARD,
                                  &fcData, NULL);

            pCardList->hwndClient = WinCreateWindow (pCardList->hwndCard, szBingoClass,
                                  NULL, 0L,
                                  0, 0, 0, 0,
                                  0L, HWND_TOP, FID_CLIENT,
                                  (PVOID)pcd, NULL);

            if (hwndHelp)
              WinAssociateHelpInstance(hwndHelp, pCardList->hwndCard);

            if( last == NULL )         /* build double linked list here ...   */
            {
              start = pCardList;
              last  = pCardList;
            }
            else
              last->nextcard = pCardList;
            pCardList->nextcard  = NULL;
            pCardList->priorcard = last;
            last = pCardList;
                                       /* title of the bingo card - use the   */
                                       /* address of memory location          */
            sprintf( szText, "%s %d", szCard, i+1 );
            WinSetWindowText( pCardList->hwndCard, szText );
                                       /* position the cards, this will only  */
                                       /* work for up to ten bingo cards,     */
                                       /* need to write a more generic routine*/
            if(boardtype == HORIZONTAL)
            {
              if( i == 5 )
              {
                x = -cx;
                y = 0;
              }
            }
            else
            {
              if( i == 4 )
              {
                x = 0;
                y = cy;
              }
              if( i == 8 )
              {
                x = 0;
                y = 0;
              }
            }
            x = x + cx;

            WinSetWindowPos (pCardList->hwndCard, HWND_TOP, x, y, cx, cy,
                             SWP_SHOW | SWP_SIZE | SWP_MOVE | SWP_ACTIVATE);
            WinShowWindow( pCardList->hwndCard, TRUE );

          }
                                       /* Create virtual bingo cards here     */
          for( i = 0; i < numberofplayers; i++ )
          {
            pCardList = (PCARDLIST) malloc ( sizeof( CARDLIST ) );
            pcd = (PCARDDATA) malloc (sizeof (CARDDATA));

            if(debug)
              fprintf(wptrDebug, "UM_CREATECARDS: Created card %d.\n", pcd);

            pCardList->pcd = pcd;
            pCardList->pcd->whichPlayer = OTHER;
            fnGetCardNumbers( pCardList->pcd, 0 );
            pCardList->hwndCard = 0L;
            pCardList->hwndClient = 0L;

            if( lastOther == NULL )    /* build double linked list here ...   */
            {
              startOther = pCardList;
              lastOther  = pCardList;
            }
            else
              lastOther->nextcard = pCardList;
            pCardList->nextcard  = NULL;
            pCardList->priorcard = last;
            lastOther = pCardList;
          }
                                       /* give focus to the bingo board       */
          WinSetFocus( HWND_DESKTOP, hwnd );

          startbox = START;
          boolTime = TRUE;
          WinStartTimer( hab, hwnd, ID_TIMER, drawtime );

          if(debug)
           fprintf(wptrDebug, "UM_CREATECARDS completed.\n");
        }                              /* end case UM_CREATECARDS             */
        break;

     case UM_GAMEOVER:                 /* User message to indicate game is    */
        {                              /* over. The bingo cards window proc   */
                                       /* sends us this message when that card*/
          RECTL  rcl;                  /* Disable Pause, Stop and Next number */
          time_t endtime;              /* and enable Game options and Start   */

          WinQueryWindowRect( hwnd, &rcl );
          WinInvalidateRect( hwnd, &rcl, FALSE );

          WinEnableMenuItem( WinWindowFromID( WinQueryWindow( hwnd,
                             QW_PARENT ), FID_MENU ), IDM_STOP,
                             FALSE );
          WinEnableMenuItem( WinWindowFromID( WinQueryWindow( hwnd,
                             QW_PARENT ), FID_MENU ), IDM_PAUSE,
                             FALSE );
          WinEnableMenuItem( WinWindowFromID( WinQueryWindow( hwnd,
                             QW_PARENT ), FID_MENU ), IDM_NEXTNUM,
                             FALSE );
          WinEnableMenuItem( WinWindowFromID( WinQueryWindow( hwnd,
                             QW_PARENT ), FID_MENU ), IDM_GAMES,
                             TRUE );
          WinEnableMenuItem( WinWindowFromID( WinQueryWindow( hwnd,
                             QW_PARENT ), FID_MENU ), IDM_START,
                             TRUE );
          WinEnableMenuItem( WinWindowFromID( WinQueryWindow( hwnd,
                             QW_PARENT ), FID_MENU ), IDM_PRINT,
                             TRUE );

          startbox = ON;

          time(&endtime);              /* calculate the game time just played */
          gametime = difftime(endtime, startime);

          if(debug)
            fprintf(wptrDebug, "UM_GAMEOVER completed.\n");
        }                              /* end case UM_GAMEOVER                */
        break;


        case UM_PRINT:
        {
           BOOL flag;

           flag = (BOOL)LONGFROMMP(mp1);

           WinEnableMenuItem(WinWindowFromID(WinQueryWindow(hwnd, QW_PARENT),
                             FID_MENU), IDM_START, flag);
           WinEnableMenuItem(WinWindowFromID(WinQueryWindow(hwnd, QW_PARENT),
                             FID_MENU), IDM_PRINT, flag);
           WinEnableMenuItem(WinWindowFromID(WinQueryWindow(hwnd, QW_PARENT),
                             FID_MENU), IDM_GAMES, flag);
           WinEnableMenuItem(WinWindowFromID(WinQueryWindow(hwnd, QW_PARENT),
                             FID_MENU), IDM_FONT, flag);

           if(debug)
             fprintf(wptrDebug, "UM_PRINT completed.\n");
        }
        break;

     default:                          /* Let PM handle rest of messages      */
        mr = WinDefWindowProc (hwnd, msg, mp1, mp2);
        break;

    }                                  /* end main switch statement           */

 return mr;

}

/******************************************************************************/
/*                                                                            */
/* wpBingoWndProc: Window procedure which handles the processing for a bingo  */
/*                 card. WM_CREATE calls fnGetCardNumbers which generates     */
/*                 the random numbers for each bingo card. WM_PAINT calls     */
/*                 fnDrawCard to draw the bingo card on the client window.    */
/*                 WM_BUTTONxDOWN handles the processing when a user clicks   */
/*                 on a square in the bingo card client window. If valid      */
/*                 (square contains the number just drawn), a circular marker */
/*                 is drawn by calling fnDrawMarker. UM_AUTOPLAY is a user    */
/*                 defined message which handles automated play by finding    */
/*                 which square contains the just drawn number. UM_NEWGAME    */
/*                 is the other user defined message which creates new        */
/*                 random numbers for the bingo card (message sent by         */
/*                 wpClientWndProc).                                          */
/*                                                                            */
/* Calls: fnDrawCard        - draw bingo card on client window                */
/*        fnDrawMarker      - draw circular marker on bingo card square       */
/*        fnCheckBingo      - check for bingo                                 */
//
/*        fnDisplayBingo    - display message box indicating bingo            */
//
/*                                                                            */
/*  ÚÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄ¿                                 */
/*  ³   B   ³   I   ³   N   ³   G   ³   O   ³                                 */
/*  ÃÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄ´                                 */
/*  ³ (0,4) ³ (1,4) ³ (2,4) ³ (3,4) ³ (4,4) ³ pcd->bingono[x][y]  card number */
/*  ÃÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄ´ pcd->bingoit[x][y]  0 unmarked  */
/*  ³ (0,3) ³ (1,3) ³ (2,3) ³ (3,3) ³ (3,4) ³                     1 marked    */
/*  ÃÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄ´                   (NORMAL_CHIP) */
/*  ³ (0,2) ³ (1,2) ³ Free  ³ (3,2) ³ (2,4) ³   .                 2 missed    */
/*  ÃÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄ´   .               (O_CHIP)      */
/*  ³ (0,1) ³ (1,1) ³ (2,1) ³ (3,1) ³ (1,4) ³                     3 win       */
/*  ÃÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄ´ pcd->yline[1]     (WIN_CHIP)    */
/*  ³ (0,0) ³ (1,0) ³ (2,0) ³ (3,0) ³ (0,4) ³                                 */
/*  ÀÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÙ pcd->yline[0]                   */
/*  |       |                                                                 */
/*  pcb->    pcb->    ...                                                     */
/*  xline[0] xline[1]                                                         */
/*                                                                            */
/******************************************************************************/

MRESULT EXPENTRY wpBingoWndProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)

{

 MRESULT   mr = (MRESULT) FALSE;
 PCARDDATA pcd;

 switch (msg)
    {
     case WM_CREATE:                   /* Get mem pointer for bingo card data */
        {                              /* and place in window words.          */
          pcd = (PCARDDATA)PVOIDFROMMP(mp1);
          WinSetWindowPtr( hwnd, 0, pcd );
          WinSendMsg( hwnd, UM_POINTER, 0L, 0L );
          if(debug)
            fprintf(wptrDebug, "WM_CREATE completed for %d.\n", pcd);
        }
        break;                         /* store in bingo card data (pcd)      */

     case WM_PAINT:                    /* Redraw the bingo card window here   */
        {
          HPS   hps;
          RECTL rect;

          pcd = WinQueryWindowPtr( hwnd, 0 );

          hps = WinBeginPaint(hwnd, 0, 0);

          WinQueryWindowRect (hwnd, &rect);
          if(effects3d == ON)
            WinFillRect( hps, &rect, CLR_PALEGRAY );
          else
            WinFillRect( hps, &rect, SYSCLR_WINDOW );
          fnDrawCard( hps, pcd, 0, 0, rect.xRight, rect.yTop, TRUE );

          WinEndPaint(hps);
        }
        break;

     case WM_ERASEBACKGROUND:
        mr = (MRESULT) TRUE;
        break;

     case WM_CLOSE:                    /* User requested to close the bingo   */
        break;                         /* card window, cannot do that!        */

     case WM_MOUSEMOVE:
        pcd = WinQueryWindowPtr( hwnd, 0 );
        WinSetPointer( HWND_DESKTOP, pcd->hptr );
        break;

     case WM_BUTTON1DOWN:              /* User clicked on the bingo card      */
     case WM_BUTTON2DOWN:              /* window with either mouse button     */
        {

          if(bingostatus != BINGOSHOWN)/* Process only if bingo has not been  */
          {                            /* shown yet. Otherwise, ignore mouse  */

            USHORT i, x, y, found;
            POINTL ptlMouse;

            pcd = WinQueryWindowPtr( hwnd, 0 );

            ptlMouse.x = SHORT1FROMMP (mp1);/*determine mouse pointer location*/
            ptlMouse.y = SHORT2FROMMP (mp1);/*x and y coordinates needed      */
            found = 0;

            for( i = 0; i < 6; i++ )   /* check vertical position             */
            {
              if( ptlMouse.x > pcd->xline[i] && ptlMouse.x < pcd->xline[i+1] )
              {
                x = i;                 /* yup, found it, set flag, save       */
                i = 6;                 /* x position and leave                */
                found++;
              }
            }
            for( i = 0; i < 6; i++ )   /* check horizontal position           */
            {
              if( ptlMouse.y > pcd->yline[i] && ptlMouse.y < pcd->yline[i+1] )
              {
                y = i;                 /* yup found it, set flag, save y      */
                i = 6;                 /* position and leave                  */
                found++;
              }
            }

            if( x == 2 && y == 2 )     /* Free square, ignore by setting off  */
              found = 0;

            if( found == 2 && pcd->status != CARD_BINGO )
            {                          /* Mouse pointer was within one of the */
                                       /* bingo card squares, check if it     */
                                       /* matches the currently or previously */
              USHORT bx, by;           /* drawn number, if yes, draw marker   */
              fnGetXYValues( pcd->bingono[x][y], &bx, &by );
                                       /* check if current number or previous */
                                       /* drawn number and                    */
              if( ( ( pcd->bingono[x][y] == bingonumber ) ||      /* current  */
                    ( pbd->board[bx][by] == 0 ) ) &&              /* previous */
                                       /* ignore if markall is OFF and number */
                                       /* not part of game                    */
                ( markall == ON || pbd->bingogames[gametype][x][y] == 1 ) )
                {
                  fnProcessBingoSquare( hwnd, pcd, x, y );
                  if( sound == ON )
                    fnSound( SOUND_CHIP, 0, 0 );
                }
              else
                if( sound == ON )
                  fnSound( SOUND_ERROR, 0, 0 );
            }
            else
              if( sound == ON )
                fnSound( SOUND_ERROR, 0, 0 );

            WinSetFocus( HWND_DESKTOP, hwnd );
          }

        }                              /* end CASE WM_BUTTONxDOWN             */
        break;

     case WM_CHAR:                     /* allow Pause and Next Number keyboard*/
       {                               /* commands from bingo card procedure  */
         if(CHARMSG(&msg)->fs & KC_KEYUP)
           return(MRESULT)FALSE;
         if(CHARMSG(&msg)->fs & KC_VIRTUALKEY)
         {
           switch( CHARMSG(&msg)->vkey )
           {

             case VK_F4:               /* Pause, send msg to board client win */
               WinSendMsg( hwndBoard, WM_COMMAND,
                           MPFROM2SHORT( IDM_PAUSE, TRUE ), 0L );
               mr = (MRESULT)TRUE;
               break;
             case VK_F5:               /* Next number, msg to board client win*/
               WinSendMsg( hwndBoard, WM_COMMAND,
                           MPFROM2SHORT( IDM_NEXTNUM, TRUE ), 0L );
               mr = (MRESULT)TRUE;
               break;
           }
         }
       }                               /* end case WM_CHAR                    */
       break;

     case WM_DESTROY:                  /* Window about to be destroyed.       */
        WinShowWindow( WinQueryWindow( hwnd, QW_PARENT ), FALSE );

        if(debug)
         fprintf(wptrDebug, "Bingo Card WM_DESTROY called for %d.\n", pcd);

        break;

     case UM_AUTOPLAY:                 /* User define message for auto play   */
        {
          LONG y;

          pcd = WinQueryWindowPtr( hwnd, 0 );

          y = fnCheckCard( pcd, bingoletter, bingonumber );
          if( y != NOT_FOUND )
            fnProcessBingoSquare( hwnd, pcd, bingoletter, y );
          else
            pcd->status = CARD_NONE;

        }                              /* end case UM_AUTOPLAY                */
        break;

     case UM_NEWGAME:                  /* User defined message for new game   */
        {                              /* Create a new bingo card (actually   */
          RECTL rcl;                   /* just get new random card numbers)   */

          pcd = WinQueryWindowPtr( hwnd, 0 );
          fnGetCardNumbers( pcd, SHORT1FROMMP( mp1 ) );

          if(debug)
            fprintf(wptrDebug, "UM_NEWGAME completed for %d.\n", pcd);

          WinQueryWindowRect( hwnd, &rcl );
          WinInvalidateRect( hwnd, &rcl, FALSE );

        }                              /* end case UM_NEWGAME                 */
        break;

     case UM_MISSMARK:                 /* User defined message to determine   */
        {                              /* any bingo card numbers not marked   */
          USHORT bx, by, x, y;
          HPS    hps;
          RECTL  rcl;

          pcd = WinQueryWindowPtr( hwnd, 0 );

          hps = WinBeginPaint (hwnd, 0L, 0L);

          for(x = 0; x < 5; x++)       /* For each bingo square, check if the */
          {                            /* square is unmarked and if the bingo */
            for(y = 0; y < 5; y++)     /* number has been called, mark using  */
            {                          /* a transparent chip                  */
              if(x == 2 && y == 2);    /* ignore free square                  */
              else
              {
                fnGetXYValues( pcd->bingono[x][y], &bx, &by );
                if( pbd->board[bx][by] == 0  &&
                    pcd->bingoit[x][y] == NO_CHIP &&
                   (markall == ON || pbd->bingogames[gametype][x][y] == 1) )
                  {
                    pcd->bingoit[x][y] = O_CHIP;
                    fnDrawMarker( hps, pcd, x, y, O_CHIP );
                    rcl.xLeft   = pcd->xline[x];
                    rcl.xRight  = pcd->xline[x+1];
                    rcl.yBottom = pcd->yline[y];
                    rcl.yTop    = pcd->yline[y+1];
                    WinInvalidateRect( hwnd, &rcl, FALSE );
                  }
              }
            }
          }
          if(debug)
            fprintf(wptrDebug, "UM_MISSMARK completed for %d.\n", pcd);
          WinEndPaint(hps);
        }                              /* end case UM_MISSMARK                */
        break;

     case UM_POINTER:
        pcd = WinQueryWindowPtr( hwnd, 0 );
        pcd->hptr = pbd->hptr[pointer];
        break;

     default:                          /* Let PM handle rest of messages      */
        mr = WinDefWindowProc (hwnd, msg, mp1, mp2);
        break;

    }                                  /* end main switch statement           */

 return mr;

}

/******************************************************************************/
/*                    B I N G O   B O A R D   R O U T I N E S                 */
/******************************************************************************/
/******************************************************************************/
/*                                                                            */
/* fnDrawBoard: Function to draw the bingo board containing. Current bingo    */
/*              number is drawn in reverse video, drawn numbers are blanked   */
/*              out.                                                          */
/*                                                                            */
/* Calls: fnDrawStatusLine                                                    */
/*                                                                            */
/*                                                               yspace       */
/*  xRight/16  ³<ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ xl ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ>                ³          */
/*        ³   ³³      ³ xl/15³                     ³ yl/2³ yl/2³   V          */
/*       ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ  ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿ÄÄÄÄÄÄÄÄÄÄÄ  */
/* ÄÄÄÄÄÄ³ÚÄÄÄ¿ÚÄÄÄÄÄÄÂÄÄÄÄÄÄÂÄ  ÄÂÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄ¿ÚÄÄÄÄÄÂÄÄÄÄÄ¿³ÄÄÄÄÄ    A   */
/* A     ³³ B ³³0,0  1³1,0  2³    ³13,0 14³14,0 15³³     ³     ³³  A      ³   */
/* ³ ÄÄÄÄ³³   ³ÃÄÄÄÄÄÄÅÄÄÄÄÄÄÅÄ  ÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄ´³     ³ BIT ³³  ³      ³   */
/* ³ yl/5³³ I ³³0,1 16³1,1 17³    ³13,1 29³14,1 30³³00:00³ MAP ³³ yl/2    ³   */
/* ³ ÄÄÄÄ³³   ³ÃÄÄÄÄÄÄÅÄÄÄÄÄÄÅÄ  ÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄ´³     ³     ³³  V      ³   */
/* yl    ³³ N ³³0,2 31³1,2 32³    ³13,2 44³14,2 45³ÃÄÄÄÄÄÁÄÄÄÄÄ´³ÄÄÄÄÄ  yTop  */
/* ³     ³³   ³ÃÄÄÄÄÄÄÅÄÄÄÄÄÄÅÄ  ÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄ´³           ³³  A      ³   */
/* ³     ³³ G ³³0,3 46³1,3 47³    ³13,3 59³14,3 60³³           ³³  ³      ³   */
/* ³     ³³   ³ÃÄÄÄÄÄÄÅÄÄÄÄÄÄÅÄ  ÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄ´³    N 38   ³³ yl/2    ³   */
/* V     ³³ O ³³0,4 61³1,4 62³    ³13,4 74³14,4 75³³           ³³  V      ³   */
/* ÄÄÄÄÄÄ³ÀÄÄÄÙÀÄÄÄÄÄÄÁÄÄÄÄÄÄÁÄ  ÄÁÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÙÀÄÄÄÄÄÄÄÄÄÄÄÙ³ÄÄÄÄÄ    V   */
/*       ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ  ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙÄÄÄÄÄÄÄÄÄÄÄ  */
/*     Ä>³³<Ä xspace                                            ³             */
/*       ³<ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ xRight ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ>³             */
/*                                                                            */
/*       ÚÄÄÄÄÄÄÄÄ¿                               ÚÄÄ xboard                  */
/*       ³ x,y ## ³  pbd->board[x][y] = ##        ³                           */
/*       ÀÄÄÄÄÄÄÄÄÙ                               yboard                      */
/*                                                                            */
/*                                                                            */
/*       ³³<ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ xl ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ>³   Ú xspace                */
/*       ³³ yl/5 ³                                    V                       */
/*       ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿ÄÄÄÄÄÄÄÄÄÄÄ               */
/*  ÄÄÄÄÄ³ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿³ÄÄÄÄÄ   A                 */
/*       ³³                                       ³³  A     ³                 */
/*yTop/16³³   B       I       N       G       O   ³³  ³     ³                 */
/*       ³³                                       ³³        ³                 */
/*  ÄÄÄÄÄ³ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ³        ³                 */
/*  ÄÄÄÄÄ³ÚÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄ¿³        ³                 */
/*   A   ³³0,0   1³0,1  16³0,2  31³0,3  46³0,4  61³³        ³    ÚÄÄ yboard   */
/*   ³   ³ÃÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄ´³        ³    ³            */
/*   ³   ³³1,0   2³1,1  17³1,2  32³1,3  29³1,4  62³³        ³    xboard       */
/*   ³   ³ÃÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄ´³        ³                 */
/*       ³³2,0   3³2,1  18³2,2  33³2,3  44³2,4  63³³        ³                 */
/*   xl  ³ÃÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄ´³                          */
/*                                                        yTop                */
/*   ³                                                                        */
/*   ³   ³ÃÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄ´³        ³                 */
/*   ³   ³³13,0 14³13,1 29³13,2 44³13,3 59³13,4 74³³        ³                 */
/*   ³   ³ÃÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄ´³ÄÄÄÄÄÄ  ³                 */
/*   V   ³³14,0 15³14,1 30³14,2 45³14,3 74³14,4 75³³ xl/15  ³                 */
/*  ÄÄÄÄÄ³ÀÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÙ³ÄÄÄÄÄÄ  ³                 */
/*  ÄÄÄÄÄ³ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿³        ³                 */
/*   A   ³³                   ³                   ³³        ³                 */
/*   ³   ³³                   ³                   ³³        ³                 */
/*  yl/2 ³³       00:00       ³      BITMAP       ³³        ³                 */
/*   ³   ³³                   ³                   ³³        ³                 */
/*   V   ³³                   ³                   ³³        ³                 */
/*  ÄÄÄÄÄ³ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´³        ³                 */
/*   A   ³³                                       ³³        ³                 */
/*   ³   ³³                                       ³³        ³                 */
/*  yl/2 ³³                 N 38                  ³³        ³                 */
/*   ³   ³³                                       ³³        ³                 */
/*   V   ³³                                       ³³        ³                 */
/*  ÄÄÄÄÄ³ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ³        V                 */
/*       ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙÄÄÄÄÄÄÄÄÄÄÄ               */
/*     Ä>³³<Ä yspace                                                          */
/*       ³<ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ xRight ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ>³                          */
/*                                                                            */
/*                                                                            */
/******************************************************************************/

VOID fnDrawBoard( HWND hwnd )

{

  HPS         hps;
  RECTL       rect, orect;
  POINTL      point;
  LONG        i, x, y;
  SIZEF       sizfx;
  CHARBUNDLE  cbnd;
  LONG        xspace, yspace, xl, yl, sl;
  LONG        xpos[15][5];
  LONG        ypos[15][5];
  CHAR        bno[3];
  CHAR        bletno[5], elaptime[6];
  time_t      endtime;
  ULONG       timediff;

  hps = WinBeginPaint (hwnd, 0L, 0L);        /* get a presentation space      */

  WinQueryWindowRect (hwnd, &rect);          /* get size of client window     */
  if(effects3d)
    WinFillRect( hps, &rect, CLR_PALEGRAY );
  else
    WinFillRect( hps, &rect, SYSCLR_WINDOW );/* repaint with white background */

  fnDrawStatusLine( hab, hwnd );

  fnGetBoardSizes( rect, &xspace, &yspace, &xl, &yl );

  GpiSetLineWidth( hps, LINEWIDTH_THICK );

  if(boardtype == HORIZONTAL)
  {
    if(effects3d)
    {
      RECTL rectl;
      /* BINGO */
      rectl.xLeft   = xspace;
      rectl.yBottom = yspace;
      rectl.xRight  = xspace + rect.xRight/16;
      rectl.yTop    = yspace + yl;
      WinDrawBorder (hps, &rectl, effects3d, effects3d,
                     SYSCLR_BUTTONDARK, SYSCLR_BUTTONMIDDLE, DB_RAISED);
      /* Bingo squares */
      rectl.xLeft   = 2*xspace + rect.xRight/16;
      rectl.yBottom = yspace;
      rectl.xRight  = 2*xspace + rect.xRight/16 + xl;
      rectl.yTop    = yspace + yl;
      WinDrawBorder (hps, &rectl, effects3d, effects3d,
                     SYSCLR_BUTTONDARK, SYSCLR_BUTTONMIDDLE, DB_DEPRESSED);
      /* Number drawn */
      rectl.xLeft   = 3*xspace + rect.xRight/16 + xl;
      rectl.yBottom = yspace;
      rectl.xRight  = 3*xspace + rect.xRight/16 + xl + yl;
      rectl.yTop    = yspace + yl/2;
      WinDrawBorder (hps, &rectl, effects3d, effects3d,
                     SYSCLR_BUTTONDARK, SYSCLR_BUTTONMIDDLE, DB_RAISED);
      /* Time */
      rectl.xLeft   = 3*xspace + rect.xRight/16 + xl;
      rectl.yBottom = rect.yTop/2;
      rectl.xRight  = 3*xspace + rect.xRight/16 + xl + yl/2;
      rectl.yTop    = yspace + yl;
      WinDrawBorder (hps, &rectl, effects3d, effects3d,
                     SYSCLR_BUTTONDARK, SYSCLR_BUTTONMIDDLE, DB_RAISED);
    }
    else
    {
      /* BINGO */
      point.x = xspace;                /* Draw two boxes, top to contain the  */
      point.y = yspace;                /* red BINGO letters and below is the  */
      GpiMove( hps, &point );          /* border for the bingo board numbers  */
      point.x = xspace + rect.xRight/16;
      point.y = yspace + yl;
      GpiBox( hps, DRO_OUTLINE, &point, 0L, 0L );
      /* Bingo squares */
      point.x = 2*xspace + rect.xRight/16;
      point.y = yspace;
      GpiMove( hps, &point );
      point.x = 2*xspace + rect.xRight/16 + xl;
      point.y = yspace + yl;
      GpiBox( hps, DRO_OUTLINE, &point, 0L, 0L );
      /* Number drawn */
      point.x = 3*xspace + rect.xRight/16 + xl; /* Draw the three status boxes*/
      point.y = yspace;                       /* the right of the bingo board */
      GpiMove( hps, &point );                 /* 1) Game time, 2) Game type   */
      point.x = 3*xspace + rect.xRight/16 + xl + yl; /* bitmap, 3) Num drawn  */
      point.y = yspace + yl;
      GpiBox( hps, DRO_OUTLINE, &point, 0L, 0L );
      /* Time and bitmap area */
      point.x = 3*xspace + rect.xRight/16 + xl;
      point.y = rect.yTop/2;
      GpiMove( hps, &point );
      point.x = 3*xspace + rect.xRight/16 + xl + yl;
      GpiLine( hps, &point );
      point.x = 3*xspace + rect.xRight/16 + xl + yl/2;
      point.y = yspace + yl;
      GpiMove( hps, &point );
      point.y = rect.yTop/2;
      GpiLine( hps, &point );
    }
  }
  else
  {
    if(effects3d)
    {
      RECTL rectl;

      /* BINGO */
      rectl.xLeft   = yspace;
      rectl.yBottom = rect.yTop - rect.yTop/16 - xspace;
      rectl.xRight  = yspace + yl;
      rectl.yTop    = rect.yTop - xspace;
      WinDrawBorder (hps, &rectl, effects3d, effects3d,
                     SYSCLR_BUTTONDARK, SYSCLR_BUTTONMIDDLE, DB_RAISED);
      /* Bingo squares */
      rectl.xLeft   = yspace;
      rectl.yBottom = yl + 2*xspace;
      rectl.xRight  = yspace + yl;
      rectl.yTop    = yl + 2*xspace + xl;
      WinDrawBorder (hps, &rectl, effects3d, effects3d,
                     SYSCLR_BUTTONDARK, SYSCLR_BUTTONMIDDLE, DB_DEPRESSED);
      /* Number drawn */
      rectl.xLeft   = yspace;
      rectl.yBottom = xspace;
      rectl.xRight  = yspace + yl;
      rectl.yTop    = xspace + yl/2;
      WinDrawBorder (hps, &rectl, effects3d, effects3d,
                     SYSCLR_BUTTONDARK, SYSCLR_BUTTONMIDDLE, DB_RAISED);
      /* Time */
      rectl.xLeft   = yspace;
      rectl.yBottom = xspace + yl/2;
      rectl.xRight  = yspace + yl/2;
      rectl.yTop    = xspace + yl;
      WinDrawBorder (hps, &rectl, effects3d, effects3d,
                     SYSCLR_BUTTONDARK, SYSCLR_BUTTONMIDDLE, DB_RAISED);
    }
    else
    {
      /* BINGO */
      point.x = yspace;
      point.y = rect.yTop - rect.yTop/16 - xspace;
      GpiMove( hps, &point );
      point.x = yspace + yl;
      point.y = rect.yTop - xspace;
      GpiBox( hps, DRO_OUTLINE, &point, 0L, 0L );
      /* Bingo squares */
      point.x = yspace;
      point.y = yl + 2*xspace;
      GpiMove( hps, &point );
      point.x = yspace + yl;
      point.y = yl + 2*xspace + xl;
      GpiBox( hps, DRO_OUTLINE, &point, 0L, 0L );
      /* Number drawn */
      point.x = yspace;
      point.y = xspace;
      GpiMove( hps, &point );
      point.x = yspace + yl;
      point.y = xspace + yl;
      GpiBox( hps, DRO_OUTLINE, &point, 0L, 0L );
      /* Time and bitmap area */
      point.x = yspace;
      point.y = xspace + yl/2;
      GpiMove( hps, &point );
      point.x = yspace + yl;
      GpiLine( hps, &point );
      point.x = yspace + yl/2;
      point.y = xspace + yl/2;
      GpiMove( hps, &point );
      point.y = xspace + yl;
      GpiLine( hps, &point );
    }
  }

  GpiSetLineWidth( hps, LINEWIDTH_NORMAL );
                                       /* Draw type of bitmap in status area  */
  fnDrawBitmap( hps, rect, xspace, yspace, xl, yl, &orect );

  if(effects3d)
    GpiSetColor( hps, CLR_WHITE );

  for( i = 1; i < 15; i++ )            /* Draw the main bingo board squares   */
  {
    if(boardtype == HORIZONTAL)
    {                                  /* Draw vertical lines for squares     */
      point.x = 2 * xspace + rect.xRight/16 + i * xl / 15;
      point.y = yspace + effects3d;
      GpiMove( hps, &point );
      point.y = yspace + yl - effects3d;
      GpiLine( hps, &point );
    }
    else
    {                                  /* Draw horizontal lines for squares   */
      point.x = yspace + effects3d;
      point.y = 2*xspace + yl + i * xl / 15;
      GpiMove( hps, &point );
      point.x = yspace + yl - effects3d;
      GpiLine( hps, &point );
    }

  }

  for( i = 1; i < 5; i++ )             /* Draw the main bingo board squares   */
  {
    if(boardtype == HORIZONTAL)
    {                                  /* Draw horizontal lines for squares   */
      point.x = 2 * xspace + rect.xRight/16 + effects3d;
      point.y = yspace + i * yl / 5;
      GpiMove( hps, &point );
      point.x = 2 * xspace + rect.xRight/16 + xl - effects3d;
      GpiLine( hps, &point );
    }
    else
    {                                  /* Draw vertical lines for squares     */
      point.x = yspace + i * yl / 5;
      point.y = 2*xspace + yl + effects3d;
      GpiMove( hps, &point );
      point.y = 2*xspace + yl + xl - effects3d;
      GpiLine( hps, &point );
    }
  }

  if(effects3d)
    GpiSetColor( hps, CLR_BLACK );

  if(boardtype == HORIZONTAL)          /* Determine position where bingo board*/
  {
    for( x = 0; x < 15; x++ )          /* Numbers to be drawn in bingo board  */
    {                                  /* client window coordinates           */
      i = 5;
      for( y = 0; y < 5; y++ )
      {
        i--;
        xpos[x][i] = 6 * xspace + rect.xRight/16 + x * xl / 15;
        ypos[x][i] = 4 * yspace + y * yl / 5;
      }
    }
  }
  else
  {
    for( x = 0; x < 15; x++ )
    {
      for( y = 0; y < 5; y++ )
      {
        xpos[x][y] = yspace + y*yl/5 + 6*yspace;
        ypos[x][y] = 2*xspace + yl + xl - (x+1)*xl/15 + xspace;
      }
    }
  }

  if(boardtype == HORIZONTAL)
  {
    for( i = 0; i < 16; i++ )          /* Locate of lines so we know which    */
      pbd->xboard[i] = 2 * xspace + rect.xRight/16 + i * xl / 15;
    y = 6;                             /* ones to redraw                      */
    for( i = 0; i < 6; i++ )
    {
      y--;
      pbd->yboard[y] = yspace + i * yl / 5;
    }
  }
  else
  {
    for( i = 0; i < 6; i++ )
      pbd->yboard[i] = yspace + i * yl / 5;
    for( i = 0; i < 16; i ++ )
      pbd->xboard[i] = 2*xspace + yl + xl - i * xl / 15;
  }

  GpiCreateLogFont( hps, (PSTR8) NULL, pbd->lcid, &(pbd->fAttr) );
  GpiSetCharSet( hps, pbd->lcid );

  cbnd.lColor = CLR_RED;
  GpiSetAttrs( hps, PRIM_CHAR, CBB_COLOR, NULLHANDLE, &cbnd );

  if(bingonumber > 0)                  /* Write number drawn in status box    */
  {
    sizfx.cx = MAKEFIXED( yl/3, 0 );
    sizfx.cy = MAKEFIXED( yl/2, 0 );
    GpiSetCharBox( hps, &sizfx );
    if(boardtype == HORIZONTAL)
    {
      if((bingonumber < 10) || (bingoletter == 1))
        point.x = 3*xspace + rect.xRight/16 + xl + 9*xspace;
      else
        point.x = 3*xspace + rect.xRight/16 + xl + 5*xspace;
      point.y = 10*yspace;
    }
    else
    {
      if((bingonumber < 10) || (bingoletter == 1))
        point.x = yspace + 28*yspace;
      else
        point.x = yspace + 20*yspace;
      point.y = 5*xspace;
    }
    sprintf(bletno, "%c%3d", fnGetBingoLetter(bingoletter), bingonumber);
    GpiCharStringAt(hps, &point, 4, bletno);
  }

  if(boardtype == HORIZONTAL)
  {
    sizfx.cx = MAKEFIXED( xl / 15, 0 );/* Write BINGO running vertically in   */
    sizfx.cy = MAKEFIXED( yl / 5, 0 ); /* big bingo board                     */
    GpiSetCharBox( hps, &sizfx );
    point.x = 3 * xspace;
    point.y = 3*yspace + 4*yl/5;
    GpiCharStringAt( hps, &point, 1, "B" );
    point.x = 6 * xspace;
    point.y = 3*yspace + 3*yl/5;
    GpiCharStringAt( hps, &point, 1, "I" );
    point.x = 3 * xspace;
    point.y = 3*yspace + 2*yl/5;
    GpiCharStringAt( hps, &point, 1, "N" );
    point.y = 3*yspace + yl/5;
    GpiCharStringAt( hps, &point, 1, "G" );
    point.y = 3*yspace;
    GpiCharStringAt( hps, &point, 1, "O" );
  }
  else
  {
    sizfx.cx = MAKEFIXED( yl / 5, 0 );
    sizfx.cy = MAKEFIXED( rect.yTop / 16, 0 );
    GpiSetCharBox( hps, &sizfx );
    point.x = yspace + 2*yspace;
    point.y = 3*xspace + yl + xl + xspace;
    GpiMove( hps, &point );
    GpiCharString( hps, 12, "B  I  N G O\0" );
  }

  cbnd.lColor = CLR_BLACK;
  GpiSetAttrs( hps, PRIM_CHAR, CBB_COLOR, NULLHANDLE, &cbnd );

                                       /* Write game time in status box       */
  sizfx.cx = MAKEFIXED(yl/6, 0);
  sizfx.cy = MAKEFIXED(yl/6, 0);
  GpiSetCharBox(hps, &sizfx);
  if(boardtype == HORIZONTAL)
  {
    point.x = 3*xspace + rect.xRight/16 + xl + 2*xspace;
    point.y = yl/2 + 20*yspace;
  }
  else
  {
    point.x = yspace + 6*yspace;
    point.y = xspace + yl/2 + 10*xspace;
  }

  if(bingostatus != PREGAME)
  {
    if(bingostatus == INPROGRESS)
    {
      time(&endtime);
      timediff = (ULONG)difftime(endtime, startime);
    }
    else
      timediff = gametime;
    fnConvertTime(timediff, elaptime);
  }
  else
    strcpy(elaptime, "00:00");
  GpiCharStringAt(hps, &point, 5, elaptime);

  if(boardtype == HORIZONTAL)
  {
    sizfx.cx = MAKEFIXED( xl / 30, 0 );
    sizfx.cy = MAKEFIXED( yl / 5, 0 );
  }
  else
  {
    sizfx.cx = MAKEFIXED( yl / 10, 0 );
    sizfx.cy = MAKEFIXED( xl / 15, 0 );
  }
  GpiSetCharBox( hps, &sizfx );
  for( y = 0; y < 5; y++ )             /* write the bingo board numbers from  */
  {                                    /* 1 to 75 in proper sequence & column */
    for( x = 0; x < 15; x++ )
    {
      point.x = xpos[x][y];
      point.y = ypos[x][y];
      GpiMove( hps, &point );
      if( pbd->board[x][y] != 0 )      /* only numbers not drawn yet          */
      {                                /* current number, draw in reverse vid */
        if( pbd->board[x][y] == bingonumber )
        {
          cbnd.lColor = CLR_WHITE;
          GpiSetAttrs( hps, PRIM_CHAR, CBB_COLOR, NULLHANDLE, &cbnd );
          if(boardtype == HORIZONTAL)
          {
            rect.xLeft   = pbd->xboard[x];
            rect.xRight  = pbd->xboard[x+1];
            rect.yBottom = pbd->yboard[y+1];
            rect.yTop    = pbd->yboard[y];
          }
          else
          {
            rect.xLeft   = pbd->yboard[y];
            rect.xRight  = pbd->yboard[y+1];
            rect.yBottom = pbd->xboard[x+1];
            rect.yTop    = pbd->xboard[x];
          }
          WinFillRect( hps, &rect, CLR_BLACK );
        }
        _ultoa( pbd->board[x][y], bno, 10 );
        if( pbd->board[x][y] < 10 )    /* format the single numbers nicer by  */
        {                              /* adding a space before the number    */
          bno[1] = bno[0];
          bno[0] = ' ';
          bno[2] = '\0';
        }
        GpiCharString( hps, 3, bno );
        if( pbd->board[x][y] == bingonumber )
        {
          cbnd.lColor = CLR_BLACK;
          GpiSetAttrs( hps, PRIM_CHAR, CBB_COLOR, NULLHANDLE, &cbnd );
        }
      }
    }
  }

  WinEndPaint (hps);

}

/******************************************************************************/
/*                                                                            */
/* fnPositionBoard: Position the board either horizontally or vertically.     */
/*                                                                            */
/******************************************************************************/

VOID fnPositionBoard( HWND hwnd )
{

 RECTL rectlDesktop;
 LONG  x, y, cx, cy;

 WinShowWindow( hwnd, FALSE );         /* Hide the bingo board first          */

 WinQueryWindowRect (HWND_DESKTOP, &rectlDesktop);

 if(boardtype == HORIZONTAL)           /* Horizontal - Top of screen, full    */
 {                                     /* width and 1/3 height of Desktop     */
   x = 0;
   y = 2 * rectlDesktop.yTop / 3;
   cx = rectlDesktop.xRight;
   cy = rectlDesktop.yTop / 3;
 }
 else                                  /* Vertical - Left of screen, full     */
 {                                     /* height and 1/5 width of DeskTop     */
   x = 0;
   y = 0;
   cx = rectlDesktop.xRight / 5;
   cy = rectlDesktop.yTop;
 }
                                       /* Now, position the window on desktop */
 WinSetWindowPos (hwnd, HWND_TOP, x, y, cx, cy,
                  SWP_SHOW | SWP_SIZE | SWP_MOVE | SWP_ACTIVATE);
 WinShowWindow( hwnd, TRUE );          /* Show it after positioned            */

}

/******************************************************************************/
/*                                                                            */
/* fnDrawStatusLine: Write two line status, 1) Game type and 2) Status of     */
/*                   game (game not started, in progress or game over).       */
/*                                                                            */
/******************************************************************************/

VOID fnDrawStatusLine( HAB habx, HWND hwnd )
{

  CHAR szGame[TITLESIZE+1];
  CHAR szStatus[TEXTSIZE+1];
  CHAR szType[2*TITLESIZE+1];

  WinLoadString( habx, 0L, IDS_GAME, TITLESIZE, szGame );
  WinLoadString( habx, 0L, IDS_TITLEBAR, TITLESIZE, szTitle );

  switch( bingostatus )
  {
    case( PREGAME ):
      WinLoadString( habx, 0L, IDS_PREGAME, TEXTSIZE, szStatus );
      break;
    case( INPROGRESS ):
      WinLoadString( habx, 0L, IDS_INPROGRESS, TEXTSIZE, szStatus );
      break;
    case( GAMEOVER ):
      WinLoadString( habx, 0L, IDS_GAMEOVER, TEXTSIZE, szStatus );
      break;
    case( BINGOSHOWN ):
      WinLoadString( habx, 0L, IDS_GAMEOVER, TEXTSIZE, szStatus );
      break;
    default:
      break;
  }

  sprintf( szType, "%s %s", szGame, pbd->gametypes[gametype] );
  sprintf( szText, "%s - <%s - %s%d> %s (%d)", szTitle, szType,
           szMoney, prizemoney, szStatus, psd->gamesplayed );
  WinSetWindowText( WinQueryWindow( hwnd, QW_PARENT), szText );

}

/******************************************************************************/
/*                                                                            */
/* fnDrawBingoNumber: Draw next bingo number (random) and update the bingo    */
/*                    board and all bingo cards.                              */
/*                                                                            */
/******************************************************************************/

VOID fnDrawBingoNumber( HWND hwnd )
{

  PCARDLIST item;
  USHORT    x, y, ox, oy;
  ULONG     xRight, xLeft, yBottom, yTop, xspace, yspace;
  RECTL     rcl;
  BOOL      newgame = YES;
  LONG      checkFlag;
  LONG      numbersleft;
                                       /* Previously drawn number, get x,y val*/
  if( bingonumber != 0 )
  {
    fnGetXYValues( bingonumber, &ox, &oy ); /* blank out number               */
    pbd->board[ox][oy] = 0;            /* Set to NULL (drawn already)         */
    newgame = NO;
  }

  if(numbersdrawn > 74)                /* All numbers have been drawn and     */
  {                                    /* no bingo found yet!                 */
    fnDisplayBingo(hwnd, FALSE);
  }
  else
  {
                                       /* Draw the next bingo number here     */
    bingonumber = fnGetNextNumber( );
    numbersdrawn++;
    fnGetXYValues( bingonumber, &x, &y );
    bingoletter = y;
    numbersleft = 75 - numbersdrawn;   /* Send numbers left info to chatter   */
    fnChatDrawNumber( bingonumber, bingoletter, numbersleft );

    if(debug)
    {
      sprintf( szText, "Number drawn: %c %2d %2d.",
               fnGetBingoLetter(bingoletter),
               bingonumber, numbersdrawn );
//    WinMessageBox (HWND_DESKTOP, 0L, szText,
//                   "Debug", 1, MB_OK | MB_MOVEABLE);
      fprintf(wptrDebug, "%s\n", szText);
    }

    if( sound == ON )
    {
      WinEnableMenuItem( WinWindowFromID( WinQueryWindow( hwnd,
                         QW_PARENT ), FID_MENU ), IDM_NEXTNUM, FALSE );
      fnSound( SOUND_DRAWNUM, bingoletter, bingonumber );
      WinEnableMenuItem( WinWindowFromID( WinQueryWindow( hwnd,
                         QW_PARENT ), FID_MENU ), IDM_NEXTNUM, TRUE );
    }

    if( newgame == NO ) /* Just update the square old number (blank) and the  */
    {                   /* square for the current number (reverse video).     */
                        /* Finally, invalidate the window to force fnDrawBoard*/
      if(boardtype == HORIZONTAL)
      {
        rcl.xLeft   = pbd->xboard[ox];
        rcl.xRight  = pbd->xboard[ox+1];
        rcl.yBottom = pbd->yboard[oy+1];
        rcl.yTop    = pbd->yboard[oy];
      }
      else
      {
        rcl.xLeft   = pbd->yboard[oy];
        rcl.xRight  = pbd->yboard[oy+1];
        rcl.yBottom = pbd->xboard[ox+1];
        rcl.yTop    = pbd->xboard[ox];
      }
      WinInvalidateRect( hwnd, &rcl, FALSE );
    }

    if(boardtype == HORIZONTAL)
    {
      rcl.xLeft   = pbd->xboard[x];
      rcl.xRight  = pbd->xboard[x+1];
      rcl.yBottom = pbd->yboard[y+1];
      rcl.yTop    = pbd->yboard[y];
    }
    else
    {
      rcl.xLeft   = pbd->yboard[y];
      rcl.xRight  = pbd->yboard[y+1];
      rcl.yBottom = pbd->xboard[x+1];
      rcl.yTop    = pbd->xboard[x];
    }
    WinInvalidateRect( hwnd, &rcl, FALSE );

    WinQueryWindowRect( hwnd, &rcl );
    xLeft   = rcl.xLeft;
    xRight  = rcl.xRight;
    yTop    = rcl.yTop;
    yBottom = rcl.yBottom;

    /* Invalidate number drawn */
    if(boardtype == HORIZONTAL)
    {
      xspace  = rcl.xRight/200;
      yspace  = rcl.yTop/100;
      rcl.xLeft   = xRight - yTop + xspace;
      rcl.xRight  = xRight - 2*xspace;
      rcl.yBottom = yBottom + 2*yspace;
      rcl.yTop    = yTop/2 - yspace;
    }
    else
    {
      xspace  = rcl.yTop/200;
      yspace  = rcl.xRight/100;
      rcl.xLeft   = yspace;
      rcl.xRight  = xRight - yspace;
      rcl.yBottom = xspace;
      rcl.yTop    = xRight/2;
    }
    WinInvalidateRect( hwnd, &rcl, FALSE );

    /* Invalidate bingo time */
    if(boardtype == HORIZONTAL)
    {
      rcl.xRight  = xRight - yTop/2 - xspace;
      rcl.yBottom = yTop/2 + yspace;
      rcl.yTop    = yTop - 2*yspace;
    }
    else
    {
      rcl.xRight  = xRight/2 - yspace;
      rcl.yBottom = xRight/2 + xspace;
      rcl.yTop    = xRight + xspace;
    }
    WinInvalidateRect( hwnd, &rcl, FALSE );

    if( autoplay == ON )               /* For automated play, post user       */
    {                                  /* message to the client window        */
                                       /* for all bingo cards                 */
      item = start;
      while( item )                    /* Loop thru all bingo cards here      */
      {                                /* in the linked list                  */
        item->pcd->status = CARD_CHECK;
        WinSendMsg( item->hwndCard, UM_AUTOPLAY, 0L, 0L );
        item = (PCARDLIST) item->nextcard;
      }
    }
                                       /* Now process the virtual bingo cards */
    item = startOther;                 /* Virtually mark the cards and then   */
    while( item )                      /* check for bingo also                */
    {
      if(item->hwndCard != 0)
      {
        item->pcd->status = CARD_CHECK;
        WinSendMsg( item->hwndCard, UM_AUTOPLAY, 0L, 0L );
      }
      else
      {
        y = fnCheckCard( item->pcd, bingoletter, bingonumber );
        if( y != NOT_FOUND )           /* Bingo number found in bingo card    */
        {                              /* Mark it with a normal chip          */
          item->pcd->bingoit[bingoletter][y] = NORMAL_CHIP;
          checkFlag = fnCheckBingo( item->pcd );
          switch( checkFlag )
          {
            case( FOUND_BINGO ):
            {                          /* this card has a bingo! Hurray!      */
              item->pcd->status = CARD_BINGO;/* set card bingo indicator to Y */
              item->pcd->waitnumber = FOUND_BINGO;

              psd->otherbingos    = psd->otherbingos + 1;
              psd->othermoneywon  = psd->othermoneywon  + prizemoney;

              bingostatus = GAMEOVER;  /* set flag to indicate bingo found    */
                                       /* already found, game completed!      */
              fnChatProcess(CHAT_BINGO, bingonumber, 0L, 0L);
//            WinPostMsg( hwndBoard, WM_TIMER, MPFROMSHORT(ID_TIMER), 0L );
            }
            break;

            case( NO_BINGO ):
              item->pcd->status = CARD_NONE;
              item->pcd->waitnumber = NO_BINGO;
              break;

            default:
              item->pcd->status = CARD_NONE;
              if(item->pcd->waitnumber == NO_BINGO)
              {
                 item->pcd->waitnumber = checkFlag;
                 fnChatProcess(CHAT_WAIT, bingonumber, OTHER, checkFlag);
              }
              break;

          }
        }
      }
      item = (PCARDLIST) item->nextcard;
    }
    /* Check if we almost had a bingo. Loop thru all bingo cards, if bingo */
    /* card is waiting for a number, check if that number is +1 or -1 the  */
    /* bingo number just drawn (bingonumber), if yes, display chatter      */
    item = start;
    while( item )                      /* Loop thru all bingo cards here      */
    {                                  /* in the linked list                  */
      if(item->pcd->waitnumber != NO_BINGO)
      {
        if( (item->pcd->waitnumber+1 == bingonumber) ||
            (item->pcd->waitnumber-1 == bingonumber) )
          fnChatProcess(CHAT_WAITCLOSE, bingonumber, 0L, 0L);
      }
      item = (PCARDLIST) item->nextcard;
    }
    item = startOther;
    while( item )                      /* Loop thru all bingo cards here      */
    {                                  /* in the linked list                  */
      if(item->pcd->waitnumber != NO_BINGO)
      {
        if( (item->pcd->waitnumber+1 == bingonumber) ||
            (item->pcd->waitnumber-1 == bingonumber) )
          fnChatProcess(CHAT_WAITCLOSE, bingonumber, 0L, 0L);
      }
      item = (PCARDLIST) item->nextcard;
    }

  }
}

/******************************************************************************/
/*                                                                            */
/* fnGetBoardSizes: Determine the bingo board sizes.                          */
/*                                                                            */
/******************************************************************************/

VOID fnGetBoardSizes( RECTL rect, LONG *xspace, LONG *yspace, LONG *xl,
                      LONG *yl )

{

  if(boardtype == HORIZONTAL)
  {
    *yspace = rect.yTop   / 100;
    *yl = rect.yTop - 2 * *yspace;
    *xspace = rect.xRight / 200;
    *xl = rect.xRight - 4 * *xspace - rect.xRight/16 - *yl;
  }
  else
  {
    *yspace = rect.xRight / 100;
    *yl = rect.xRight - 2 * *yspace;
    *xspace = rect.yTop / 200;
    *xl = rect.yTop - 4 * *xspace - rect.yTop/16 - *yl;
  }

}

/******************************************************************************/
/*                                                                            */
/* fnDrawBitmap: Draw the game type bitmap on the bingo board.                */
/*                                                                            */
/******************************************************************************/

VOID fnDrawBitmap( HPS hps, RECTL rect, LONG xspace, LONG yspace, LONG xl,
                   LONG yl, PRECTL orcl )
{

  HBITMAP     hbm;
  POINTL      point;

  if(boardtype == HORIZONTAL)
  {
    hbm = GpiLoadBitmap(hps, 0L, pbd->bitmaps[gametype], yl/2-2*yspace,
                        yl/2-2*yspace);
    point.x = 3 * xspace + rect.xRight/16 + xl + yl/2 + yspace;
    point.y = rect.yTop/2 + yspace;
  }
  else
  {
    hbm = GpiLoadBitmap(hps, 0L, pbd->bitmaps[gametype], yl/2-2*xspace,
                        yl/2-2*xspace);
    point.x = yspace + yl/2 + xspace;
    point.y = xspace + yl/2 + xspace;
  }
  WinDrawBitmap (hps, hbm, 0L, &point, 0L, 0L, DBM_NORMAL);
  GpiDeleteBitmap (hbm);

  if(boardtype == HORIZONTAL)
  {
    orcl->xLeft   = 3 * xspace + rect.xRight/16 + xl + yl/2 + yspace;
    orcl->yBottom = rect.yTop/2 + yspace;
    orcl->xRight  = rect.xRight + yl/2-2*yspace;
    orcl->yTop    = orcl->yBottom + yl/2-2*yspace;
  }
  else
  {
    orcl->xLeft   = yspace + yl/2 + xspace;
    orcl->yBottom = xspace + yl/2 + xspace;
    orcl->xRight  = xspace + yl - xspace;
    orcl->yTop    = yspace + yl - xspace;
  }

}

/******************************************************************************/
/*                                                                            */
/* fnGetBoardNumbers: Initialize bingo board numbers from 1 to 75 in          */
/*                    pbd->board[x][y].                                       */
/*                                                                            */
/******************************************************************************/

VOID fnGetBoardNumbers( VOID )

{

  LONG x, y, num;

  num = 0;
  for( y = 0; y < 5; y++ )
  {
    for( x = 0; x < 15; x++ )
    {
      num++;
      pbd->board[x][y] = num;
    }
  }
  strcpy( pbd->letter, "BINGO" );

  if(debug)
    fprintf(wptrDebug, "Bingo board numbers initialized.\n");

}

/******************************************************************************/
/*                                                                            */
/* fnGetNextNumber: Draw a random number between 1 and 75 which has not been  */
/*                  drawn before. Calls GetRandom( ) and fnGetXYValues( ).    */
/*                  Returns an unsigned long (ULONG) number.                  */
/*                                                                            */
/******************************************************************************/

ULONG fnGetNextNumber( VOID )
{

  USHORT x, y;
  USHORT number;

  switch(numbersdrawn)
  {
    case 74:                           /* One more number not drawn, find it  */
      {                                /* instead of using the random number  */
        for( y = 0; y < 5; y++ )       /* generator                           */
        {
          for( x = 0; x < 15; x++ )
          {
            if( pbd->board[x][y] != 0 )/* Number not drawn yet found here     */
            {
              number = pbd->board[x][y];
              x = 15;                  /* Break from both loops               */
              y = 5;
            }
          }
        }
      }
      break;

    case 75:                           /* All numbers have been drawn, do     */
      number = 0;                      /* nothing - game must be over!        */
      break;

    default:                           /* numbersdrawn < 74 is normal, use    */
      {                                /* random number generator to draw the */
                                       /* next number between 1 and 75        */
        number = (USHORT) GetRandom( 1, 75 );
        fnGetXYValues( number, &x, &y );
        while( pbd->board[x][y] == 0 ) /* Make sure the number drawn has not  */
        {                              /* been drawn before, if yes draw again*/
          number = (USHORT) GetRandom( 1, 75 );
          fnGetXYValues( number, &x, &y );
        }
      }
      break;
  }                                    /* end switch                          */

  return( number );

}

/******************************************************************************/
/*                                                                            */
/* fnCreateBingoGames: Initialize all bingo games. pbd.bingogames[i][x][y]    */
/*                     must contain a 1 if the square must be marked. Other-  */
/*                     wise, it must be 0.                                    */
/*                                                                            */
/******************************************************************************/

VOID fnCreateBingoGames( VOID )
{

  LONG i, x, y;

  for( i = 2; i < BINGOGAMES; i++ )    /* initialize all bingo games here     */
    for( x = 0; x < 5; x++ )           /* set flag to FALSE (0), don't do     */
      for( y = 0; y < 5; y++ )         /* classic (i=0) and all numbers (i=1) */
        pbd->bingogames[i][x][y] = 0;  /* bingo games since flag set to TRUE  */

  for( i = 0; i < 2; i++ )             /* Classic and all numbers bingo, set  */
    for( x = 0; x < 5; x++ )           /* flag to TRUE (0) - all numbers may  */
      for( y = 0; y < 5; y++ )         /* be marked                           */
        pbd->bingogames[i][x][y] = 1;
  pbd->bingogames[0][2][2] = 0;        /* free square, set back to FALSE      */
  pbd->bingogames[1][2][2] = 0;
  WinLoadString(hab, 0L, IDS_CLASSIC, TITLESIZE, pbd->gametypes[0]);
  pbd->bitmaps[0] = IDBMP_CLASSIC;
  WinLoadString(hab, 0L, IDS_ALLNO, TITLESIZE, pbd->gametypes[1]);
  pbd->bitmaps[1] = IDBMP_ALLNO;

  if ( BINGOGAMES > 2 )
  {
    pbd->bingogames[2][0][0] = 1;        /* X bingo */
    pbd->bingogames[2][0][4] = 1;
    pbd->bingogames[2][4][0] = 1;
    pbd->bingogames[2][4][4] = 1;
    pbd->bingogames[2][1][3] = 1;
    pbd->bingogames[2][3][1] = 1;
    pbd->bingogames[2][1][1] = 1;
    pbd->bingogames[2][3][3] = 1;
    WinLoadString(hab, 0L, IDS_X, TITLESIZE, pbd->gametypes[2]);
    pbd->bitmaps[2] = IDBMP_X;
  }

  if ( BINGOGAMES > 3 )
  {
    pbd->bingogames[3][0][2] = 1;        /* Diamond bingo */
    pbd->bingogames[3][1][1] = 1;
    pbd->bingogames[3][2][0] = 1;
    pbd->bingogames[3][3][1] = 1;
    pbd->bingogames[3][4][2] = 1;
    pbd->bingogames[3][3][3] = 1;
    pbd->bingogames[3][2][4] = 1;
    pbd->bingogames[3][1][3] = 1;
    WinLoadString(hab, 0L, IDS_DIAMOND, TITLESIZE, pbd->gametypes[3]);
    pbd->bitmaps[3] = IDBMP_DIAMOND;
  }

  if ( BINGOGAMES > 4 )
  {
    pbd->bingogames[4][0][0] = 1;        /* Four corners bingo */
    pbd->bingogames[4][0][4] = 1;
    pbd->bingogames[4][4][0] = 1;
    pbd->bingogames[4][4][4] = 1;
    WinLoadString(hab, 0L, IDS_4CORNERS, TITLESIZE, pbd->gametypes[4]);
    pbd->bitmaps[4] = IDBMP_4CORNERS;
  }

  if ( BINGOGAMES > 5 )
  {
    pbd->bingogames[5][1][1] = 1;        /* Center or Cross (+) bingo */
    pbd->bingogames[5][2][1] = 1;
    pbd->bingogames[5][3][1] = 1;
    pbd->bingogames[5][1][2] = 1;
    pbd->bingogames[5][3][2] = 1;
    pbd->bingogames[5][1][3] = 1;
    pbd->bingogames[5][2][3] = 1;
    pbd->bingogames[5][3][3] = 1;
    WinLoadString(hab, 0L, IDS_CENTER, TITLESIZE, pbd->gametypes[5]);
    pbd->bitmaps[5] = IDBMP_CENTER;
  }

  if ( BINGOGAMES > 6 )                 /* Outside square */
  {
    for( x = 0; x < 5; x++ )
    {
      pbd->bingogames[6][x][4] = 1;
      pbd->bingogames[6][x][0] = 1;
    }
    pbd->bingogames[6][0][1] = 1;
    pbd->bingogames[6][0][2] = 1;
    pbd->bingogames[6][0][3] = 1;
    pbd->bingogames[6][4][1] = 1;
    pbd->bingogames[6][4][2] = 1;
    pbd->bingogames[6][4][3] = 1;
    WinLoadString(hab, 0L, IDS_OUTSQUARE, TITLESIZE, pbd->gametypes[6]);
    pbd->bitmaps[6] = IDBMP_OUTSQUARE;
  }

  if ( BINGOGAMES > 7 )
  {
    pbd->bingogames[7][1][3] = 1;        /* Inside square */
    pbd->bingogames[7][2][3] = 1;
    pbd->bingogames[7][3][3] = 1;
    pbd->bingogames[7][1][2] = 1;
    pbd->bingogames[7][3][2] = 1;
    pbd->bingogames[7][1][1] = 1;
    pbd->bingogames[7][2][1] = 1;
    pbd->bingogames[7][3][1] = 1;
    WinLoadString(hab, 0L, IDS_INSQUARE, TITLESIZE, pbd->gametypes[7]);
    pbd->bitmaps[7] = IDBMP_INSQUARE;
  }

  if ( BINGOGAMES > 8 )
  {
    pbd->bingogames[8][0][4] = 1;        /* T bingo */
    pbd->bingogames[8][1][4] = 1;
    pbd->bingogames[8][2][4] = 1;
    pbd->bingogames[8][3][4] = 1;
    pbd->bingogames[8][4][4] = 1;
    pbd->bingogames[8][2][3] = 1;
    pbd->bingogames[8][2][1] = 1;
    pbd->bingogames[8][2][0] = 1;
    WinLoadString(hab, 0L, IDS_T, TITLESIZE, pbd->gametypes[8]);
    pbd->bitmaps[8] = IDBMP_T;
  }

  if ( BINGOGAMES > 9 )
  {
    pbd->bingogames[9][0][4] = 1;        /* Y bingo */
    pbd->bingogames[9][4][4] = 1;
    pbd->bingogames[9][1][3] = 1;
    pbd->bingogames[9][3][3] = 1;
    pbd->bingogames[9][2][1] = 1;
    pbd->bingogames[9][2][0] = 1;
    WinLoadString(hab, 0L, IDS_Y, TITLESIZE, pbd->gametypes[9]);
    pbd->bitmaps[9] = IDBMP_Y;
  }

  if ( BINGOGAMES > 10 )
  {
    for( x = 0; x < 5; x++ )
    {
      pbd->bingogames[10][x][4] = 1;
      pbd->bingogames[10][x][0] = 1;
    }
    pbd->bingogames[10][1][1] = 1;        /* Z bingo */
    pbd->bingogames[10][3][3] = 1;
    WinLoadString(hab, 0L, IDS_Z, TITLESIZE, pbd->gametypes[10]);
    pbd->bitmaps[10] = IDBMP_Z;
  }

  if ( BINGOGAMES > 11 )
  {
    for( y = 0; y < 5; y++ )
    {
      pbd->bingogames[11][0][y] = 1;
      pbd->bingogames[11][4][y] = 1;
    }
    pbd->bingogames[11][1][4] = 1;        /* N bingo */
    pbd->bingogames[11][3][1] = 1;
    WinLoadString(hab, 0L, IDS_N, TITLESIZE, pbd->gametypes[11]);
    pbd->bitmaps[11] = IDBMP_N;
  }

  if ( BINGOGAMES > 12 )
  {
    for( y = 0; y < 5; y++ )
    {
      pbd->bingogames[12][0][y] = 1;
      pbd->bingogames[12][4][y] = 1;
    }
    pbd->bingogames[12][1][2] = 1;        /* H bingo */
    pbd->bingogames[12][3][2] = 1;
    WinLoadString(hab, 0L, IDS_H, TITLESIZE, pbd->gametypes[12]);
    pbd->bitmaps[12] = IDBMP_H;
  }

  if ( BINGOGAMES > 13 )
  {
    for( y = 0; y < 5; y++ )
    {
      pbd->bingogames[13][0][y] = 1;
      pbd->bingogames[13][4][y] = 1;
    }
    pbd->bingogames[13][1][3] = 1;        /* M bingo */
    pbd->bingogames[13][3][3] = 1;
    WinLoadString(hab, 0L, IDS_M, TITLESIZE, pbd->gametypes[13]);
    pbd->bitmaps[13] = IDBMP_M;
  }

  if ( BINGOGAMES > 14 )
  {
    for( x = 0; x < 5; x++ )
      pbd->bingogames[14][x][4] = 1;
    pbd->bingogames[14][1][1] = 1;        /* Lucky 7 bingo */
    pbd->bingogames[14][3][3] = 1;
    WinLoadString(hab, 0L, IDS_LUCKY7, TITLESIZE, pbd->gametypes[14]);
    pbd->bitmaps[14] = IDBMP_LUCKY7;
  }

  if ( BINGOGAMES > 15 )
  {
    for( x = 0; x < 5; x++ )              /* Hardaway bingo, all numbers may  */
      for( y = 0; y < 5; y++ )            /* be marked                        */
        pbd->bingogames[15][x][y] = 1;
    WinLoadString(hab, 0L, IDS_HARDAWAY, TITLESIZE, pbd->gametypes[15]);
    pbd->bitmaps[15] = IDBMP_HARDAWAY;
  }

  if(debug)
    fprintf(wptrDebug, "All bingo games created.\n");
}


/******************************************************************************/
/*                                                                            */
/* fnUpdateScores: Update score information (psd).                            */
/*                                                                            */
/******************************************************************************/

VOID fnUpdateScores(BOOL type)

{

  if(type)
  {
    psd->gamesplayed      = psd->gamesplayed - 1;
    psd->cardsplayed      = psd->cardsplayed - numberofcards;
    psd->othercardsplayed = psd->othercardsplayed - numberofplayers;
    psd->moneyleft        = psd->moneyleft + numberofcards*CARD_COST;
  }
  else
  {
    psd->gamesplayed      = psd->gamesplayed + 1;
    psd->cardsplayed      = psd->cardsplayed + numberofcards;
    psd->othercardsplayed = psd->othercardsplayed + numberofplayers;
    psd->moneyleft        = psd->moneyleft - numberofcards*CARD_COST;

    prizemoney = CARD_COST*(numberofcards + numberofplayers);
  }
}


/******************************************************************************/
/*                                                                            */
/* fnInitializeScores: Initialize score values to 0.                          */
/*                                                                            */
/******************************************************************************/

VOID fnInitializeScores( VOID )

{

  psd->otherbingos      = 0;
  psd->othermoneywon    = 0;
  psd->othercardsplayed = 0;
  psd->gamesplayed      = 0;
  psd->cardsplayed      = 0;
  psd->bingos           = 0;
  psd->moneyleft        = MONEY_START;
  psd->moneywon         = 0;

}

/******************************************************************************/
/*                                                                            */
/* fnRedrawWindows: Invalidate all windows (board and cards) so they are      */
/*                  redrawn.                                                  */
/*                                                                            */
/******************************************************************************/

BOOL fnRedrawWindows( HWND hwnd )

{

  PCARDLIST item;
  RECTL     rcl;

  /* Bingo board */
  WinQueryWindowRect( hwnd, &rcl );
  WinInvalidateRect( hwnd, &rcl, FALSE );

  /* Bingo cards */
  item = start;
  while( item )
  {
    WinQueryWindowRect( item->hwndClient, &rcl );
    WinInvalidateRect( item->hwndClient, &rcl, FALSE );
    item = (PCARDLIST) item->nextcard;
  }

  return( TRUE );

}

/******************************************************************************/
/*                    B I N G O  C A R D  R O U T I N E S                     */
/******************************************************************************/
/******************************************************************************/
/*                                                                            */
/* fnDrawCard: Draw the bingo card.                                           */
/*                                                                            */
/* Calls: fnDrawMarker                                                        */
/*                                                                            */
/*                                                                            */
/*  yspace                                                                    */
/*    ÄÄÄ   ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿ ÄÄÄÄÄÄÄ               */
/*    ÄÄÄ   ³ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿³       A               */
/*  yTop/6  ³³   B       I       N       G       O   ³³       ³               */
/*    ÄÄÄ   ³ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ³       ³               */
/*    ÄÄÄ   ³ÚÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄ¿³       ³               */
/*     A    ³³ (0,4) ³ (1,4) ³ (2,4) ³ (3,4) ³ (4,4) ³³       ³               */
/*     |    ³ÃÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄ´³       ³               */
/*     |    ³³ (0,3) ³ (1,3) ³ (2,3) ³ (3,3) ³ (3,4) ³³       ³               */
/*          ³ÃÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄ´³                       */
/*    yl    ³³ (0,2) ³ (1,2) ³ Free  ³ (3,2) ³ (2,4) ³³      height           */
/*          ³ÃÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄ´³ ÄÄ                    */
/*     |    ³³ (0,1) ³ (1,1) ³ (2,1) ³ (3,1) ³ (1,4) ³³ yl/5  ³               */
/*     |    ³ÃÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄ´³ ÄÄ    ³               */
/*     V    ³³ (0,0) ³ (1,0) ³ (2,0) ³ (3,0) ³ (0,4) ³³       ³               */
/*    ÄÄÄ   ³ÀÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÙ³       V               */
/*    ÄÄÄ   ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ ÄÄÄÄÄÄÄ               */
/*    (left,bottom)  ³  xl/5 ³                                                */
/*                                                                            */
/* xspace Ä>³³<ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ xl ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ>³³<Ä xspace              */
/*          ³<ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ width  ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ>³                       */
/*                                                                            */
/******************************************************************************/

VOID fnDrawCard( HPS hps, PCARDDATA pcd, LONG left, LONG bottom,
                 LONG width, LONG height, BOOL type )

{

  POINTL      point;
  LONG        i, x, y;
  SIZEF       sizfx;
  FATTRS      fat;
  CHARBUNDLE  cbnd;
  LONG        xspace, yspace, xl, yl;
  LONG        xpos[5][5];
  LONG        ypos[5][5];
  CHAR        bno[3];
  RECTL       rectl;

  xspace = width / 100;                        /* spacer in x-direction       */
  yspace = height / 100;                       /* spacer in y-direction       */
  xl = width - 2 * xspace;                     /* total width of card         */
  yl = height - ( 3 * yspace + height / 6 );   /* total length of card        */

  GpiSetLineWidth( hps, LINEWIDTH_THICK );

  if(effects3d)
  {
    rectl.xLeft   = xspace + left;           /* draw the bingo card here ...  */
    rectl.yBottom = yspace + bottom;
    rectl.xRight  = (xspace + xl) + left;
    rectl.yTop    = (yspace + yl) + bottom;
    WinDrawBorder (hps, &rectl, effects3d, effects3d,
                   SYSCLR_BUTTONDARK, SYSCLR_BUTTONMIDDLE, DB_DEPRESSED);

    rectl.xLeft   = xspace + left;           /* draw box where BINGO letters  */
    rectl.yBottom = (2 * yspace + yl) + bottom; /* are drawn in red           */
    rectl.xRight  = (xspace + xl) + left;
    rectl.yTop    = (2 * yspace + yl + height / 6) + bottom;
    WinDrawBorder (hps, &rectl, effects3d, effects3d,
                   SYSCLR_BUTTONDARK, SYSCLR_BUTTONMIDDLE, DB_RAISED);
  }
  else
  {
    point.x = xspace + left;                 /* draw the bingo card here ...  */
    point.y = yspace + bottom;
    GpiMove( hps, &point );
    point.x = (xspace + xl) + left;
    point.y = (yspace + yl) + bottom;
    GpiBox( hps, DRO_OUTLINE, &point, 0L, 0L );

    point.x = xspace + left;                 /* draw box where BINGO letters  */
    point.y = (2 * yspace + yl) + bottom;    /* are drawn in red              */
    GpiMove( hps, &point );
    point.x = (xspace + xl) + left;
    point.y = (2 * yspace + yl + height / 6) + bottom;
    GpiBox( hps, DRO_OUTLINE, &point, 0L, 0L );
  }

  GpiSetLineWidth( hps, LINEWIDTH_NORMAL );

  if(effects3d)
    GpiSetColor( hps, CLR_WHITE );

  for( i = 1; i < 5; i++ )             /* draw vertical lines */
  {
    point.x = (xspace + i * xl / 5) + left + effects3d;
    point.y = yspace + bottom; + effects3d;
    GpiMove( hps, &point );
    point.y = (yspace + yl) + bottom - effects3d;
    GpiLine( hps, &point );
  }

  for( i = 1; i < 5; i++ )             /* draw horizontal lines */
  {
    point.x = xspace + left + effects3d;
    point.y = (yspace + i * yl / 5) + bottom + effects3d;
    GpiMove( hps, &point );
    point.x = (xspace + xl) + left - effects3d;
    GpiLine( hps, &point );
  }

  if(effects3d)
    GpiSetColor( hps, CLR_BLACK );

  for( x = 0; x < 5; x++ )             /* define draw location of numbers     */
  {                                    /* (0,0) is bottom left corner while   */
    for( y = 0; y < 5; y++ )           /* (4,4) is top right corner, watch it */
    {
      xpos[x][y] = 6 * xspace + x * xl / 5;  /* 6 x xspace and 4 x space to   */
      ypos[x][y] = 4 * yspace + y * yl / 5;  /* center number inside box      */
    }
  }

  if(type)
  {
    for( i = 0; i < 6; i++ )           /* location of lines, used so we know  */
      pcd->xline[i] = xspace + i * xl / 5;/* which number when button click on*/
    for( i = 0; i < 6; i++ )           /* card                                */
      pcd->yline[i] = yspace + i * yl / 5;
  }

  GpiCreateLogFont( hps, (PSTR8) NULL, pbd->lcid, &(pbd->fAttr) );
  GpiSetCharSet( hps, pbd->lcid );

  sizfx.cx = MAKEFIXED( xl / 5, 0 );
  sizfx.cy = MAKEFIXED( yl / 5, 0 );
  GpiSetCharBox( hps, &sizfx );

  cbnd.lColor = CLR_RED;
  GpiSetAttrs( hps, PRIM_CHAR, CBB_COLOR, NULLHANDLE, &cbnd );

  point.x = (4 * xspace) + left;
  point.y = (4 * yspace + yl) + bottom;
  GpiMove( hps, &point );
  GpiCharString( hps, 12, "B  I  N G O\0" );

  sizfx.cx = MAKEFIXED( xl / 20, 0 );
  GpiSetCharBox( hps, &sizfx );
  point.x = xpos[2][2] + left;
  point.y = ypos[2][2] + bottom;
  GpiMove( hps, &point );
  GpiCharString( hps, 4, "FREE" );

  cbnd.lColor = CLR_BLACK;
  GpiSetAttrs( hps, PRIM_CHAR, CBB_COLOR, NULLHANDLE, &cbnd );
  sizfx.cx = MAKEFIXED( xl / 10, 0 );
  GpiSetCharBox( hps, &sizfx );

  for( x = 0; x < 5; x++ )             /* draw the bingo card numbers         */
  {                                    /* bingo card number found in bingono  */
    for( y = 0; y < 5; y++ )           /* variable array                      */
    {
      point.x = xpos[x][y] + left;     /* xpos and ypos is the draw location  */
      point.y = ypos[x][y] + bottom;   /* of each number                      */
      GpiMove( hps, &point );
      if( x == 2 & y == 2 ) ;          /* ignore FREE space (center)          */
      else
      {
        _ultoa( pcd->bingono[x][y], bno, 10 );   /* convert LONG into string       */
        if( pcd->bingono[x][y] < 10 )            /* if single digit, add pad       */
        {
          bno[1] = bno[0];
          bno[0] = ' ';
          bno[2] = '\0';
        }
        GpiCharString( hps, 3, bno );
      }
    }
  }

  if(type)
  {
    for( x = 0; x < 5; x++ )          /* draw red chips on all marked numbers */
                                      /* marked number have bingoit = 1       */
      for( y = 0; y < 5; y++ )        /* size (radius) of chip is 1\3 of box  */
        if( pcd->bingoit[x][y] > 0 )
          fnDrawMarker( hps, pcd, x, y, pcd->bingoit[x][y] );
  }

}

/******************************************************************************/
/*                                                                            */
/* fnDrawMarker: Draw a circular marker over the bingo card square as given   */
/*               by x and y.                                                  */
/*                                                                            */
/******************************************************************************/

VOID fnDrawMarker( HPS hps, PCARDDATA pcd, LONG x, LONG y, ULONG chipType )
{

  FIXED     radius;
  POINTL    point;
  ARCPARAMS arcp = { 1, 1, 0, 0 };

  point.x = pcd->xline[x+1] - (pcd->xline[x+1]-pcd->xline[x]) / 2;
  point.y = pcd->yline[y+1] - (pcd->yline[y+1]-pcd->yline[y]) / 2;
  if( (pcd->xline[x+1]-pcd->xline[x]) / 2 < (pcd->yline[y+1]-pcd->yline[y]) / 2 )
    radius = MAKEFIXED((pcd->xline[x+1]-pcd->xline[x])/3, 0 );
  else
    radius = MAKEFIXED((pcd->yline[y+1]-pcd->yline[y])/3, 0 );

  GpiSetColor ( hps, chipcolor );
  if(chipType == WIN_CHIP)
    GpiSetPattern( hps, PATSYM_DENSE5);
  if(chipType != O_CHIP)
    GpiBeginArea( hps, BA_BOUNDARY | BA_WINDING );
  GpiMove( hps, &point );
  GpiSetArcParams( hps, &arcp );
  GpiFullArc( hps, DRO_OUTLINE, radius );
  if(chipType != O_CHIP)
    GpiEndArea( hps );
  if(chipType == WIN_CHIP)
    GpiSetPattern( hps, PATSYM_DEFAULT);

}

/******************************************************************************/
/*                                                                            */
/* fnGetCardNumbers: Generate the numbers for the bingo card. Calls           */
/*                   GetRandom( ). Numbers for bingo card placed in           */
/*                   pcd.bingono[x][y].                                       */
/*                                                                            */
/******************************************************************************/

VOID fnGetCardNumbers( PCARDDATA pcd, SHORT newcard )

{

  LONG x, y;

  for( x = 0; x < 5; x++ )
    for( y = 0; y < 5; y++ )
      pcd->bingoit[x][y] = NO_CHIP;    /* indicator for marked square in card */
                                       /* 0 means unmarked, 1 means marked    */
  pcd->status     = CARD_NONE;         /* indicator if card has bingo (1)     */
  pcd->waitnumber = NO_BINGO;

  if( newcard == 0 )
  {
    BOOL card[75];
    LONG number, min, max;

    for( x = 0; x < 75; x++ )
      card[x] = 0;

    min = 1;
    max = 15;
    number = 0;
    for( x = 0; x < 5; x++ )
    {
      for( y = 0; y < 5; y++ )
      {
        number = GetRandom( min, max );
        while( card[number-1] == 1 )
          number = GetRandom( min, max );
        card[number-1] = 1;
        pcd->bingono[x][y] = number;   /* assign random number of each square */
      }                                /* in the 5x5 bingo card               */
      min = min + 15;
      max = max + 15;
    }

    pcd->bingono[2][2] = 0;            /* free space, change number to 0      */

  }

/* Debugging purposes in case random routines do not work                     */
//pcd->bingono[0][0] = 1;  pcd->bingono[0][1] = 2;  pcd->bingono[0][2] = 3;
//pcd->bingono[0][3] = 4;  pcd->bingono[0][4] = 5;
//pcd->bingono[1][0] = 21; pcd->bingono[1][1] = 22; pcd->bingono[1][2] = 23;
//pcd->bingono[1][3] = 24; pcd->bingono[1][4] = 25;
//pcd->bingono[2][0] = 41; pcd->bingono[2][1] = 42; pcd->bingono[2][2] = 0;
//pcd->bingono[2][3] = 44; pcd->bingono[2][4] = 45;
//pcd->bingono[3][0] = 51; pcd->bingono[3][1] = 52; pcd->bingono[3][2] = 53;
//pcd->bingono[3][3] = 54; pcd->bingono[3][4] = 55;
//pcd->bingono[4][0] = 61; pcd->bingono[4][1] = 62; pcd->bingono[4][2] = 63;
//pcd->bingono[4][3] = 64; pcd->bingono[4][4] = 65;


}

/******************************************************************************/
/*                                                                            */
/* fnProcessBingoSquare: Bingo square played is valid. Process it by drawing  */
/*                       a marker and then check for bingo.                   */
/*                                                                            */
/* Calls: fnDrawMarker                                                        */
/*        fnCheckBingo                                                        */
/*                                                                            */
/******************************************************************************/

VOID fnProcessBingoSquare( HWND hwnd, PCARDDATA pcd, LONG x, LONG y )
{

  HPS    hps;
  RECTL  rcl;
  LONG   checkFlag;

  pcd->bingoit[x][y] = NORMAL_CHIP;    /* set bingo card square to marked     */

  hps = WinBeginPaint( hwnd, 0L, 0L );
  fnDrawMarker( hps, pcd, x, y, pcd->bingoit[x][y] );
  WinEndPaint( hps );

  rcl.xLeft   = pcd->xline[x];
  rcl.xRight  = pcd->xline[x+1];
  rcl.yBottom = pcd->yline[y];
  rcl.yTop    = pcd->yline[y+1];
  WinInvalidateRect( hwnd, &rcl, FALSE );

  checkFlag = fnCheckBingo( pcd );
  switch( checkFlag )
  {
    case( FOUND_BINGO ):
    {                                  /* This card has a bingo! Hurray!      */
      pcd->status = CARD_BINGO;        /* set card bingo indicator to YES (1) */

      if(pcd->whichPlayer == PLAYER)
      {
        psd->bingos = psd->bingos + 1;
        psd->moneywon  = psd->moneywon  + prizemoney;
        psd->moneyleft = psd->moneyleft + prizemoney;
      }
      else
      {
        psd->otherbingos    = psd->otherbingos + 1;
        psd->othermoneywon  = psd->othermoneywon  + prizemoney;
      }

      bingostatus = GAMEOVER;          /* set flag to indicate bingo found    */
                                       /* already found, game completed!      */
      fnChatProcess(CHAT_BINGO, bingonumber, 0L, 0L);
//    WinPostMsg( hwndBoard, WM_TIMER, MPFROMSHORT(ID_TIMER), 0L );
    }                                  /* have bingo for this card            */
    break;

    case( NO_BINGO ):                  /* Nothing found, update flags         */
      pcd->status     = CARD_NONE;
      pcd->waitnumber = NO_BINGO;
      break;

    default:                           /* Otherwise, card must be waiting     */
      pcd->status     = CARD_NONE;     /* Send status message to Chatter      */
      if(pcd->waitnumber == NO_BINGO)  /* only if not already previously      */
      {                                /* waiting for a number                */
        pcd->waitnumber = checkFlag;
        fnChatProcess(CHAT_WAIT, bingonumber, pcd->whichPlayer, checkFlag);
      }
      break;
  }
}

/******************************************************************************/
/*                                                                            */
/* fnCheckBingo: Check for bingo. Returns 1 if bingo found, 0 for no bingo.   */
/*                                                                            */
/******************************************************************************/

LONG fnCheckBingo( PCARDDATA pcd )
{

  LONG foundbingo;                     /* FOUND_BINGO, NO_BINGO or bingonumber*/
  LONG x, y, i;

  if( gametype == 0 ||                 /* check classic bingo game and        */
      gametype == 15 )                 /* hardaway game, special checking     */
  {
    foundbingo = NO_BINGO;
                                       /* Check diagonals                     */
    if(gametype == 0)
    {                                  /* Remember, 0 not marked and 1 marked */
                                       /* Although missed is 2, but we are not*/
                                       /* called when the game is over!       */
      i = pcd->bingoit[0][0] + pcd->bingoit[1][1] +
          pcd->bingoit[3][3] + pcd->bingoit[4][4];
      if( i > 2 )                      /* Only want bingo and waiting cards   */
      {
        if( i == 4 )                   /* Bingo!                              */
        {
          pcd->bingoit[0][0] = WIN_CHIP;/* Now set the winning bingo number   */
          pcd->bingoit[1][1] = WIN_CHIP;/* squares to 3 (WIN_CHIP) so that    */
          pcd->bingoit[3][3] = WIN_CHIP;/* they are drawn with a special chip */
          pcd->bingoit[4][4] = WIN_CHIP;/* to indicate the winning combination*/
          return( FOUND_BINGO );
        }
        else                           /* Card is waiting for one more number */
        {                              /* only.                               */
          if( pcd->bingoit[0][0] == 0 )
            foundbingo = pcd->bingono[0][0];
          if( pcd->bingoit[1][1] == 0 )
            foundbingo = pcd->bingono[1][1];
          if( pcd->bingoit[3][3] == 0 )
            foundbingo = pcd->bingono[3][3];
          if( pcd->bingoit[4][4] == 0 )
            foundbingo = pcd->bingono[4][4];
        }
      }

      i = pcd->bingoit[0][4] + pcd->bingoit[1][3] +
          pcd->bingoit[3][1] + pcd->bingoit[4][0];
      if( i > 2 )
      {
        if( i == 4 )
        {
          pcd->bingoit[0][4] = WIN_CHIP;
          pcd->bingoit[3][1] = WIN_CHIP;
          pcd->bingoit[1][3] = WIN_CHIP;
          pcd->bingoit[4][0] = WIN_CHIP;
          return( FOUND_BINGO );
        }
        else
        {
          if( pcd->bingoit[0][4] == 0 )
            foundbingo = pcd->bingono[0][4];
          if( pcd->bingoit[3][1] == 0 )
            foundbingo = pcd->bingono[3][1];
          if( pcd->bingoit[1][3] == 0 )
            foundbingo = pcd->bingono[1][3];
          if( pcd->bingoit[4][0] == 0 )
            foundbingo = pcd->bingono[4][0];
        }
      }
    }

    for( x = 0; x < 5; x++ )           /* Check all vertical lines            */
    {
      if( x == 2 && gametype == 0 )    /* Vertical with free square           */
      {
        i = pcd->bingoit[x][0] + pcd->bingoit[x][1] +
            pcd->bingoit[x][3] + pcd->bingoit[x][4];
        if( i > 2 )
        {
          if( i == 4 )
          {
            pcd->bingoit[x][0] = WIN_CHIP;
            pcd->bingoit[x][1] = WIN_CHIP;
            pcd->bingoit[x][3] = WIN_CHIP;
            pcd->bingoit[x][4] = WIN_CHIP;
            return( FOUND_BINGO );
          }
          else
          {
            if( pcd->bingoit[x][0] == 0 )
              foundbingo = pcd->bingono[x][0];
            if( pcd->bingoit[x][1] == 0 )
              foundbingo = pcd->bingono[x][1];
            if( pcd->bingoit[x][3] == 0 )
              foundbingo = pcd->bingono[x][3];
            if( pcd->bingoit[x][4] == 0 )
              foundbingo = pcd->bingono[x][4];
          }
        }
      }
      else                             /* normal vertical                     */
      {
        i = pcd->bingoit[x][0] + pcd->bingoit[x][1] +
            pcd->bingoit[x][2] + pcd->bingoit[x][3] +
            pcd->bingoit[x][4];
        if( i > 3 )
        {
          if( i == 5 )
          {
            pcd->bingoit[x][0] = WIN_CHIP;
            pcd->bingoit[x][1] = WIN_CHIP;
            pcd->bingoit[x][2] = WIN_CHIP;
            pcd->bingoit[x][3] = WIN_CHIP;
            pcd->bingoit[x][4] = WIN_CHIP;
            return( FOUND_BINGO );
          }
          else
          {
            if( pcd->bingoit[x][0] == 0 )
              foundbingo = pcd->bingono[x][0];
            if( pcd->bingoit[x][1] == 0 )
              foundbingo = pcd->bingono[x][1];
            if( pcd->bingoit[x][2] == 0 )
              foundbingo = pcd->bingono[x][2];
            if( pcd->bingoit[x][3] == 0 )
              foundbingo = pcd->bingono[x][3];
            if( pcd->bingoit[x][4] == 0 )
              foundbingo = pcd->bingono[x][4];
          }
        }
      }
    }

    for( y = 0; y < 5; y++ )           /* check all horizontal lines          */
    {
      if( y == 2 && gametype == 0 )    /* horizontal line with free space     */
      {
        i = pcd->bingoit[0][y] + pcd->bingoit[1][y] +
            pcd->bingoit[3][y] + pcd->bingoit[4][y];
        if( i > 2 )
        {
          if( i == 4 )
          {
            pcd->bingoit[0][y] = WIN_CHIP;
            pcd->bingoit[1][y] = WIN_CHIP;
            pcd->bingoit[3][y] = WIN_CHIP;
            pcd->bingoit[4][y] = WIN_CHIP;
            return( FOUND_BINGO );
          }
          else
          {
            if( pcd->bingoit[0][y] == 0 )
              foundbingo = pcd->bingono[0][y];
            if( pcd->bingoit[1][y] == 0 )
              foundbingo = pcd->bingono[1][y];
            if( pcd->bingoit[3][y] == 0 )
              foundbingo = pcd->bingono[3][y];
            if( pcd->bingoit[4][y] == 0 )
              foundbingo = pcd->bingono[4][y];
          }
        }
      }
      else                             /* normal horizontal line              */
      {
        i = pcd->bingoit[0][y] + pcd->bingoit[1][y] +
            pcd->bingoit[2][y] + pcd->bingoit[3][y] +
            pcd->bingoit[4][y];
        if( i > 3 )
        {
          if( i == 5 )
          {
            pcd->bingoit[0][y] = WIN_CHIP;
            pcd->bingoit[1][y] = WIN_CHIP;
            pcd->bingoit[2][y] = WIN_CHIP;
            pcd->bingoit[3][y] = WIN_CHIP;
            pcd->bingoit[4][y] = WIN_CHIP;
            return( FOUND_BINGO );
          }
          else
          {
            if( pcd->bingoit[0][y] == 0 )
              foundbingo = pcd->bingono[0][y];
            if( pcd->bingoit[1][y] == 0 )
              foundbingo = pcd->bingono[1][y];
            if( pcd->bingoit[2][y] == 0 )
              foundbingo = pcd->bingono[2][y];
            if( pcd->bingoit[3][y] == 0 )
              foundbingo = pcd->bingono[3][y];
            if( pcd->bingoit[4][y] == 0 )
              foundbingo = pcd->bingono[4][y];
          }
        }
      }
    }
  }
  else                                 /* other games - generic check         */
  {                                    /* assume we have a bingo, set true (1)*/
    foundbingo = FOUND_BINGO;
    for( x = 0; x < 5; x++ )
    {
      for( y = 0; y < 5; y++ )
      {                                /* Check only those needed             */
        if( pbd->bingogames[gametype][x][y] == 1 )
        {                              /* Check if bingo card marked          */
          if( pcd->bingoit[x][y] != NORMAL_CHIP )
          {                            /* First number not marked, save as it */
            if(foundbingo == FOUND_BINGO) /* may be the only one -> waiting   */
              foundbingo = pcd->bingono[x][y];
            else
            {                          /* More than one bingo square not      */
              x = 5;                   /* marked, no need to check any        */
              y = 5;                   /* further, get out of loop and set    */
              foundbingo = NO_BINGO;   /* return code to NO_BINGO             */
            }
          }
        }
      }
    }
    if(foundbingo == FOUND_BINGO)      /* Now, set the winning bingo number   */
    {                                  /* square to 3 (WIN_CHIP) so that the  */
      for( x = 0; x < 5; x++ )         /* markers are drawn using a special   */
      {                                /* chip to indicate the winning card   */
        for( y = 0; y < 5; y++ )
        {
          if( pbd->bingogames[gametype][x][y] == 1 )
            pcd->bingoit[x][y] = WIN_CHIP;
        }
      }
    }
  }

  return( foundbingo );                /* return bingo flag here              */

}

/******************************************************************************/
/*                                                                            */
/* fnDisplayBingo: Display message box containing the bingo card number       */
/*                 (title of bingo card frame window).                        */
/*                                                                            */
/******************************************************************************/

VOID fnDisplayBingo( HWND hwnd, BOOL bFound )
{                                      /* there is a bingo so display as      */
                                       /* message box with the window text    */
  RECTL     rcl;                       /* as the message text                 */
  PCARDLIST item;
  LONG      bingoFound = FALSE;

  bingostatus = BINGOSHOWN;            /* set flag to indicate that bingo is  */
                                       /* already found, game completed!      */
  item = start;                        /* for all bingo cards, check for bingo*/
  while( item )                        /* squares not marked, then invalidate */
  {                                    /* window to refresh                   */
    WinSendMsg( item->hwndCard, UM_MISSMARK, 0L, 0L );
    WinQueryWindowRect( item->hwndClient, &rcl );
    WinInvalidateRect( item->hwndClient, &rcl, FALSE );
    item = (PCARDLIST) item->nextcard;
  }

  WinEnableMenuItem( WinWindowFromID( WinQueryWindow( hwndBoard,
                     QW_PARENT ), FID_MENU ), IDM_START,
                     FALSE );

  WinSendMsg( hwndBoard, UM_GAMEOVER, 0L, 0L );

  boolTime = FALSE;
  if( bFound )                         /* Normal processing, bingo found      */
    WinDlgBox( HWND_DESKTOP, hwnd, wpBingoDlgProc, 0L, IDD_BINGO,
               (PVOID)&bingoFound );
  else                                 /* No bingo found, display message box */
  {                                    /* instead of normal Bingo! dialog     */
    if( sound == ON )
      fnSound( SOUND_NOBINGO, 0, 0 );

    WinLoadString( hab, 0L, IDS_TITLEBAR,  TITLESIZE, szTitle);
    WinLoadString( hab, 0L, IDS_NOBINGO, TEXTSIZE, szText);
    WinMessageBox( HWND_DESKTOP, hwnd, szText, szTitle,
                   IDD_NOBINGO,
                   MB_MOVEABLE | MB_OK | MB_HELP );
  }
  boolTime = TRUE;

  return;

}

/******************************************************************************/
/*                                                                            */
/* fnCheckCard: Check if bingo card (pcd) contains the bingo number that was  */
/*              just drawn. Input is bingo letter (0-4) and bingo number.     */
/*              Return is bingo card row number (0-4) or NOT_FOUND (99).      */
/*                                                                            */
/******************************************************************************/

USHORT fnCheckCard( PCARDDATA pcd, USHORT x, LONG number )
{

  USHORT i, y;                         /* x is bingo letter (0=B, 1=I, etc.)  */
                                       /* number is bingo number drawn        */
  y = NOT_FOUND;                       /* Default return value is not found.  */
  for( i = 0; i < 5; i++ )             /* loop thru all the rows in bingo card*/
  {
    if( ( number == pcd->bingono[x][i] ) &&
        ( markall == ON || pbd->bingogames[gametype][x][i] == 1 ) )
    {                                  /* Yup, found bingo number in card!    */
      y = i;                           /* Set return value to row number      */
      break;                           /* break                               */
    }
  }

  return( y );

}

/******************************************************************************/
/*                         M I S C    R O U T I N E S                         */
/******************************************************************************/
/******************************************************************************/
/*                                                                            */
/* fnGetXYValues: Based on the input (num), return the corresponding x and y  */
/*                coordinate values.                                          */
/*                                                                            */
/******************************************************************************/

VOID fnGetXYValues( USHORT num, USHORT *x, USHORT *y )
{

  *y = (num - 1) / 15;
  *x = num - ( *y * 15 + 1 );
  return;

}

/******************************************************************************/
/*                                                                            */
/* fnFreeCardList: Free the memory of the linked list containing the window   */
/*                 handles of the bingo cards. Send a message to the bingo    */
/*                 card client window so it can free the bingo card data      */
/*                 storage also.                                              */
/*                                                                            */
/******************************************************************************/

VOID fnFreeCardList( PCARDLIST startItem, PCARDLIST lastItem )
{

   PCARDLIST item, temp;

   item = startItem;                   /* loop thru all members in linked     */
   while( item )                       /* list, post WM_DESTROY message for   */
   {                                   /* each and then free storage          */
     WinSendMsg( item->hwndCard, WM_DESTROY, 0L, 0L );
     free( item->pcd );
     temp = (PCARDLIST)item->nextcard;
     free( item );
     item = temp;
   }
   startItem = NULL;
   lastItem  = NULL;

}

/******************************************************************************/
/*                                                                            */
/* fnSetDefaultOptions: Set default game option values.                       */
/*                                                                            */
/******************************************************************************/

VOID fnSetDefaultOptions( VOID )
{

  numberofcards   = 1;
  gametype        = 0;
  numberofplayers = 1;

}

/******************************************************************************/
/*                                                                            */
/* fnSetDefaultSettings: Set default setting values.                          */
/*                                                                            */
/******************************************************************************/

VOID fnSetDefaultSettings( VOID )
{

  sound      = ON;
  markall    = OFF;
  chatter    = ON;
  chipcolor  = CLR_RED;
  drawtime   = 5000;
  newcards   = PROMPT;
  showstart  = YES;
  audiotype  = PC_SPEAKER;
  gameprompt = YES;
  pointer    = 0;
  autoplay   = ON;
  boardtype  = HORIZONTAL;
  effects3d  = OFF;
  strcpy( pbd->selFont, szDefaultFont );
}


/******************************************************************************/
/*                                                                            */
/* fnResetHiScores: Reset high scores to 0.                                   */
/*                                                                            */
/******************************************************************************/

VOID fnResetHiScores( VOID )
{

  LONG i;

  for(i=0; i < MAXHISCORES; i++)
  {
    strcpy( phsd->player[i], "" );     /* Reset player name to NULL           */
    phsd->score[i] = 0;                /* Reset high score to 0               */
  }

}

/******************************************************************************/
/*                                                                            */
/* fnHighScores: Determine if current player's money won is a high score.     */
/*               Compare with existing high scores, if greater and add to     */
/*               high score and move those less one level down.               */
/*                                                                            */
/******************************************************************************/

BOOL fnHighScores( VOID )
{

  LONG  i;
  LONG  found = NOT_FOUND;

  for(i=0; i<MAXHISCORES; i++)
  {
    if(psd->moneyleft > phsd->score[i])
    {                                  /* Yup, current score is greater than  */
      found = i;                       /* this high score, save number, leave */
      break;
    }
  }

  if(found != NOT_FOUND)               /* High score found? If yes, then add  */
  {                                    /* to high scores and move other down  */
    CHAR  tempName[TITLESIZE+1];
    ULONG tempScore;
    for(i=MAXHISCORES-1; i>found; i--)
    {
      strcpy(phsd->player[i], phsd->player[i-1]);
      phsd->score[i] = phsd->score[i-1];
    }
    strcpy(phsd->player[found], szName);
    phsd->score[found] = psd->moneyleft;

    return( TRUE );
  }

  return( FALSE );
}


/******************************************************************************/
/*                                                                            */
/* fnGetBingoLetter: Return bingo letter (as CHAR).                           */
/*                                                                            */
/******************************************************************************/

CHAR fnGetBingoLetter( ULONG number )
{

  CHAR letter;
  switch( number )
  {
    case( 0 ):
      letter = 'B';
      break;
    case( 1 ):
      letter = 'I';
      break;
    case( 2 ):
      letter = 'N';
      break;
    case( 3 ):
      letter = 'G';
      break;
    case( 4 ):
      letter = 'O';
      break;
    default:
      letter = ' ';
      break;
  }

  return(letter);
}

/******************************************************************************/
/*                                                                            */
/* fnGetBingoLetter: Return bingo letter (as CHAR).                           */
/*                                                                            */
/******************************************************************************/

CHAR fnGetBingoLetterFromBingoNumber( ULONG number )
{

  CHAR letter;

  if( number < 16 )
    letter = 'B';
  else
  {
    if( number < 31 )
      letter = 'I';
    else
    {
      if( number < 46 )
        letter = 'N';
      else
      {
        if( number < 61 )
          letter = 'G';
        else
        {
          if( number < 76 )
            letter = 'O';
          else
            letter = ' ';
        }
      }
    }
  }

  return(letter);
}

/******************************************************************************/
/*                                                                            */
/* fnConvertTime: Convert time (in seconds) to mm:ss format.                  */
/*                                                                            */
/******************************************************************************/

VOID fnConvertTime( ULONG timer, CHAR *szOuttime )

{
  ULONG seconds;
  ULONG minutes;

  seconds = timer % 60;
  minutes = timer / 60;

  sprintf( szOuttime, "%02d:%02d\0", minutes, seconds );

  return;
}


/******************************************************************************/
/*                                                                            */
/* fnToUpperString: Convert string to upper case.                             */
/*                                                                            */
/******************************************************************************/

VOID fnToUpperString( char *string )

{
  ULONG i;

  for(i=0;i<strlen(string);i++)
  {
    if(string[i] > 96 && string[i] < 123 )
      string[i] = string[i] - 32;
  }
}


/******************************************************************************/
/*                        C H A T T E R   B O X                               */
/******************************************************************************/
/******************************************************************************/
/*                                                                            */
/* fnChatDrawNumber: Chatter when a bingo number is drawn. Calls              */
/*                   fnChatProcess().                                         */
/*                                                                            */
/******************************************************************************/

VOID fnChatDrawNumber( LONG bingoNumber, USHORT bingoLetter, LONG numbersleft )
{

  CHAR szNumber[5];
  LONG x, y, z;

  /* Chatter for numbers left to be drawn */
  if(numbersleft < 6   || numbersleft == 10 || numbersleft == 15 ||
     numbersleft == 20 || numbersleft == 30 || numbersleft == 40 )
//   numbersleft == 50 || numbersleft == 60 || numbersleft == 70 )
   fnChatProcess( CHAT_NUMBER, bingoNumber, numbersleft, 0L );

  /* Chatter for a letter with all numbers drawn */
  for(y=0; y<5; y++)
  {
    z = 0;
    for(x=0; x<15; x++)
      z = z + pbd->board[x][y];
    if(z == 0 && pbd->letter[y] != ' ')
    {
      fnChatProcess( CHAT_LETTER, bingoNumber, y, 0L );
      pbd->letter[y] = ' ';
    }
  }

  /* Chatter for current number drawn */
  sprintf(szNumber, "%c %d", fnGetBingoLetter(bingoLetter), bingoNumber);
  WinSendMsg(hwndChat, UM_NUMBER, MPFROMP(szNumber), 0L);

}

/******************************************************************************/
/*                                                                            */
/* fnChatProcess: Depending on the chat type (first parameter), create the    */
/*                chatter text and send string ptr to Chatter box dialog.     */
/*                                                                            */
/*                lp1 and lp2 are optional LONG data type parameters.         */
/*                                                                            */
/******************************************************************************/

VOID fnChatProcess( LONG chatType, LONG bingoNumber, LONG lp1, LONG lp2 )

{
  CHAR szChat[CHATSIZE+1];
  CHAR szWho[TEXTSIZE+1];

  switch( chatType )
  {
    case( CHAT_WAIT ):                 /* Bingo card is waitng for last no    */
      {
        switch(lp1)
        {
          case( PLAYER ):
            WinLoadString(hab, 0L, IDS_CHAT_WAITPLAYER, TEXTSIZE, szWho);
            break;
          case( OTHER ):
            WinLoadString(hab, 0L, IDS_CHAT_WAITOTHER, TEXTSIZE, szWho);
            break;
          default:
            break;
        }
        sprintf(szChat, "%s %c %d.", szWho,
                fnGetBingoLetterFromBingoNumber(lp2), lp2);
        fnSound(SOUND_WAIT, 0L, 0L);
      }
      break;
    case( CHAT_WAITCLOSE ):            /* Almost got a bingo!                 */
      WinLoadString(hab, 0L, IDS_CHAT_WAITCLOSE, CHATSIZE, szChat);
      fnSound(SOUND_CLOSE, 0L, 0L);
      break;
    case( CHAT_NUMBER ):               /* Bingo numbers left to be drawn      */
      WinLoadString(hab, 0L, IDS_CHAT_NUMBERLEFT, TEXTSIZE, szWho);
      sprintf(szChat, "%d %s", lp1, szWho);
      fnSound(SOUND_NUMBER, lp1, 0L);
      break;
    case( CHAT_BINGO ):                /* Someone yelled Bingo!               */
      WinLoadString(hab, 0L, IDS_BINGO, CHATSIZE, szChat);
      break;
    case( CHAT_LETTER ):               /* All numbers in letter called        */
      WinLoadString(hab, 0L, IDS_CHAT_LETTER, TEXTSIZE, szWho);
      sprintf(szChat, "%s %c.", szWho, fnGetBingoLetter(lp1));
      fnSound(SOUND_LETTER, lp1, 0L);
      break;
    default:
      break;
  }                                    /* end switch(chatType)                */
                                       /* Send message to Chatter dialog box  */
  WinSendMsg(hwndChat, UM_CHAT, MPFROMP(szChat), 0L);

  if(debug)
    fprintf(wptrDebug, "Chatter: %s\n", szChat);

}


/******************************************************************************/
/*                         F O N T    S U P P O R T                           */
/******************************************************************************/
/******************************************************************************/
/*                                                                            */
/* fnFindAllFonts: Find all valid vector or outline fonts in the system and   */
/*                 place results in pbd->pFontnames. This will be used in     */
/*                 the Select Font dialog.                                    */
/*                                                                            */
/******************************************************************************/

VOID fnFindAllFonts( HWND hwnd )
{

  HPS          hps;
  LONG         i;
  PFONTMETRICS afm = NULL;
  LONG         lNumFonts;
  LONG         lReqFonts;
  LONG         lRemFonts = GPI_ERROR;
  PFONTS       pFonts;

  hps = WinGetPS(hwnd);                /* get a presentation space      */

  pbd->totalFonts = 0;                 /* total number of outline fonts */
  pbd->lcid = 1;                       /* set lcid to default to 1      */


  lReqFonts = 0L;
  lNumFonts = GpiQueryFonts(hps, QF_PUBLIC, NULL, &lReqFonts, 0L, NULL);
  if((lNumFonts != GPI_ALTERROR) && (lNumFonts != 0L))
  {

    if(debug)
      fprintf(wptrDebug, "Total system fonts found is %d.\n", lNumFonts);

    afm = (PFONTMETRICS)malloc(lNumFonts*sizeof(FONTMETRICS));
    if(afm != NULL)
    {                                  /* Allocate memory for list of fonts   */
      pbd->pFontnames = (PFONTS)malloc(lNumFonts*sizeof(FONTS));
      pFonts = pbd->pFontnames;        /* Save the original memory location   */

      lRemFonts = GpiQueryFonts(hps, QF_PUBLIC, NULL, &lNumFonts,
                                (LONG)sizeof(FONTMETRICS), afm);

      for(i = 0; i < lNumFonts; i++)   /* Loop through all valid fonts found  */
      {                                /* Only process outline fonts          */
        if(afm[i].fsDefn & FM_DEFN_OUTLINE)
        {
          strcpy(pbd->pFontnames->name, afm[i].szFacename);

          if(debug)
            fprintf(wptrDebug, "Font: %s.\n", afm[i].szFacename);

          pbd->totalFonts = pbd->totalFonts + 1;
          pbd->pFontnames++;
        }
      }
      pbd->pFontnames = (PFONTS)realloc((char *)pFonts,
                                pbd->totalFonts * sizeof(FONTS));
      if(debug)
        fprintf(wptrDebug, "Total outline fonts found is %d.\n",
                           pbd->totalFonts);

      free(afm);
    }
    else
    {
      /* malloc() failed! */
      WinLoadString( hab, 0L, IDS_FONTERROR,  TITLESIZE, szTitle);
      WinLoadString( hab, 0L, IDS_ERROR_MEMORY, TEXTSIZE, szText);

      WinMessageBox (HWND_DESKTOP, hwnd, szText, szTitle, 1,
                     MB_MOVEABLE | MB_OK | MB_ERROR);
      if(debug)
        fprintf(wptrDebug, "malloc() failed in fnFindAllFonts.\n");
    }
  }
  else
  {
    /* GpiQueryFonts() error */
    CHAR szError[200];

    WinLoadString( hab, 0L, IDS_FONTERROR,  TITLESIZE, szTitle);
    WinLoadString( hab, 0L, IDS_ERROR_FONT_1, TEXTSIZE, szText);
    sprintf(szError, "%s %d.", szText, lNumFonts);

    WinMessageBox (HWND_DESKTOP, hwnd, szText, szError, 1,
                   MB_MOVEABLE | MB_OK | MB_ERROR);
    if(debug)
      fprintf(wptrDebug, "GpiQueryFonts() failed in fnFindAllFonts %d.\n",
                          lNumFonts);
  }

  WinReleasePS(hps);

}

/******************************************************************************/
/*                                                                            */
/* fnFindFont: Find the first valid vector or outline font from the input     */
/*             font name.                                                     */
/*                                                                            */
/******************************************************************************/

VOID fnFindFont( HWND hwnd, CHAR *szFontname )
{

  LONG         i;
  HPS          hps;
  PFONTMETRICS afm = NULL;
  LONG         lNumFonts;
  LONG         lReqFonts;
  LONG         lRemFonts = GPI_ERROR;

  hps = WinGetPS(hwnd);                /* Get a presentation space            */

  lReqFonts = 0L;
  lNumFonts = GpiQueryFonts(hps, QF_PUBLIC, szFontname, &lReqFonts, 0L,
                            NULL);
  if(lNumFonts == 0)                   /* Problem with given font, try the    */
  {                                    /* system default font next            */
    strcpy( pbd->selFont, szDefaultFont );
    lNumFonts = GpiQueryFonts(hps, QF_PUBLIC, pbd->selFont, &lReqFonts, 0L,
                              NULL);
  }

  if((lNumFonts != GPI_ALTERROR) && (lNumFonts != 0L))
  {

    if(debug)
      fprintf(wptrDebug, "Total fonts found for %s is %d.\n",
                         szFontname, lNumFonts);

    afm = (PFONTMETRICS)malloc(lNumFonts*sizeof(FONTMETRICS));
    if(afm != NULL)
    {

      lRemFonts = GpiQueryFonts(hps, QF_PUBLIC, szFontname, &lNumFonts,
                                (LONG)sizeof(FONTMETRICS), afm);
      for(i = 0; i < lNumFonts; i++)
      {
        if(afm[i].fsDefn & FM_DEFN_OUTLINE)
        {

          pbd->fAttr.usRecordLength = sizeof(FATTRS);
          pbd->fAttr.fsSelection = 0;
          pbd->fAttr.lMatch = 0L;
          strcpy(pbd->fAttr.szFacename, afm[i].szFacename);
          pbd->fAttr.idRegistry = afm[i].idRegistry;

          if(debug)
            fprintf(wptrDebug, "Font used: %s %d.\n",
                                afm[i].szFacename, afm[i].idRegistry);

          pbd->fAttr.usCodePage = 0;
          pbd->fAttr.lMaxBaselineExt = 0;
          pbd->fAttr.lAveCharWidth = 0;
          pbd->fAttr.fsType = 0;
          pbd->fAttr.fsFontUse = FATTR_FONTUSE_OUTLINE;

          break;
        }
      }
      free(afm);
    }
    else
    {
      /* malloc() failed! */
      WinLoadString( hab, 0L, IDS_FONTERROR,  TITLESIZE, szTitle);
      WinLoadString( hab, 0L, IDS_ERROR_MEMORY, TEXTSIZE, szText);

      WinMessageBox (HWND_DESKTOP, hwnd, szText, szTitle, 1,
                     MB_MOVEABLE | MB_OK | MB_ERROR);
      if(debug)
        fprintf(wptrDebug, "malloc() failed in fnFindFont for %s.\n",
                           szFontname);
    }
  }
  else
  {
    /* GpiQueryFonts() error */
    CHAR szError[200];

    WinLoadString( hab, 0L, IDS_FONTERROR,  TITLESIZE, szTitle);
    WinLoadString( hab, 0L, IDS_ERROR_FONT_1, TEXTSIZE, szText);
    sprintf(szError, "%s %d.", szText, lNumFonts);

    WinMessageBox (HWND_DESKTOP, hwnd, szError, szTitle, 1,
                   MB_MOVEABLE | MB_OK | MB_ERROR);
    if(debug)
     fprintf(wptrDebug, "GpiQueryFonts() failed in fnFindFont for %s %d.\n",
                        szFontname, lNumFonts);
  }

  WinReleasePS(hps);

}
