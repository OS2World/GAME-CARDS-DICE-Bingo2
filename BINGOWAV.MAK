bingowav.dll: bingowav.obj bingowav.res
   link386 /NOI /MAP bingowav.obj,bingowav.dll,bingowav.map,,bingowav.def;
   rc bingowav.res bingowav.dll

bingowav.res: bingowav.rc bingowav.h
   rc -r bingowav.rc

bingowav.obj: bingowav.c
   icc /c /Ss /O+ /Ti- bingowav.c
