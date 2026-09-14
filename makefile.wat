# Bingo/2 for OS/2 - Open Watcom makefile
# Usage: wmake -f makefile.wat
# Builds: bin\bingo.exe  bin\bingowav.dll

CC      = wcc386
LINK    = wlink
RC      = wrc
IPFC    = ipfc

CFLAGS  = -bt=os2 -mf -5 -fpi -Oaxt -W3 -ze -d0 -i=src
LFLAGS  = system os2v2 pm option stack=65536 option heap=4096

# ---- BINGO.EXE targets ----

EXE     = bin\bingo.exe
EXERES  = bin\bingo.res
EXEDEF  = src\bingo.def

EXEOBJS = bin\bingo.obj bin\bingodlg.obj bin\bingoprt.obj bin\bingosnd.obj bin\random.obj

# ---- BINGOWAV.DLL targets ----

DLL     = bin\bingowav.dll
DLLRES  = bin\bingowav.res
DLLDEF  = src\bingowav.def

DLLOBJS = bin\bingowav.obj

HLP     = bin\bingo.hlp

# ---- Default target ----

all: $(DLL) $(EXE) $(HLP)

# ---- EXE link ----

$(EXE): $(EXEOBJS) $(EXERES)
	$(LINK) $(LFLAGS) name $(EXE) file bin\bingo.obj,bin\bingodlg.obj,bin\bingoprt.obj,bin\bingosnd.obj,bin\random.obj
	$(RC) $(EXERES) $(EXE)

# ---- EXE resources ----

$(EXERES): src\bingo.rc src\bingo.h src\bingodlg.h src\bingo.dlg src\bingo.ico src\bingo.bmp src\bingo1.bmp src\bingo2.bmp src\bingo3.bmp src\bingo4.bmp src\bingo5.bmp src\marker.ptr src\hand.ptr src\pencil.ptr src\hapyface.ptr src\dollar.ptr src\clover.ptr src\spot.ptr src\startrek.ptr src\bingo.ptr src\check.ptr src\x.bmp src\y.bmp src\z.bmp src\m.bmp src\n.bmp src\h.bmp src\diamond.bmp src\4corners.bmp src\center.bmp src\insquare.bmp src\outsquar.bmp src\allno.bmp src\classic.bmp src\t.bmp src\lucky7.bmp src\hardaway.bmp src\card.bmp src\hlpbingo.bmp src\hlpboard.bmp src\hlpcard.bmp src\hlpchat.bmp src\hlpfont.bmp src\hlpgame.bmp src\hlphiscr.bmp src\hlpother.bmp src\hlpprint.bmp src\hlpscore.bmp src\hlpset.bmp
	$(RC) -bt=os2 -i=src -r -fo=$(EXERES) src\bingo.rc

# ---- EXE object files ----

bin\bingo.obj: src\bingo.c src\bingo.h src\bingodlg.h src\bingowav.h
	$(CC) $(CFLAGS) -fo=bin\bingo.obj src\bingo.c

bin\bingodlg.obj: src\bingodlg.c src\bingo.h src\bingodlg.h
	$(CC) $(CFLAGS) -fo=bin\bingodlg.obj src\bingodlg.c

bin\bingoprt.obj: src\bingoprt.c src\bingo.h src\bingodlg.h
	$(CC) $(CFLAGS) -fo=bin\bingoprt.obj src\bingoprt.c

bin\bingosnd.obj: src\bingosnd.c src\bingo.h src\bingowav.h
	$(CC) $(CFLAGS) -fo=bin\bingosnd.obj src\bingosnd.c

bin\random.obj: src\random.c
	$(CC) $(CFLAGS) -fo=bin\random.obj src\random.c

# ---- DLL link ----

$(DLL): $(DLLOBJS) $(DLLRES)
	$(LINK) system os2v2 dll name $(DLL) file bin\bingowav.obj
	$(RC) $(DLLRES) $(DLL)

# ---- DLL resources ----

$(DLLRES): src\bingowav.rc src\bingowav.h
	$(RC) -bt=os2 -i=src -r -fo=$(DLLRES) src\bingowav.rc

# ---- DLL object files ----

bin\bingowav.obj: src\bingowav.c src\bingowav.h
	$(CC) $(CFLAGS) -fo=bin\bingowav.obj src\bingowav.c

# ---- HLP ----

$(HLP): help\bingo.ipf
	$(IPFC) help\bingo.ipf /W3
	copy bingo.hlp $(HLP)
	del bingo.hlp

# ---- clean ----

clean: .SYMBOLIC
	@if exist bin\bingo.obj    del bin\bingo.obj
	@if exist bin\bingodlg.obj del bin\bingodlg.obj
	@if exist bin\bingoprt.obj del bin\bingoprt.obj
	@if exist bin\bingosnd.obj del bin\bingosnd.obj
	@if exist bin\random.obj   del bin\random.obj
	@if exist bin\bingo.res    del bin\bingo.res
	@if exist bin\bingo.exe    del bin\bingo.exe
	@if exist bin\bingo.map    del bin\bingo.map
	@if exist bin\bingowav.obj del bin\bingowav.obj
	@if exist bin\bingowav.res del bin\bingowav.res
	@if exist bin\bingowav.dll del bin\bingowav.dll
	@if exist bin\bingowav.map del bin\bingowav.map
	@if exist bin\bingo.hlp    del bin\bingo.hlp
