/******************************************************************************/
/*                       (c) IBM Corp. 1993                                   */
/*                                                                            */
/* Program: OS/2 PM Bingo - Dialog procedures                                 */
/*                                                                            */
/* Author : E. L. Zapanta                                                     */
/* Date   : 03Oct93                                                           */
/*                                                                            */
/******************************************************************************/

#define INCL_WIN
#define INCL_BASE
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

VOID fnOptionsDlgInit( HWND hwnd );
VOID fnSettingsDlgInit( HWND hwnd );

/******************************************************************************/
/*                                                                            */
/* wpOptionsDlgProc: Window procedure for Options dialog window.              */
/*                                                                            */
/******************************************************************************/

MRESULT EXPENTRY wpOptionsDlgProc (HWND hwnd, ULONG msg, MPARAM mp1,
                                   MPARAM mp2)

{

 MRESULT mr = (MRESULT) FALSE;
 static ULONG oldgametype, oldnumberofcards, oldnumberofplayers;
 static CHAR  oldszName[20];

 switch (msg)
    {
     case WM_INITDLG:                  /* Dialog box just created. Initialize */
        {                              /* combo box, radio buttons and entry  */
          ULONG i;

          oldgametype        = gametype;
          oldnumberofcards   = numberofcards;
          oldnumberofplayers = numberofplayers;
          strcpy( oldszName, szName );

          for( i = 0; i < BINGOGAMES; i++ )
            WinSendDlgItemMsg( hwnd, IDC_GAMETYPE, LM_INSERTITEM,
                               MPFROMLONG( i ), MPFROMP( pbd->gametypes[i] ) );

          WinSendDlgItemMsg( hwnd, IDC_NUMCARDS, SPBM_SETLIMITS,
                             MPFROMSHORT(MAXNUMCARDS),
                             MPFROMSHORT(MINNUMCARDS) );
          WinSendDlgItemMsg( hwnd, IDC_NUMCARDS, SPBM_SETTEXTLIMIT,
                             MPFROMSHORT(2L), 0L );

          WinSendDlgItemMsg( hwnd, IDC_NUMPLAYERS, SPBM_SETLIMITS,
                             MPFROMSHORT(MAXNUMPLAYERS),
                             MPFROMSHORT(0) );
          WinSendDlgItemMsg( hwnd, IDC_NUMPLAYERS, SPBM_SETTEXTLIMIT,
                             MPFROMSHORT(3L), 0L );

          fnOptionsDlgInit( hwnd );
        }
        break;                         /* end case (WM_INITDLG)               */

     case WM_CONTROL:
       switch( SHORT1FROMMP( mp1 ))    /* determine if entry fields changed   */
       {
         case IDC_GAMETYPE:
           if( (USHORT) SHORT2FROMMP(mp1) == CBN_LBSELECT )
           {
             gametype = (ULONG) WinSendDlgItemMsg( hwnd, IDC_GAMETYPE,
                                LM_QUERYSELECTION, 0L, 0L );
             WinSendMsg( hwndBoard, UM_GAMETYPE, 0L, 0L );
           }
           break;

         default:
           mr = WinDefDlgProc (hwnd, msg, mp1, mp2);
           break;
       }
       break;

     case WM_COMMAND:
        switch (SHORT1FROMMP(mp1))
           {
            case DID_OK:               /* User clicked on OK push button      */
               {                       /* Process options settings and place  */
                                       /* Get user's name from entry field    */
                 WinQueryWindowText( WinWindowFromID( hwnd, IDE_USERNAME ),
                                     sizeof( szName ), szName );
                                       /* Get game type selected from combobox*/
                 gametype = (ULONG) WinSendDlgItemMsg( hwnd, IDC_GAMETYPE,
                            LM_QUERYSELECTION, 0L, 0L );
                                       /*Get numbers of cards from entry field*/
                 WinSendDlgItemMsg( hwnd, IDC_NUMCARDS, SPBM_QUERYVALUE, szText,
                                    MPFROM2SHORT( sizeof( szText ),0L ));
                 numberofcards = atoi( szText );
                 if( numberofcards < MINNUMCARDS )
                   numberofcards = MINNUMCARDS;
                 if( numberofcards > MAXNUMCARDS )
                   numberofcards = MAXNUMCARDS;
                                       /*Get numbers of players from spin butt*/
                 WinSendDlgItemMsg( hwnd, IDC_NUMPLAYERS, SPBM_QUERYVALUE,
                                    szText,
                                    MPFROM2SHORT( sizeof( szText ),0L ));
                 numberofplayers = atoi( szText );
                 if( numberofplayers < 0 )
                   numberofplayers = 0;
                 if( numberofplayers > MAXNUMPLAYERS )
                   numberofplayers = MAXNUMPLAYERS;

                 if(debug)
                 {
                   fprintf(wptrDebug, "DID_OK in Options dialog.\n");
                   fprintf(wptrDebug, "szName         : %s\n", szName);
                   fprintf(wptrDebug, "gametype       : %d\n", gametype);
                   fprintf(wptrDebug, "numberofcards  : %d\n", numberofcards);
                   fprintf(wptrDebug, "numberofplayers: %d\n", numberofplayers);
                 }

                 WinDismissDlg (hwnd, TRUE);        /* remove the dialog box  */
               }                       /* end case (DID_OK)                   */
               break;

            case IDP_DEFAULTS:         /* User clicked on Default push button */
               {
                 fnSetDefaultOptions( ); /* Re-initialize variables to system */
                 fnOptionsDlgInit( hwnd );
               }                       /* end case (IDP_DEFAULT)              */
               break;

            case IDP_OPTHELP:
               break;

            case DID_CANCEL:           /* User clicked on Cancel push button  */
               gametype        = oldgametype;
               numberofcards   = oldnumberofcards;
               numberofplayers = oldnumberofplayers;
               strcpy( szName, oldszName );
               WinSendMsg( hwndBoard, UM_GAMETYPE, 0L, 0L );
               WinDismissDlg (hwnd, FALSE);
               break;

            case IDP_SETTINGS:
               WinDlgBox (HWND_DESKTOP, hwnd, wpSettingsDlgProc, 0L,
                         IDD_SETTINGS, NULL );
               break;

            default:                   /* Let PM handle rest of messages      */
               mr = WinDefDlgProc (hwnd, msg, mp1, mp2);
               break;
           }
        break;

     default:                          /* Let PM handle rest of messages      */
        mr = WinDefDlgProc (hwnd, msg, mp1, mp2);
        break;
    }

 return (mr);

}

/******************************************************************************/
/*                                                                            */
/* fnOptionsDlgInit: Initialize Options dialog window.                        */
/*                                                                            */
/******************************************************************************/

VOID fnOptionsDlgInit( HWND hwnd )

{

  ULONG i;

  WinSetWindowText ( WinWindowFromID( hwnd, IDE_USERNAME ), szName );

  for( i = 0; i < BINGOGAMES; i++ )
  {                                    /* Loop thru all game types to find    */
                                       /* which current one selected          */
    if( i == gametype )
      WinSendDlgItemMsg( hwnd, IDC_GAMETYPE, LM_SELECTITEM,
                         MPFROMLONG( i ), MPFROMSHORT( TRUE ) );
  }

                                       /* Number of cards spin button         */
  WinSendDlgItemMsg( hwnd, IDC_NUMCARDS, SPBM_SETCURRENTVALUE,
                     MPFROMSHORT(numberofcards), 0L );
                                       /* Number of players spin button       */
  WinSendDlgItemMsg( hwnd, IDC_NUMPLAYERS, SPBM_SETCURRENTVALUE,
                     MPFROMSHORT(numberofplayers), 0L );

}

/******************************************************************************/
/*                                                                            */
/* wpSettingsDlgProc: Window procedure for Settings dialog window.            */
/*                                                                            */
/******************************************************************************/

MRESULT EXPENTRY wpSettingsDlgProc (HWND hwnd, ULONG msg, MPARAM mp1,
                                   MPARAM mp2)

{

 MRESULT mr = (MRESULT) FALSE;
 static BOOL  oldsound, oldmarkall, oldshowstart, oldautoplay, oldeffects3d;
 static BOOL  oldaudiotype, oldgameprompt, oldchatter, oldboardtype;
 static ULONG oldchipcolor, olddrawtime, oldnewcards, oldpointer;

 switch (msg)
    {
     case WM_INITDLG:                  /* Dialog box just created. Initialize */
        {                              /* combo box, radio buttons and entry  */
          ULONG i;                     /* field                               */

          oldsound      = sound;       /* Save old values in case user decides*/
          oldmarkall    = markall;     /* to quit after making changes        */
          oldnewcards   = newcards;
          oldchipcolor  = chipcolor;
          olddrawtime   = drawtime;
          oldshowstart  = showstart;
          oldaudiotype  = audiotype;
          oldgameprompt = gameprompt;
          oldpointer    = pointer;
          oldchatter    = chatter;
          oldautoplay   = autoplay;
          oldboardtype  = boardtype;
          oldeffects3d  = effects3d;
                                       /* Process chip color value set cntrl  */
          for (i = 0; i < 8; i++)
            WinSendDlgItemMsg(hwnd, IDV_COLOR, VM_SETITEM,
                              MPFROM2SHORT (1, i + 1),
                              MPFROMLONG ((ULONG) i));
          for (i = 8; i < 16; i++)
            WinSendDlgItemMsg(hwnd, IDV_COLOR, VM_SETITEM,
                              MPFROM2SHORT (2, i - 7),
                              MPFROMLONG ((ULONG) i));

                                       /* Process pointer value set control   */
          for (i = 0; i < MAXPOINTERS; i++)
            WinSendDlgItemMsg(hwnd, IDV_POINTER, VM_SETITEM,
                              MPFROM2SHORT(1, i+1),
                              MPFROMLONG((ULONG) pbd->hptr[i]));

                                       /* Process draw time slider bar control*/
          WinSendDlgItemMsg(hwnd, IDC_DRAWTIME, SLM_SETTICKSIZE,
                            MPFROM2SHORT(SMA_SETALLTICKS, 6), 0L);
          WinSendDlgItemMsg(hwnd, IDC_DRAWTIME,
                       SLM_SETSCALETEXT, MPFROMSHORT(0),
                       MPFROMP("0"));
          WinSendDlgItemMsg(hwnd, IDC_DRAWTIME,
                       SLM_SETSCALETEXT, MPFROMSHORT(5),
                       MPFROMP("5"));
          WinSendDlgItemMsg(hwnd, IDC_DRAWTIME,
                       SLM_SETSCALETEXT, MPFROMSHORT(10),
                       MPFROMP("10"));
          WinSendDlgItemMsg(hwnd, IDC_DRAWTIME,
                       SLM_SETSCALETEXT, MPFROMSHORT(15),
                       MPFROMP("15"));
          WinSendDlgItemMsg(hwnd, IDC_DRAWTIME,
                       SLM_SETSCALETEXT, MPFROMSHORT(20),
                       MPFROMP("20"));
          WinSendDlgItemMsg(hwnd, IDC_DRAWTIME,
                       SLM_SETSCALETEXT, MPFROMSHORT(25),
                       MPFROMP("25"));
          WinSendDlgItemMsg(hwnd, IDC_DRAWTIME,
                       SLM_SETSCALETEXT, MPFROMSHORT(30),
                       MPFROMP("30"));

          if( os2mm && os2wave )
            WinEnableControl(hwnd, IDB_AUDIOCARD, TRUE);
          else
            WinEnableControl(hwnd, IDB_AUDIOCARD, FALSE);

          fnSettingsDlgInit( hwnd );   /* Initialize all controls             */
        }
        break;                         /* end case (WM_INITDLG)               */

     case WM_COMMAND:
        switch (SHORT1FROMMP(mp1))
           {
            case DID_OK:               /* User clicked on OK push button      */
               {                       /* Process options settings and place  */
                                       /* into appropriate variables          */
                                       /* get sound from radio button         */
                 MRESULT mrSelect;
                 ULONG   x;
                 SHORT   rows = 1;     /* number of rows for pointer valueset */

                 sound    = (BOOL) WinSendDlgItemMsg( hwnd, IDC_SOUND,
                            BM_QUERYCHECK, 0L, 0L );
                 if( WinSendDlgItemMsg( hwnd, IDB_PCSPEAKER,
                     BM_QUERYCHECK, 0L, 0L ))
                   audiotype = PC_SPEAKER;
                 else
                   audiotype = AUDIO_CARD;
                 if(audiotype == AUDIO_CARD && oldaudiotype == PC_SPEAKER &&
                    audioDevId == 0)
                   fnAudioCardInit(hwnd);

                                       /* Get mark all from check box         */
                 markall  = (BOOL) WinSendDlgItemMsg( hwnd, IDC_MARKALL,
                            BM_QUERYCHECK, 0L, 0L );

                                       /* Get autoplay from check box         */
                 autoplay = (BOOL) WinSendDlgItemMsg( hwnd, IDC_AUTOPLAY,
                            BM_QUERYCHECK, 0L, 0L );

                                       /* Get chatter from check box          */
                 chatter  = (BOOL) WinSendDlgItemMsg( hwnd, IDC_CHATTER,
                            BM_QUERYCHECK, 0L, 0L );

                                       /* Get show initially from check box   */
                 showstart = (BOOL) WinSendDlgItemMsg( hwnd, IDC_SHOWINIT,
                            BM_QUERYCHECK, 0L, 0L );

                                       /* Get game prompt from check box      */
                 gameprompt = (BOOL) WinSendDlgItemMsg( hwnd, IDC_PROMPT,
                              BM_QUERYCHECK, 0L, 0L );

                                       /* Get game prompt from check box      */
                 effects3d  = (BOOL) WinSendDlgItemMsg( hwnd, IDC_3DEFFECTS,
                              BM_QUERYCHECK, 0L, 0L );

                                       /* Get board type radio button value   */
                 if( WinSendDlgItemMsg( hwnd, IDB_HORIZONTAL, BM_QUERYCHECK,
                                        0L, 0L ))
                   boardtype = HORIZONTAL;
                 else
                   boardtype = VERTICAL;

                                       /*Get chip color selected from valueset*/
                 mrSelect  = WinSendDlgItemMsg( hwnd, IDV_COLOR,
                             VM_QUERYSELECTEDITEM, 0L, 0L );
                 chipcolor = (ULONG) WinSendDlgItemMsg(hwnd, IDV_COLOR,
                             VM_QUERYITEM, (MPARAM) mrSelect, 0L);

                                       /*Get pointer selected from valueset   */
                 x         = (ULONG) WinSendDlgItemMsg(hwnd, IDV_POINTER,
                             VM_QUERYSELECTEDITEM, NULL, NULL);
                 pointer   = rows*(SHORT1FROMMR(x)-1) + SHORT2FROMMR(x) - 1;

                                       /* Get New Cards radio button value    */
                 if( WinSendDlgItemMsg( hwnd, IDB_NEWCARDS,
                     BM_QUERYCHECK, 0L, 0L ))
                   newcards = YES;
                 else
                 {
                   if( WinSendDlgItemMsg( hwnd, IDB_OLDCARDS,
                       BM_QUERYCHECK, 0L, 0L ))
                     newcards = NO;
                   else
                     newcards = PROMPT;
                 }
                                       /* Get draw time (ms) from slider bar  */
                 drawtime = (ULONG)WinSendDlgItemMsg (hwnd, IDC_DRAWTIME,
                                   SLM_QUERYSLIDERINFO,
                                   MPFROM2SHORT (SMA_SLIDERARMPOSITION,
                                   SMA_INCREMENTVALUE), NULL) * 1000;

                 if(debug)
                 {
                   fprintf(wptrDebug, "DID_OK in Settings dialog.\n");
                   fprintf(wptrDebug, "sound     : %d\n", sound);
                   fprintf(wptrDebug, "markall   : %d\n", markall);
                   fprintf(wptrDebug, "chatter   : %d\n", chatter);
                   fprintf(wptrDebug, "showstart : %d\n", showstart);
                   fprintf(wptrDebug, "gameprompt: %d\n", gameprompt);
                   fprintf(wptrDebug, "chipcolor : %d\n", chipcolor);
                   fprintf(wptrDebug, "drawtime  : %d\n", drawtime);
                   fprintf(wptrDebug, "newcards  : %d\n", newcards);
                   fprintf(wptrDebug, "audiotype : %d\n", audiotype);
                   fprintf(wptrDebug, "pointer   : %d\n", pointer);
                   fprintf(wptrDebug, "autoplay  : %d\n", autoplay);
                   fprintf(wptrDebug, "boardtype : %d\n", boardtype);
                   fprintf(wptrDebug, "effects3d : %d\n", effects3d);
                 }

                 WinDismissDlg( hwnd, TRUE );       /* remove the dialog box  */

               }                       /* end case (DID_OK)                   */
               break;

            case IDP_DEFAULTS:         /* User clicked on Default push button */
               {
                 HAB hab;
                 hab = WinQueryAnchorBlock( hwnd );
                 fnSetDefaultSettings( );
                 fnSettingsDlgInit( hwnd );
               }                       /* end case (IDP_DEFAULT)              */
               break;

            case IDP_SETHELP:
               break;

            case DID_CANCEL:           /* User clicked on Cancel push button  */
               sound      = oldsound;
               markall    = oldmarkall;
               chatter    = oldchatter;
               newcards   = oldnewcards;
               chipcolor  = oldchipcolor;
               drawtime   = olddrawtime;
               showstart  = oldshowstart;
               audiotype  = oldaudiotype;
               pointer    = oldpointer;
               gameprompt = oldgameprompt;
               autoplay   = oldautoplay;
               boardtype  = oldboardtype;
               effects3d  = oldeffects3d;
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
/* fnSettingsDlgInit: Initialization for Settings dialog window.              */
/*                                                                            */
/******************************************************************************/

VOID fnSettingsDlgInit( HWND hwnd )
{

  USHORT row = 1;
  USHORT col = 1;

  if( sound == ON )
    WinSendDlgItemMsg (hwnd, IDC_SOUND, BM_SETCHECK, MPFROMSHORT (TRUE),
                       0L);
  else
    WinSendDlgItemMsg (hwnd, IDC_SOUND, BM_SETCHECK, MPFROMSHORT (FALSE),
                       0L);

  if( audiotype == PC_SPEAKER )
    WinSendDlgItemMsg (hwnd, IDB_PCSPEAKER, BM_SETCHECK,
                       MPFROMSHORT (TRUE), 0L);
  else
    WinSendDlgItemMsg (hwnd, IDB_AUDIOCARD, BM_SETCHECK,
                       MPFROMSHORT (TRUE), 0L);

  if( markall == ON )
    WinSendDlgItemMsg (hwnd, IDC_MARKALL, BM_SETCHECK,
                       MPFROMSHORT (TRUE), 0L);
  else
    WinSendDlgItemMsg (hwnd, IDC_MARKALL, BM_SETCHECK,
                       MPFROMSHORT (FALSE), 0L);

  if( chatter == ON )
    WinSendDlgItemMsg (hwnd, IDC_CHATTER, BM_SETCHECK,
                       MPFROMSHORT (TRUE), 0L);
  else
    WinSendDlgItemMsg (hwnd, IDC_CHATTER, BM_SETCHECK,
                       MPFROMSHORT (FALSE), 0L);

  if( showstart == YES )
    WinSendDlgItemMsg (hwnd, IDC_SHOWINIT, BM_SETCHECK,
                       MPFROMSHORT (TRUE), 0L);
  else
    WinSendDlgItemMsg (hwnd, IDC_SHOWINIT, BM_SETCHECK,
                       MPFROMSHORT (FALSE), 0L);

  if( gameprompt == YES )
    WinSendDlgItemMsg (hwnd, IDC_PROMPT, BM_SETCHECK,
                       MPFROMSHORT (TRUE), 0L);
  else
    WinSendDlgItemMsg (hwnd, IDC_PROMPT, BM_SETCHECK,
                       MPFROMSHORT (FALSE), 0L);

  if( autoplay == YES )
    WinSendDlgItemMsg (hwnd, IDC_AUTOPLAY, BM_SETCHECK,
                       MPFROMSHORT (TRUE), 0L);
  else
    WinSendDlgItemMsg (hwnd, IDC_AUTOPLAY, BM_SETCHECK,
                       MPFROMSHORT (FALSE), 0L);


  if( boardtype == HORIZONTAL )
      WinSendDlgItemMsg( hwnd, IDB_HORIZONTAL, BM_SETCHECK,
                         MPFROMSHORT (TRUE), 0L);
  else
      WinSendDlgItemMsg( hwnd, IDB_VERTICAL, BM_SETCHECK,
                         MPFROMSHORT (TRUE), 0L);

  if( effects3d == ON )
    WinSendDlgItemMsg (hwnd, IDC_3DEFFECTS, BM_SETCHECK, MPFROMSHORT (TRUE),
                       0L);
  else
    WinSendDlgItemMsg (hwnd, IDC_3DEFFECTS, BM_SETCHECK, MPFROMSHORT (FALSE),
                       0L);

  switch( newcards )
  {
    case( YES ):
      WinSendDlgItemMsg( hwnd, IDB_NEWCARDS, BM_SETCHECK,
                         MPFROMSHORT (TRUE), 0L);
      break;
    case( NO ):
      WinSendDlgItemMsg( hwnd, IDB_OLDCARDS, BM_SETCHECK,
                         MPFROMSHORT (TRUE), 0L);
      break;
    case( PROMPT ):
      WinSendDlgItemMsg( hwnd, IDB_PROMPT, BM_SETCHECK,
                         MPFROMSHORT (TRUE), 0L);
      break;
    default:
      break;
  }
                                       /* Initialize color value set control  */
  if(chipcolor > 7)
  {
    row = 2;
    col = (chipcolor-8) + 1;
  }
  else
    col = chipcolor + 1;
  WinSendDlgItemMsg(hwnd, IDV_COLOR, VM_SELECTITEM, MPFROM2SHORT(row,col), 0L);

                                       /* Initialize pointer value set cntrl  */
  WinSendDlgItemMsg(hwnd, IDV_POINTER, VM_SELECTITEM,
                    MPFROM2SHORT(1,pointer+1), 0L);
                                       /* Initialize slider bar for draw time */
  WinSendDlgItemMsg (hwnd, IDC_DRAWTIME, SLM_SETSLIDERINFO,
                     MPFROM2SHORT (SMA_SLIDERARMPOSITION, SMA_INCREMENTVALUE),
                     MPFROMSHORT(drawtime/1000));

}

/******************************************************************************/
/*                                                                            */
/* wpScoreDlgProc: Window procedure for Bingo Scores dialog window.           */
/*                                                                            */
/******************************************************************************/

MRESULT EXPENTRY wpScoreDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)

{

 MRESULT mr = (MRESULT) FALSE;

 switch (msg)
    {
     case WM_INITDLG:                  /* Dialog box just created. Initialize */
        {                              /* read-only MLE with bingo score      */
          CHAR szBuffer[TEXTSIZE+1];

          WinSetWindowText( WinWindowFromID(hwnd, IDS_PLAYER_NAME), szName );
          WinLoadString (hab, 0L, IDS_OTHERNAME, TEXTSIZE, szBuffer);
          WinSetWindowText( WinWindowFromID(hwnd, IDS_OTHER_NAME), szBuffer );
                                       /* Players scores here ...             */
          sprintf(szBuffer, "%d", psd->gamesplayed);
          WinSetWindowText(WinWindowFromID(hwnd, IDE_PLAYER_GAMES), szBuffer);
          sprintf(szBuffer, "%d", psd->cardsplayed);
          WinSetWindowText(WinWindowFromID(hwnd, IDE_PLAYER_CARDS), szBuffer);
          sprintf(szBuffer, "%d", psd->bingos);
          WinSetWindowText(WinWindowFromID(hwnd, IDE_PLAYER_BINGOS), szBuffer);
          sprintf(szBuffer, "%s%d", szMoney, psd->moneywon);
          WinSetWindowText(WinWindowFromID(hwnd, IDE_PLAYER_MONEYWON), szBuffer);

                                       /* Other players scores here ...       */
          sprintf(szBuffer, "%d", psd->othercardsplayed);
          WinSetWindowText(WinWindowFromID(hwnd, IDE_OTHER_GAMES), szBuffer);
          sprintf(szBuffer, "%d", psd->othercardsplayed);
          WinSetWindowText(WinWindowFromID(hwnd, IDE_OTHER_CARDS), szBuffer);
          sprintf(szBuffer, "%d", psd->otherbingos);
          WinSetWindowText(WinWindowFromID(hwnd, IDE_OTHER_BINGOS), szBuffer);
          sprintf(szBuffer, "%s%d", szMoney, psd->othermoneywon);
          WinSetWindowText(WinWindowFromID(hwnd, IDE_OTHER_MONEYWON), szBuffer);

                                       /* Money left for player .             */
          sprintf(szBuffer, "%s%d", szMoney, psd->moneyleft);
          WinSetWindowText(WinWindowFromID(hwnd, IDE_MONEYLEFT), szBuffer);
        }
        break;                         /* end case (WM_INITDLG)               */

     case WM_COMMAND:
        switch (SHORT1FROMMP(mp1))
           {
            case DID_OK:               /* User clicked on OK push button      */
               WinDismissDlg (hwnd, TRUE);          /* remove the dialog box  */
               break;

            case IDP_RESET:            /* User clicked on Reset push button   */
               {                       /* set values back to 0 here           */
                 if( bingostatus != INPROGRESS )
                   fnInitializeScores();
                 WinDismissDlg( hwnd, TRUE );
               }                       /* end case (IDP_RESET)                */
               break;

            case IDP_SCOREHELP:
               break;

            default:                   /* Let PM handle rest of messages      */
               mr = WinDefDlgProc (hwnd, msg, mp1, mp2);
               break;
           }
        break;

     default:                          /* Let PM handle rest of messages      */
        mr = WinDefDlgProc (hwnd, msg, mp1, mp2);
        break;
    }

 return (mr);

}

/******************************************************************************/
/*                                                                            */
/* wpFontDlgProc: Window procedure for Font dialog window.                    */
/*                                                                            */
/******************************************************************************/

MRESULT EXPENTRY wpFontDlgProc (HWND hwnd, ULONG msg, MPARAM mp1,
                                 MPARAM mp2)

{

 MRESULT mr = (MRESULT) FALSE;

 switch (msg)
    {
     case WM_INITDLG:                  /* Dialog box just created. Initialize */
        {                              /* list box, etc.                      */

          LONG   i;
          PFONTS pFonts;

          pFonts = pbd->pFontnames;
          for( i = 0; i < pbd->totalFonts; i++ )
          {
            WinSendDlgItemMsg( hwnd, IDL_FONTS, LM_INSERTITEM,
                               MPFROMLONG( LIT_END ),
                               MPFROMP( pbd->pFontnames->name ) );
            if(strcmp(pbd->pFontnames->name, pbd->selFont) == 0)
              WinSendDlgItemMsg( hwnd, IDL_FONTS, LM_SELECTITEM,
                           MPFROMLONG( i ), MPFROMSHORT( TRUE ) );
            pbd->pFontnames++;
          }
          pbd->pFontnames = pFonts;
        }
        break;                         /* end case (WM_INITDLG)               */

     case WM_COMMAND:
        switch (SHORT1FROMMP(mp1))
           {
            case DID_OK:               /* User clicked on OK push button      */
               {
                 LONG i,j;
                 PFONTS pFonts;

                 i = (LONG) WinSendDlgItemMsg( hwnd, IDL_FONTS,
                                               LM_QUERYSELECTION, 0L, 0L);
                 pFonts = pbd->pFontnames;
                 for(j=0; j < i; j++)
                   pbd->pFontnames++;
                 strcpy(pbd->selFont, pbd->pFontnames->name);
                 pbd->pFontnames = pFonts;

                 WinDismissDlg( hwnd, TRUE );       /* remove the dialog box  */

               }                       /* end case (DID_OK)                   */
               break;

            case DID_CANCEL:           /* User clicked on Cancel push button  */
               WinDismissDlg( hwnd, FALSE );
               break;

            case IDP_FONTHELP:
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
/* wpBingoDlgProc: Dialog window procedure to display Bingo dialog box.       */
/*                 Use PM timer to display for exactly as long as necessary.  */
/*                                                                            */
/******************************************************************************/

MRESULT EXPENTRY wpBingoDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)

{

 MRESULT mr = (MRESULT) FALSE;
 static  ULONG idBitmap;
 static  RECTL bitmapRect;
 static  ULONG bitmapSize;

 switch (msg)
    {
     case WM_CHAR:
       {
         if(CHARMSG(&msg)->fs & KC_KEYUP)
           return(MRESULT)FALSE;
         if(CHARMSG(&msg)->fs & KC_VIRTUALKEY)
         {
           switch( CHARMSG(&msg)->vkey )
           {
              case VK_ESC:
              case VK_NEWLINE:
              case VK_ENTER:
                WinDismissDlg( hwnd, TRUE );
                return(MRESULT)TRUE;
           }
         }
       }
       break;

     case WM_INITDLG:
        {
          LONG      i, x, y, cx, cy, cz;
          RECTL     rectl;
          HAB       hab;
          PCARDLIST item;
          CHAR      szItem[20];
          LONG      testFlag;
          LONG      bingoFound;              /* flag to indicate bingo, needed*/
                                             /* in case user quits game       */
          bingoFound = * (PLONG) PVOIDFROMMP (mp2);
          testFlag   = bingoFound;

          if(debug)
            fprintf(wptrDebug, "WM_INITDLG for Bingo dialog %d.\n", bingoFound);

          WinQueryWindowRect (HWND_DESKTOP, &rectl);
          cx = rectl.xRight / 5;
          cy = rectl.yTop / 3;
          cz = rectl.xRight / 20;

          /* Find all players bingo cards with a bingo and insert    */
          /* into the Bingo! listbox                                 */
          i = 1;
          item = start;
          while( item )                      /* loop thru all bingo cards here*/
          {                                  /* in the linked list            */
            if(item->pcd->status == CARD_BINGO)
            {
              WinLoadString(hab, 0L, IDS_BINGOTITLE, TITLESIZE, szTitle);
              sprintf(szItem, "%s %d", szTitle, i);

              WinSendDlgItemMsg(hwnd, IDL_BINGO, LM_INSERTITEM,
                                MPFROMLONG(LIT_END),
                                MPFROMP(szItem));
              if(debug)
                fprintf(wptrDebug, "Bingo displayed for %s %d.\n",
                                   szItem, item);

              bingoFound = TRUE;
            }
            item = (PCARDLIST) item->nextcard;
            i++;
          }
          /* Find all other players bingo cards with a bingo and     */
          /* insert in the Bingo! listbox. Also, create window for   */
          /* window bingo cards.                                     */
          x = 4 * cx + cz;
          y = 0;
          i = 1;
          item = startOther;               /* Now process virtual bingo card*/
          while( item )                    /* loop thru all bingo cards here*/
          {                                /* in the linked list            */
            if(item->pcd->status == CARD_BINGO)
            {
              FRAMECDATA fcData;

              fcData.cb            = sizeof (FRAMECDATA);
              fcData.flCreateFlags =  FCF_TITLEBAR      |
                                      FCF_SYSMENU       |
                                      FCF_SIZEBORDER    |
                                      FCF_SHELLPOSITION;
              fcData.hmodResources = 0L;
              fcData.idResources   = idFrameWindow;

              WinLoadString(hab, 0L, IDS_HALLTITLE, TITLESIZE, szTitle);
              sprintf(szItem, "%s %d", szTitle, i);
              WinSendDlgItemMsg(hwnd, IDL_BINGO, LM_INSERTITEM,
                                MPFROMLONG(LIT_END),
                                MPFROMP(szItem));
              if(debug)
                fprintf(wptrDebug, "Bingo displayed for %s %d.\n",
                                   szItem, item);

              if(testFlag != TEST && item->hwndCard == 0)
              {
                item->hwndCard = WinCreateWindow (HWND_DESKTOP, WC_FRAME,
                                 NULL, 0L, 0, 0, 0, 0,
                                 0L, HWND_TOP, IDD_BINGOCARD,
                                 &fcData, NULL);
                item->hwndClient = WinCreateWindow (item->hwndCard, szBingoClass,
                                   NULL, 0L, 0, 0, 0, 0,
                                   0L, HWND_TOP, FID_CLIENT,
                                   (PVOID)item->pcd, NULL);

                if (hwndHelp)
                  WinAssociateHelpInstance(hwndHelp, item->hwndCard);

                WinSetWindowText( item->hwndCard, szItem );
                x = x - cz;
                WinSetWindowPos (item->hwndCard, HWND_TOP, x, y, cx, cy,
                               SWP_SHOW | SWP_SIZE | SWP_MOVE | SWP_ACTIVATE);
                WinShowWindow( item->hwndCard, TRUE );
              }

              bingoFound = TRUE;
            }
            item = (PCARDLIST) item->nextcard;
            i++;
          }

          if(bingoFound == FALSE)
            WinDismissDlg (hwnd, FALSE);
          else
          {
            if(sound == ON)
              fnSound( SOUND_BINGO, PLAYER_WON, 0 );
          }

          /* Set the prize money amount in the Bingo! dialog         */
          WinLoadString(hab, 0L, IDS_PRIZETEXT, TITLESIZE, szTitle);
          sprintf(szItem, "%s %s%d", szTitle, szMoney, prizemoney);
          WinSetWindowText(WinWindowFromID(hwnd, IDS_PRIZE), szItem);

          /* Set timer for how long Bingo! dialog will be shown      */
          /* When it times out, dialog will be automatically removed */
          WinStartTimer(hab, hwnd, TID_BINGOTIMER, drawtime);

          /* Bitmap initialization, set bitmap location and timer    */
          /* interval so we can show a series of bitmaps             */
          idBitmap = IDBMP_BINGO1;
          WinStartTimer(hab, hwnd, ID_BINGOTIMER, drawtime/10);

          WinQueryWindowRect(hwnd, &rectl);

          bitmapSize = 2 * rectl.xRight / 3;
          bitmapRect.xLeft   = rectl.xRight/2 - bitmapSize/2;
          bitmapRect.xRight  = bitmapRect.xLeft + bitmapSize;
          bitmapRect.yBottom = rectl.yTop - 5*bitmapSize/4;
          bitmapRect.yTop    = bitmapRect.yBottom + bitmapSize;

        }
        break;

     case WM_PAINT:
        {
          HPS    hps;
          HBITMAP hbm;
          POINTL  point;
          RECTL   rectl;

          hps = WinBeginPaint(hwnd, 0L, 0L);

          WinQueryWindowRect(hwnd, &rectl);
          WinFillRect (hps, &rectl, SYSCLR_DIALOGBACKGROUND);
          WinDrawBorder(hps, &rectl, 5, 5, 0L, 0L, DB_DLGBORDER);

          hbm = GpiLoadBitmap(hps, 0L, idBitmap, bitmapSize, bitmapSize);
          point.x = bitmapRect.xLeft;
          point.y = bitmapRect.yBottom;
          WinDrawBitmap (hps, hbm, 0L, &point, 0L, 0L, DBM_NORMAL);
          GpiDeleteBitmap (hbm);

          WinEndPaint(hwnd);

        }
        break;

     case WM_TIMER:
        {
          USHORT timerid;
          timerid = SHORT1FROMMP( mp1 );

          switch(timerid)
          {
            case TID_BINGOTIMER:
            {
              HAB hab;

              hab = WinQueryAnchorBlock (hwnd);
              WinStopTimer (hab, hwnd, TID_BINGOTIMER);
              WinStopTimer (hab, hwnd, ID_BINGOTIMER);
              WinDismissDlg (hwnd, TRUE);
            }
            break;

            case ID_BINGOTIMER:
              idBitmap++;
              if(idBitmap > IDBMP_BINGO5)
                idBitmap = IDBMP_BINGO1;
              WinInvalidateRect(hwnd, &bitmapRect, FALSE);
              break;

            default:
              break;

          }

        }
        break;

     case WM_COMMAND:
        switch (SHORT1FROMMP(mp1))
           {
            case DID_OK:
            {
               HAB hab;

               hab = WinQueryAnchorBlock (hwnd);
               WinStopTimer (hab, hwnd, TID_BINGOTIMER);
               WinStopTimer (hab, hwnd, ID_BINGOTIMER);
               WinDismissDlg (hwnd, TRUE);
            }
            break;

            default:
               break;
           }
        break;

     default:
        mr = WinDefDlgProc (hwnd, msg, mp1, mp2);
        break;
    }

 return (mr);

}

/******************************************************************************/
/*                                                                            */
/* wpCardDlgProc: Window procedure for List of Cards dialog window.           */
/*                                                                            */
/******************************************************************************/

MRESULT EXPENTRY wpCardDlgProc (HWND hwnd, ULONG msg, MPARAM mp1,
                                 MPARAM mp2)

{

 MRESULT mr = (MRESULT) FALSE;

 switch (msg)
    {
     case WM_INITDLG:                  /* Dialog box just created. Initialize */
        {                              /* list box, etc.                      */
          PCARDLIST item = startOther;
          LONG      i = 0;
          CHAR      szItem[TITLESIZE+1];

          if(numberofplayers > 0)
          {

            WinLoadString(hab, 0L, IDS_HALLTITLE, TITLESIZE, szTitle);

            while(item)
            {
              sprintf(szItem, "%s %d", szTitle, i+1);
              WinSendDlgItemMsg( hwnd, IDL_CARDS, LM_INSERTITEM,
                                 MPFROMLONG( LIT_END ),
                                 MPFROMP( szItem ) );
              if(item->hwndCard != 0)
              {
                WinSendDlgItemMsg( hwnd, IDL_CARDS, LM_SELECTITEM,
                             MPFROMLONG( i ), MPFROMSHORT( TRUE ) );
              }
              item = (PCARDLIST) item->nextcard;
              i++;
            }
          }
          else
            WinEnableWindow(WinWindowFromID(hwnd, DID_OK), FALSE);
        }
        break;                         /* end case (WM_INITDLG)               */

     case WM_COMMAND:
        switch (SHORT1FROMMP(mp1))
           {
            case DID_OK:               /* User clicked on OK push button      */
               {

                 FRAMECDATA fcData;
                 SHORT      sel, index;
                 CHAR       szItem[TITLESIZE+1];
                 LONG       i, x, y, cx, cy, cz;
                 RECTL      rectl;
                 PCARDLIST  item;

                 fcData.cb            = sizeof (FRAMECDATA);
                 fcData.flCreateFlags =  FCF_TITLEBAR      |
                                         FCF_SYSMENU       |
                                         FCF_SIZEBORDER    |
                                         FCF_SHELLPOSITION;
                 fcData.hmodResources = 0L;
                 fcData.idResources   = idFrameWindow;

                 item = startOther;    /* Now process the virtual bingo cards */
                 while( item )         /* Destroy all windows if found.       */
                 {
                   if(item->hwndCard != 0)
                   {
                     WinDestroyWindow(item->hwndCard);
                     item->hwndCard   = 0L;
                     item->hwndClient = 0L;
                   }
                   item = (PCARDLIST)item->nextcard;
                 }

                 WinLoadString(hab, 0L, IDS_HALLTITLE, TITLESIZE, szTitle);

                 WinQueryWindowRect (HWND_DESKTOP, &rectl);
                 cx = rectl.xRight / 5;
                 cy = rectl.yTop / 3;
                 cz = rectl.xRight / 20;

                 x = 4 * cx + cz;
                 y = 0;
                 index = LIT_FIRST;    /* selected in the list                */
                 sel = (SHORT)WinSendDlgItemMsg(hwnd, IDL_CARDS,
                              LM_QUERYSELECTION, MPFROMSHORT(index), 0L);
                 while( sel != LIT_END ) /* loop until end of the list box    */
                 {
                   item = startOther;
                   for( i=0; i < sel; i++ )
                     item = (PCARDLIST)item->nextcard;

                   if(item->hwndCard == 0)
                   {
                     sprintf(szItem, "%s %d", szTitle, i+1);
                     item->hwndCard = WinCreateWindow (HWND_DESKTOP, WC_FRAME,
                                      NULL, 0L, 0, 0, 0, 0,
                                      0L, HWND_TOP, idFrameWindow,
                                      &fcData, NULL);
                     item->hwndClient = WinCreateWindow (item->hwndCard,
                                        szBingoClass,
                                        NULL, 0L, 0, 0, 0, 0,
                                        0L, HWND_TOP, FID_CLIENT,
                                        (PVOID)item->pcd, NULL);
                     if (hwndHelp)
                       WinAssociateHelpInstance(hwndHelp, item->hwndCard);
                     WinSetWindowText( item->hwndCard, szItem );
                     x = x - cz;
                     WinSetWindowPos (item->hwndCard, HWND_TOP, x, y, cx, cy,
                                      SWP_SHOW | SWP_SIZE |
                                      SWP_MOVE | SWP_ACTIVATE);
                     WinShowWindow( item->hwndCard, TRUE );
                   }

                   index = sel;
                   sel = (SHORT)WinSendDlgItemMsg(hwnd, IDL_CARDS,
                                LM_QUERYSELECTION, MPFROMSHORT(index), 0L);
                 }

                 WinDismissDlg( hwnd, TRUE );       /* remove the dialog box  */
               }                       /* end case (DID_OK)                   */
               break;

            case DID_CANCEL:           /* User clicked on Cancel push button  */
               WinDismissDlg( hwnd, FALSE );
               break;

            case IDP_CARDHELP:
               break;

            case IDP_ALL:              /* All pushbutton clicked, select all  */
               {                       /* entries in listbox (which are cards)*/
                  LONG i;              /* and send DID_OK message             */

                  for(i=0; i< numberofplayers; i++)
                    WinSendDlgItemMsg( hwnd, IDL_CARDS, LM_SELECTITEM,
                                       MPFROMLONG( i ), MPFROMSHORT( TRUE ) );

                  WinSendMsg(hwnd, WM_COMMAND, MPFROM2SHORT(DID_OK,TRUE), 0L);
               }                       /* end case (IPD_ALL)                  */
               break;

            case IDP_NONE:             /* None pushbutton clicked, deselect   */
               {                       /* all entries in listbox and send     */
                  LONG i;              /* DID_OK message.                     */

                  for(i=0; i< numberofplayers; i++)
                    WinSendDlgItemMsg( hwnd, IDL_CARDS, LM_SELECTITEM,
                                       MPFROMLONG( i ), MPFROMSHORT( FALSE ) );

                  WinSendMsg(hwnd, WM_COMMAND, MPFROM2SHORT(DID_OK,TRUE), 0L);
               }                       /* end case (IPD_NONE)                 */
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
/* wpChatDlgProc: Window procedure for Chatter dialog window.                 */
/*                                                                            */
/******************************************************************************/

MRESULT EXPENTRY wpChatDlgProc (HWND hwnd, ULONG msg, MPARAM mp1,
                                 MPARAM mp2)

{

 MRESULT mr = (MRESULT) FALSE;

 switch (msg)
    {
     case WM_INITDLG:                  /* Dialog box just created. Initialize */
        {                              /* list box, etc.                      */
          if( hwndHelp )
           WinAssociateHelpInstance( hwndHelp, hwnd );
          WinSendDlgItemMsg( hwnd, IDL_CHATTER, LM_DELETEALL, 0L, 0L );
          WinSendDlgItemMsg( hwnd, IDL_NUMBERS, LM_DELETEALL, 0L, 0L );
        }
        break;                         /* end case (WM_INITDLG)               */

     case UM_NEWGAME:                  /* New game, clear the chatter listbox */
        WinSendDlgItemMsg( hwnd, IDL_CHATTER, LM_DELETEALL, 0L, 0L );
        WinSendDlgItemMsg( hwnd, IDL_NUMBERS, LM_DELETEALL, 0L, 0L );
        break;                         /* end case (UM_NEWGAME)               */

     case UM_CHAT:                     /* Insert chatter (text) passed in mp1 */
        {                              /* to end of listbox and select it     */
          SHORT index;                 /* so it automatically scrolls         */

          index = (SHORT)WinSendMsg( WinWindowFromID(hwnd, IDL_CHATTER ),
                                     LM_INSERTITEM, MPFROMLONG(LIT_END),
                                     MPFROMP(PVOIDFROMMP(mp1)) );
          WinSendDlgItemMsg( hwnd, IDL_CHATTER, LM_SELECTITEM,
                             MPFROMLONG(index), MPFROMSHORT(TRUE) );
        }
        break;                         /* end case (UM_CHAT)                  */

     case UM_NUMBER:                   /* Insert bingo call no  passed in mp1 */
        {                              /* to end of listbox and select it     */
          SHORT index;                 /* so it automatically scrolls         */

          index = (SHORT)WinSendMsg( WinWindowFromID(hwnd, IDL_NUMBERS ),
                                     LM_INSERTITEM, MPFROMLONG(LIT_END),
                                     MPFROMP(PVOIDFROMMP(mp1)) );
          WinSendDlgItemMsg( hwnd, IDL_NUMBERS, LM_SELECTITEM,
                             MPFROMLONG(index), MPFROMSHORT(TRUE) );
        }
        break;                         /* end case (UM_NUMBER)                */

     case WM_COMMAND:
        switch (SHORT1FROMMP(mp1))
           {
            case DID_OK:               /* User clicked on Close push button   */
               {                       /* Just hide the Chatter dialog window */
                 WinSendMsg( hwndBoard, WM_COMMAND,
                             MPFROM2SHORT(IDM_CHATTER, TRUE), 0L );
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
/* wpHiScoresDlgProc: Window procedure for High Scores dialog window.         */
/*                                                                            */
/******************************************************************************/

MRESULT EXPENTRY wpHiScoresDlgProc (HWND hwnd, ULONG msg, MPARAM mp1,
                                    MPARAM mp2)

{

 MRESULT mr = (MRESULT) FALSE;

 switch (msg)
    {
     case WM_INITDLG:                  /* Dialog box just created. Initialize */
        {                              /* the read only entry fields with     */
           CHAR szScore[SCORESIZE+1];  /* high scores from phsd               */
           LONG i;

           for(i=0; i < MAXHISCORES; i++)
           {
             WinSetWindowText(WinWindowFromID(hwnd, IDE_PLAYER1+i),
                              phsd->player[i]);
             sprintf(szScore, "%s%d", szMoney, phsd->score[i]);
             WinSetWindowText(WinWindowFromID(hwnd, IDE_HISCORE1+i),
                              szScore);
           }

        }
        break;                         /* end case (WM_INITDLG)               */

     case WM_COMMAND:
        switch (SHORT1FROMMP(mp1))
           {
            case DID_OK:               /* User clicked on OK push button      */
               WinDismissDlg( hwnd, TRUE );
               break;

            case IDP_HIHELP:
               break;

            case IDP_RESET:
               fnResetHiScores( );
               WinDismissDlg( hwnd, TRUE );
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
/* wpAboutDlgProc: Window procedure to process Product Information dlg box.   */
/*                                                                            */
/******************************************************************************/

MRESULT EXPENTRY wpAboutDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)

{

 MRESULT mr = (MRESULT) FALSE;

 switch (msg)
    {
     case WM_CHAR:
       {
         if(CHARMSG(&msg)->fs & KC_KEYUP)
           return(MRESULT)FALSE;
         if(CHARMSG(&msg)->fs & KC_VIRTUALKEY)
         {
           switch( CHARMSG(&msg)->vkey )
           {
              case VK_ESC:
              case VK_NEWLINE:
              case VK_ENTER:
                WinDismissDlg( hwnd, TRUE );
                return(MRESULT)TRUE;
           }
         }
       }
       break;

     case WM_INITDLG:
        {
         LONG lLogoDisplay;

         lLogoDisplay = * (PLONG) PVOIDFROMMP (mp2);

         if (lLogoDisplay != -1)
            WinStartTimer (hab, hwnd, TID_LOGOTIMER, lLogoDisplay);

         if( SHAREWARE )
           WinLoadString( hab, 0L, IDS_SHAREWARE,  TEXTSIZE, szText);
         else
           WinLoadString( hab, 0L, IDS_REGISTERED,  TEXTSIZE, szText);
         WinSetWindowText( WinWindowFromID( hwnd, IDS_VERSIONTYPE ), szText );

         WinLoadString( hab, 0L, IDS_VERSION, TEXTSIZE, szText);
         WinSetWindowText( WinWindowFromID( hwnd, IDS_VERSIONNO ), szText );

         WinPostMsg(hwnd, UM_INITDLG, 0L, 0L);

        }
        break;

     case WM_TIMER:
        {
         WinStopTimer (hab, hwnd, TID_LOGOTIMER);
         WinDismissDlg (hwnd, TRUE);
        }
        break;

     case UM_INITDLG:
       if(sound == ON)
        fnSound( SOUND_WELCOME, 0, 0 );
       break;

     case WM_COMMAND:
        switch (SHORT1FROMMP(mp1))
           {
            case DID_OK:
               WinDismissDlg (hwnd, TRUE);
            default:
               break;
           }
        break;

     default:
        mr = WinDefDlgProc (hwnd, msg, mp1, mp2);
        break;
    }

 return (mr);

}
