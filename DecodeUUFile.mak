
# ALL: gyn.EXE
#
# gyn.res : gyn.rc gyn.rch
#      rc -r gyn.rc
#
# gyn.obj : gyn.c
#      ICC /Ti+ /Gm+ /Gd- /Se /Re /ss /Ms /c /W3 gyn.c
#
# gyn.EXE : gyn.res gyn.def gyn.obj
#      link386 /NOE /NOD /CO /EXEPACK /M /BASE:0x10000 @gyn.lnk
#      rc gyn.res
#
#
# gyn.mak
# Created by Nathan at 11:44:40 on 23 May 2002
#
# The actions included in this make file are:
#  Compile::C++ Compiler
#  Link::Linker
#  Bind::Resource Bind
#  Compile::Resource Compiler

.SUFFIXES: .C .RC .obj .res

.all: \
    DecodeUUFile.exe


.C.obj:
    @echo " Compile::C++ Compiler "
    icc.exe /Gh /Ti /Tm+ /DTCPV40HDRS /Gm+ /Gd /Ss /DRELEASE /Fo"%|dpfF.obj" /C %s

.RC.res:
    @echo " Compile::Resource Compiler "
    rc.exe -r %s %|dpfF.RES

DecodeUUFile.exe: \
    DecodeUUFile.obj \
    {$(LIB)}cppopa3.obj
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /STACK:1024000"
     /FeDecodeUUFile.exe
     DecodeUUFile.obj
     cppom30.lib
<<

DecodeUUFile.obj: \
    DecodeUUFile.C \
    DecodeUUFile.h \

