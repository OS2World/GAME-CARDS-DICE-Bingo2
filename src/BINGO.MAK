bingo.exe: bingo.obj bingodlg.obj bingoprt.obj bingosnd.obj random.obj bingo.def bingo.res
   link386 /NOI /MAP bingo.obj bingodlg.obj bingoprt.obj bingosnd.obj random.obj,,bingo.map,,bingo.def;
   rc bingo.res bingo.exe

bingo.hlp: bingo.ipf
   ipfc bingo.ipf

bingo.res: bingo.rc bingo.h bingodlg.h bingo.dlg bingo.ico bingo.bmp bingo1.bmp bingo2.bmp bingo3.bmp bingo4.bmp bingo5.bmp marker.ptr hand.ptr pencil.ptr hapyface.ptr dollar.ptr clover.ptr spot.ptr startrek.ptr bingo.ptr
   rc -r bingo.rc

bingo.obj: bingo.c bingo.h bingodlg.h
   icc /c /Ss /O+ /Ti- bingo.c

bingodlg.obj: bingodlg.c bingo.h bingodlg.h
   icc /c /Ss /O+ /Ti- bingodlg.c

bingoprt.obj: bingoprt.c bingo.h bingodlg.h
   icc /c /Ss /O+ /Ti- bingoprt.c

bingosnd.obj: bingosnd.c bingo.h bingowav.h
   icc /c /Ss /O+ /Ti- bingosnd.c

random.obj: random.c
   icc /c /Ss /O+ random.c
