Bingo/2 for OS/2 Presentation Manager
======================================
Version 1.50 - Open Watcom port

Original Author : E. L. Zapanta (IBM Corp.)
Original Date   : 1992-1994
OS2World port   : 2026


DESCRIPTION
-----------
Bingo/2 is a Bingo hall simulator for OS/2 Presentation Manager. Play
up to 12 cards against up to 99 virtual players, with digitized sound,
multiple game types, and customizable settings.


REQUIREMENTS
------------
- OS/2 Warp 4 / ArcaOS 5.x
- MMPM/2 for digitized sound (optional; falls back to PC speaker)
- 4 MB RAM minimum


FILES
-----
bin\bingo.exe     - Main executable
bin\bingowav.dll  - Digitized sound resource DLL
help\bingo.ipf    - Help source (compile with: ipfc bingo.ipf)


BUILD INSTRUCTIONS
------------------
Requires Open Watcom 2.0 (http://www.openwatcom.org) and OS/2 Toolkit 4.5.

  compile-wat.cmd

This script detects WATCOM and OS2TK, cleans and builds both BINGO.EXE
and BINGOWAV.DLL. Output is logged to compile-wat.log.

Manual build:
  wmake -f makefile.wat


KEYBOARD SHORTCUTS
------------------
Ctrl+N  New Game
Ctrl+Q  Quit Game
Ctrl+P  Pause Game
Ctrl+X  Exit
Ctrl+A  Auto Play toggle
Ctrl+U  Sound toggle
Ctrl+M  Mark All toggle
Ctrl+T  Chatter toggle
Ctrl+C  Other Cards dialog
Ctrl+G  Games dialog
Ctrl+S  Settings dialog
Ctrl+R  Scores dialog
Ctrl+H  High Scores dialog
Ctrl+D  Print dialog
Ctrl+F  Frame Controls toggle
F2      New Game
F5      Next Number


CHANGES FROM ORIGINAL
---------------------
- Ported from IBM C Set/2 to Open Watcom 2.0
- New standard menu layout (Game / Options / Help)
- Added Frame Controls toggle (Ctrl+F)
- BLDLEVEL string embedded in executable
- About dialog updated to standard OS2World format
- Startup splash removed
- Compiler warnings cleaned up


LICENSE
-------
GNU General Public License Version 3 - see doc\LICENSE
