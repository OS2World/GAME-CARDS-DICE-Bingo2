# Bingo/2 for OS/2

Bingo/2 is a 32-bit, multi-threaded, multi-media OS/2 Presentation Manager
program that lets you play many different types of bingo games on your computer.
Play up to 12 cards against up to 99 virtual hall players, with MMPM/2
digitized sound, multiple game types, and a customizable board.

## Build

Requires Open Watcom 2.0 and OS/2 Toolkit 4.5.

```
compile-wat.cmd
```

Or manually:

```
wmake -f makefile.wat
```

Output: `bin\bingo.exe` and `bin\bingowav.dll`

## Keyboard Shortcuts

| Key    | Action              |
|--------|---------------------|
| Ctrl+N | New Game            |
| Ctrl+Q | Quit Game           |
| Ctrl+P | Pause Game          |
| Ctrl+X | Exit                |
| Ctrl+A | Auto Play toggle    |
| Ctrl+U | Sound toggle        |
| Ctrl+M | Mark All toggle     |
| Ctrl+T | Chatter toggle      |
| Ctrl+C | Other Cards dialog  |
| Ctrl+G | Games dialog        |
| Ctrl+S | Settings dialog     |
| Ctrl+R | Scores dialog       |
| Ctrl+H | High Scores dialog  |
| Ctrl+D | Print dialog        |
| Ctrl+F | Frame Controls      |
| F2     | New Game            |
| F5     | Next Number         |

## License

GNU General Public License Version 3 — see `doc/LICENSE`

## Authors

- E. L. Zapanta (IBM Corp.) — original 1992-1994
- OS2World port — 2026

## Links

- https://www.os2world.com/games/index.php/native-games/cards-dice/173-bingo-2
