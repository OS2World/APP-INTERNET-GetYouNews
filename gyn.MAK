
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
# compile using:
#                IBM C++ ver 3.08
#                OS/2 toolkit version 4.5
#
# The actions included in this make file are:
#  Compile::C++ Compiler
#  Link::Linker
#  Bind::Resource Bind
#  Compile::Resource Compiler

.SUFFIXES: .C .RC .obj .res

.all: \
    Base64.dll \
    Content.dll \
    DirectRe.dll \
    ForteAge.dll \
    ForteFre.dll \
    GigaNews.dll \
    HogWashe.dll \
    InterCom.dll \
    KNews.dll \
    MicroPla.dll \
    Mozilla.dll \
    MSOE60.dll \
    MSOE55.dll \
    MSOE50.dll \
    MSOE40.dll \
    MR2.dll \
    MTNewsW.dll \
    Newsbin.dll \
    NewsPost.dll \
    NewsPro.dll \
    NewsRove.dll \
    NNTPPowe.dll \
    NNTPYENC.dll \
    NomadNew.dll \
    Ozum.dll \
    PecksPow.dll \
    PowerPos.dll \
    SharkPos.dll \
    SMRUseNe.dll \
    Standard.dll \
    TechLogi.dll \
    Thoth.dll \
    UUDecode.dll \
    UUCode.dll \
    WinVN.dll \
    XNews.dll \
    XNewsL5.dll \
    YANewsWa.dll \
    yEnc.dll \
    yEncPP.dll \
    YPOST.dll \
    gyn_de.dll \
    gyn_IT.dll \
    gyn.exe \
    gynnr.exe


.C.obj:
    @echo " Compile::C++ Compiler "
    icc.exe /Gh /Ti /Tm+ /DTCPV40HDRS /Gm+ /Gd /Ss /DRELEASE /Fo"%|dpfF.obj" /C %s

.RC.res:
    @echo " Compile::Resource Compiler "
    rc.exe -r %s %|dpfF.RES

gyn.exe: \
    gyn.obj \
    {$(LIB)}cppopa3.obj \
    gyn.res
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /STACK:1024000"
     /Fegyn.exe
     gyn.obj
     cppom30.lib
     tcp32dll.lib
     so32dll.lib
     ftpapi.lib
<<
    rc.exe gyn.res gyn.exe

gyn.obj: \
    gyn.C \
    gyn.h \
    version.h \
    {$(INCLUDE)}gyn.h

gyn.res: \
    gyn.RC \
    {$(INCLUDE)}gyn.h


gynnr.exe: \
    gynnr.obj \
    {$(LIB)}cppopa3.obj \
    gyn.res
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /STACK:1024000"
     /Fegynnr.exe
     gynnr.obj
     cppom30.lib
     tcp32dll.lib
     so32dll.lib
     ftpapi.lib
<<
    rc.exe gyn.res gynnr.exe

gynnr.obj: \
    gyn.C \
    gyn.h \
    version.h \
    {$(INCLUDE)}gyn.h
    icc.exe /Gh /Ti /Tm+ /DTCPV40HDRS /DGYNNR /Gm+ /Gd /Ss /Fo"gynnr.obj" /C %s


### German(DE) Support

gyn_de.obj: \
    gyn_de.C \
    gyn.h

gyn_de.res: \
        gyn_de.rc \
        version.h \
        gyn.h


gyn_de.dll: \
        gyn_de.obj \
        gyn_de.res \
    {$(LIB)}cppopa3.obj
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /DLL /DEBUG /STACK:1024000"
     /Fegyn_de.dll
     gyn_de.obj
     gyn_de.def
     cppom30.lib
<<
        rc.exe gyn_de.res gyn_de.dll



### Italian(IT) Support

gyn_IT.obj: \
    gyn_IT.C \
    gyn.h

gyn_IT.res: \
        gyn_IT.rc \
        version.h \
        gyn.h


gyn_IT.dll: \
        gyn_IT.obj \
        gyn_IT.res \
    {$(LIB)}cppopa3.obj
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /DLL /DEBUG /STACK:1024000"
     /Fegyn_IT.dll
     gyn_IT.obj
     gyn_IT.def
     cppom30.lib
<<
        rc.exe gyn_IT.res gyn_IT.dll



### Mozilla 5.0 Support

Mozilla.obj: \
    Mozilla.C \
    Mozilla.h \
    multipart.h \
    gyn.h

Mozilla.res: \
        Mozilla.rc \
        version.h \
        gyn.h


Mozilla.dll: \
        Mozilla.obj \
        Mozilla.def \
        Mozilla.res \
    {$(LIB)}cppopa3.obj
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /DLL /DEBUG /STACK:1024000"
     /FeMozilla.dll
     Mozilla.obj
     Mozilla.def
     cppom30.lib
<<
        rc.exe Mozilla.res Mozilla.dll

### MSOE 6.0 Support

MSOE60.obj: \
    MSOE60.C \
    MSOE60.h \
    multipart.h \
    gyn.h

MSOE60.res: \
        MSOE60.rc \
        version.h \
        gyn.h


MSOE60.dll: \
        MSOE60.obj \
        MSOE60.def \
        MSOE60.res \
    {$(LIB)}cppopa3.obj
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /DLL /DEBUG /STACK:1024000"
     /FeMSOE60.dll
     MSOE60.obj
     MSOE60.def
     cppom30.lib
<<
        rc.exe MSOE60.res MSOE60.dll

###  M$5.5 Support

MSOE55.obj: \
    MSOE55.C \
    MSOE55.h  \
    multipart.h \
    gyn.h

MSOE55.res: \
        MSOE55.rc \
        version.h \
        gyn.h


MSOE55.dll: \
        MSOE55.obj \
        MSOE55.def \
        MSOE55.res \
    {$(LIB)}cppopa3.obj
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /DLL /DEBUG /STACK:1024000"
     /FeMSOE55.dll
     MSOE55.obj
     MSOE55.def
     cppom30.lib
<<
        rc.exe MSOE55.res MSOE55.dll


###  M$5.0 Support

MSOE50.obj: \
    MSOE50.C \
    MSOE50.h \
    multipart.h \
    gyn.h

MSOE50.res: \
        MSOE50.rc \
        version.h \
        gyn.h


MSOE50.dll: \
        MSOE50.obj \
        MSOE50.def \
        MSOE50.res \
    {$(LIB)}cppopa3.obj
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /DLL /DEBUG /STACK:1024000"
     /FeMSOE50.dll
     MSOE50.obj
     MSOE50.def
     cppom30.lib
<<
        rc.exe MSOE50.res MSOE50.dll


###  M$4.0 Support

MSOE40.obj: \
    MSOE40.C \
    MSOE40.h \
    multipart.h \
    gyn.h

MSOE40.res: \
        MSOE40.rc \
        version.h \
        gyn.h


MSOE40.dll: \
        MSOE40.obj \
        MSOE40.def \
        MSOE40.res \
    {$(LIB)}cppopa3.obj
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /DLL /DEBUG /STACK:1024000"
     /FeMSOE40.dll
     MSOE40.obj
     MSOE40.def
     cppom30.lib
<<
        rc.exe MSOE40.res MSOE40.dll

###  MR/2 Support

MR2.obj: \
    MR2.C \
    MR2.h \
    multipart.h \
    gyn.h

MR2.res: \
        MR2.rc \
        version.h \
        gyn.h


MR2.dll: \
        MR2.obj \
        MR2.def \
        MR2.res \
    {$(LIB)}cppopa3.obj
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /DLL /DEBUG /STACK:1024000"
     /FeMR2.dll
     MR2.obj
     MR2.def
     cppom30.lib
<<
        rc.exe MR2.res MR2.dll

###  NNTPPower Support

NNTPPower.obj: \
    NNTPPower.C \
    NNTPPower.h \
    multipart.h \
    gyn.h

NNTPPower.res: \
        NNTPPower.rc \
        version.h \
        gyn.h


NNTPPowe.dll: \
        NNTPPower.obj \
        NNTPPower.def \
        NNTPPower.res \
    {$(LIB)}cppopa3.obj
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /DLL /DEBUG /STACK:1024000"
     /FeNNTPPowe.dll
     NNTPPower.obj
     NNTPPower.def
     cppom30.lib
<<
        rc.exe NNTPPower.res NNTPPowe.dll

###  Nomad News Support

NomadNews.obj: \
    NomadNews.C \
    NomadNews.h \
    multipart.h \
    gyn.h

NomadNews.res: \
        NomadNews.rc \
        version.h \
        gyn.h


NomadNew.dll: \
        NomadNews.obj \
        NomadNews.def \
        NomadNews.res \
    {$(LIB)}cppopa3.obj
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /DLL /DEBUG /STACK:1024000"
     /FeNomadNew.dll
     NomadNews.obj
     NomadNews.def
     cppom30.lib
<<
        rc.exe NomadNews.res NomadNew.dll

###  Forte Agent Support

ForteAgent.obj: \
    ForteAgent.C \
    ForteAgent.h \
    multipart.h \
    gyn.h

ForteAgent.res: \
        ForteAgent.rc \
        version.h \
        gyn.h


ForteAge.dll: \
        ForteAgent.obj \
        ForteAgent.def \
        ForteAgent.res \
    {$(LIB)}cppopa3.obj
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /DLL /DEBUG /STACK:1024000"
     /FeForteAge.dll
     ForteAgent.obj
     ForteAgent.def
     cppom30.lib
<<
        rc.exe ForteAgent.res ForteAge.dll

###  GigaNews Support

Giganews.obj: \
    Giganews.C \
    Giganews.h \
    multipart.h \
    gyn.h

Giganews.res: \
        Giganews.rc \
        version.h \
        gyn.h


Giganews.dll: \
        Giganews.obj \
        Giganews.def \
        Giganews.res \
    {$(LIB)}cppopa3.obj
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /DLL /DEBUG /STACK:1024000"
     /FeGigaNews.dll
     GigaNews.obj
     GigaNews.def
     cppom30.lib
<<
        rc.exe GigaNews.res GigaNews.dll

###  SMR Usenet Support

SMRUsenet.obj: \
    SMRUsenet.C \
    SMRUsenet.h \
    multipart.h \
    gyn.h

SMRUsenet.res: \
        SMRUsenet.rc \
        version.h \
        gyn.h


SMRUsene.dll: \
        SMRUsenet.obj \
        SMRUsenet.def \
        SMRUsenet.res \
    {$(LIB)}cppopa3.obj
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /DLL /DEBUG /STACK:1024000"
     /FeSMRUsene.dll
     SMRUsenet.obj
     SMRUsenet.def
     cppom30.lib
<<
        rc.exe SMRUsenet.res SMRUsene.dll

###  XNews Support

XNews.obj: \
    XNews.C \
    XNews.h \
    multipart.h \
    gyn.h

XNews.res: \
        XNews.rc \
        version.h \
        gyn.h


XNews.dll: \
        XNews.obj \
        XNews.def \
        XNews.res \
    {$(LIB)}cppopa3.obj
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /DLL /DEBUG /STACK:1024000"
     /FeXNews.dll
     XNews.obj
     XNews.def
     cppom30.lib
<<
        rc.exe XNews.res XNews.dll

###  XNewsL5 Support

XNewsL5.obj: \
    XNewsL5.C \
    XNewsL5.h \
    multipart.h \
    gyn.h

XNewsL5.res: \
        XNewsL5.rc \
        version.h \
        gyn.h


XNewsL5.dll: \
        XNewsL5.obj \
        XNewsL5.def \
        XNewsL5.res \
    {$(LIB)}cppopa3.obj
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /DLL /DEBUG /STACK:1024000"
     /FeXNewsL5.dll
     XNewsL5.obj
     XNewsL5.def
     cppom30.lib
<<
        rc.exe XNewsL5.res XNewsL5.dll

###  HogWasher Support

HogWasher.obj: \
    HogWasher.C \
    HogWasher.h \
    multipart.h \
    gyn.h

HogWasher.res: \
        HogWasher.rc \
        version.h \
        gyn.h


HogWashe.dll: \
        HogWasher.obj \
        HogWasher.def \
        HogWasher.res \
    {$(LIB)}cppopa3.obj
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /DLL /DEBUG /STACK:1024000"
     /FeHogWashe.dll
     HogWasher.obj
     HogWasher.def
     cppom30.lib
<<
        rc.exe HogWasher.res HogWashe.dll

###  Internet Communication Center Support

InternetCommunication.obj: \
    InternetCommunication.C \
    InternetCommunication.h \
    multipart.h \
    gyn.h

InternetCommunication.res: \
        InternetCommunication.rc \
        version.h \
        gyn.h


InterCom.dll: \
        InternetCommunication.obj \
        InternetCommunication.def \
        InternetCommunication.res \
    {$(LIB)}cppopa3.obj
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /DLL /DEBUG /STACK:1024000"
     /FeInterCom.dll
     InternetCommunication.obj
     InternetCommunication.def
     cppom30.lib
<<
        rc.exe InternetCommunication.res InterCom.dll

###  Ozum Support

Ozum.obj: \
    Ozum.C \
    Ozum.h \
    multipart.h \
    gyn.h

Ozum.res: \
        Ozum.rc \
        version.h \
        gyn.h


Ozum.dll: \
        Ozum.obj \
        Ozum.def \
        Ozum.res \
    {$(LIB)}cppopa3.obj
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /DLL /DEBUG /STACK:1024000"
     /FeOzum.dll
     Ozum.obj
     Ozum.def
     cppom30.lib
<<
        rc.exe Ozum.res Ozum.dll

###  MTNewsWatcher Support

MTNewsWatcher.obj: \
    MTNewsWatcher.C \
    MTNewsWatcher.h \
    multipart.h \
    gyn.h

MTNewsWatcher.res: \
        MTNewsWatcher.rc \
        version.h \
        gyn.h


MTNewsW.dll: \
        MTNewsWatcher.obj \
        MTNewsWatcher.def \
        MTNewsWatcher.res \
    {$(LIB)}cppopa3.obj
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /DLL /DEBUG /STACK:1024000"
     /FeMTNewsW.dll
     MTNewsWatcher.obj
     MTNewsWatcher.def
     cppom30.lib
<<
        rc.exe MTNewsWatcher.res MTNewsW.dll

###  Thoth Support

Thoth.obj: \
    Thoth.C \
    Thoth.h \
    multipart.h \
    gyn.h

Thoth.res: \
        Thoth.rc \
        version.h \
        gyn.h

Thoth.dll: \
        Thoth.obj \
        Thoth.def \
        Thoth.res \
    {$(LIB)}cppopa3.obj
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /DLL /DEBUG /STACK:1024000"
     /FeThoth.dll
     Thoth.obj
     Thoth.def
     cppom30.lib
<<
        rc.exe Thoth.res Thoth.dll


###  Newsbin Support

Newsbin.obj: \
    Newsbin.C \
    Newsbin.h \
    multipart.h \
    gyn.h

Newsbin.res: \
        Newsbin.rc \
        version.h \
        gyn.h

Newsbin.dll: \
        Newsbin.obj \
        Newsbin.def \
        Newsbin.res \
    {$(LIB)}cppopa3.obj
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /DLL /DEBUG /STACK:1024000"
     /FeNewsbin.dll
     Newsbin.obj
     Newsbin.def
     cppom30.lib
<<
        rc.exe Newsbin.res Newsbin.dll



###  NewsPost Support

NewsPost.obj: \
    NewsPost.C \
    NewsPost.h \
    multipart.h \
    gyn.h

NewsPost.res: \
        NewsPost.rc \
        version.h \
        gyn.h

NewsPost.dll: \
        NewsPost.obj \
        NewsPost.def \
        NewsPost.res \
    {$(LIB)}cppopa3.obj
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /DLL /DEBUG /STACK:1024000"
     /FeNewsPost.dll
     NewsPost.obj
     NewsPost.def
     cppom30.lib
<<
        rc.exe NewsPost.res NewsPost.dll


###  NewsPro Support

NewsPro.obj: \
    NewsPro.C \
    NewsPro.h \
    multipart.h \
    gyn.h

NewsPro.res: \
        NewsPro.rc \
        version.h \
        gyn.h

NewsPro.dll: \
        NewsPro.obj \
        NewsPro.def \
        NewsPro.res \
    {$(LIB)}cppopa3.obj
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /DLL /DEBUG /STACK:1024000"
     /FeNewsPro.dll
     NewsPro.obj
     NewsPro.def
     cppom30.lib
<<
        rc.exe NewsPro.res NewsPro.dll


###  NewsRover Support

NewsRover.obj: \
    NewsRover.C \
    NewsRover.h \
    multipart.h \
    gyn.h

NewsRover.res: \
        NewsRover.rc \
        version.h \
        gyn.h

NewsRove.dll: \
        NewsRover.obj \
        NewsRover.def \
        NewsRover.res \
    {$(LIB)}cppopa3.obj
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /DLL /DEBUG /STACK:1024000"
     /FeNewsRove.dll
     NewsRover.obj
     NewsRover.def
     cppom30.lib
<<
        rc.exe NewsRover.res NewsRove.dll

###  ForteFreeAgent Support

ForteFreeAgent.obj: \
    ForteFreeAgent.C \
    ForteFreeAgent.h \
    multipart.h \
    gyn.h

ForteFreeAgent.res: \
        ForteFreeAgent.rc \
        version.h \
        gyn.h

ForteFre.dll: \
        ForteFreeAgent.obj \
        ForteFreeAgent.def \
        ForteFreeAgent.res \
    {$(LIB)}cppopa3.obj
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /DLL /DEBUG /STACK:1024000"
     /FeForteFre.dll
     ForteFreeAgent.obj
     ForteFreeAgent.def
     cppom30.lib
<<
        rc.exe ForteFreeAgent.res ForteFre.dll



###  DirectReadNews Support

DirectReadNews.obj: \
    DirectReadNews.C \
    DirectReadNews.h \
    multipart.h \
    gyn.h

DirectReadNews.res: \
        DirectReadNews.rc \
        version.h \
        gyn.h

DirectRe.dll: \
        DirectReadNews.obj \
        DirectReadNews.def \
        DirectReadNews.res \
    {$(LIB)}cppopa3.obj
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /DLL /DEBUG /STACK:1024000"
     /FeDirectRe.dll
     DirectReadNews.obj
     DirectReadNews.def
     cppom30.lib
<<
        rc.exe DirectReadNews.res DirectRe.dll



###  TechLogic Support

TechLogic.obj: \
    TechLogic.C \
    TechLogic.h \
    multipart.h \
    gyn.h

TechLogic.res: \
        TechLogic.rc \
        version.h \
        gyn.h

TechLogi.dll: \
        TechLogic.obj \
        TechLogic.def \
        TechLogic.res \
    {$(LIB)}cppopa3.obj
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /DLL /DEBUG /STACK:1024000"
     /FeTechLogi.dll
     TechLogic.obj
     TechLogic.def
     cppom30.lib
<<
        rc.exe TechLogic.res TechLogi.dll



###  YANewsWatcher Support

YANewsWatcher.obj: \
    YANewsWatcher.C \
    YANewsWatcher.h \
    multipart.h \
    gyn.h

YANewsWatcher.res: \
        YANewsWatcher.rc \
        version.h \
        gyn.h

YANewsWa.dll: \
        YANewsWatcher.obj \
        YANewsWatcher.def \
        YANewsWatcher.res \
    {$(LIB)}cppopa3.obj
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /DLL /DEBUG /STACK:1024000"
     /FeYANewsWa.dll
     YANewsWatcher.obj
     YANewsWatcher.def
     cppom30.lib
<<
        rc.exe YANewsWatcher.res YANewsWa.dll



###  KNews Support

KNews.obj: \
    KNews.C \
    KNews.h \
    multipart.h \
    gyn.h

KNews.res: \
        KNews.rc \
        version.h \
        gyn.h

KNews.dll: \
        KNews.obj \
        KNews.def \
        KNews.res \
    {$(LIB)}cppopa3.obj
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /DLL /DEBUG /STACK:1024000"
     /FeKNews.dll
     KNews.obj
     KNews.def
     cppom30.lib
<<
        rc.exe KNews.res KNews.dll



###  MicroPlanet Support

MicroPlanet.obj: \
    MicroPlanet.C \
    MicroPlanet.h \
    multipart.h \
    gyn.h

MicroPlanet.res: \
        MicroPlanet.rc \
        version.h \
        gyn.h

MicroPla.dll: \
        MicroPlanet.obj \
        MicroPlanet.def \
        MicroPlanet.res \
    {$(LIB)}cppopa3.obj
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /DLL /DEBUG /STACK:1024000"
     /FeMicroPla.dll
     MicroPlanet.obj
     MicroPlanet.def
     cppom30.lib
<<
        rc.exe MicroPlanet.res MicroPla.dll


###  NNTPYENC2002 Support

NNTPPower2002.obj: \
    NNTPPower2002.C \
    NNTPPower2002.h \
    multipart.h \
    gyn.h

NNTPPower2002.res: \
        NNTPPower2002.rc \
        version.h \
        gyn.h

NNTPyEnc.dll: \
        NNTPPower2002.obj \
        NNTPPower2002.def \
        NNTPPower2002.res \
    {$(LIB)}cppopa3.obj
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /DLL /DEBUG /STACK:1024000"
     /FeNNTPyEnc.dll
     NNTPPower2002.obj
     NNTPPower2002.def
     cppom30.lib
<<
        rc.exe NNTPPower2002.res NNTPyEnc.dll


###  PecksPowerPost Support

PecksPowerPost.obj: \
    PecksPowerPost.C \
    PecksPowerPost.h \
    multipart.h \
    gyn.h

PecksPowerPost.res: \
        PecksPowerPost.rc \
        version.h \
        gyn.h

PecksPow.dll: \
        PecksPowerPost.obj \
        PecksPowerPost.def \
        PecksPowerPost.res \
    {$(LIB)}cppopa3.obj
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /DLL /DEBUG /STACK:1024000"
     /FePecksPow.dll
     PecksPowerPost.obj
     PecksPowerPost.def
     cppom30.lib
<<
        rc.exe PecksPowerPost.res PecksPow.dll



###  PowerPost2K Support

PowerPost2k.obj: \
    PowerPost2k.C \
    PowerPost2k.h \
    multipart.h \
    gyn.h

PowerPost2k.res: \
        PowerPost2k.rc \
        version.h \
        gyn.h

PowerPos.dll: \
        PowerPost2k.obj \
        PowerPost2k.def \
        PowerPost2k.res \
    {$(LIB)}cppopa3.obj
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /DLL /DEBUG /STACK:1024000"
     /FePowerPos.dll
     PowerPost2k.obj
     PowerPost2k.def
     cppom30.lib
<<
        rc.exe PowerPost2k.res PowerPos.dll



###  SharkPost Support

SharkPost.obj: \
    SharkPost.C \
    SharkPost.h \
    multipart.h \
    gyn.h

SharkPost.res: \
        SharkPost.rc \
        version.h \
        gyn.h


SharkPos.dll: \
        SharkPost.obj \
        SharkPost.def \
        SharkPost.res \
    {$(LIB)}cppopa3.obj
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /DLL /DEBUG /STACK:1024000"
     /FeSharkPos.dll
     SharkPost.obj
     SharkPost.def
     cppom30.lib
<<
        rc.exe SharkPost.res SharkPos.dll

###  base64 Support

Base64.obj: \
    base64.C \
    base64.h \
    gyn.h

Base64.res: \
        Base64.rc \
        version.h \
        gyn.h


Base64.dll: \
        Base64.obj \
        Base64.def \
        Base64.res \
    {$(LIB)}cppopa3.obj
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /DLL /DEBUG /STACK:1024000"
     /FeBase64.dll
     Base64.obj
     Base64.def
     cppom30.lib
<<
        rc.exe Base64.res Base64.dll



###  Content Support

Content.obj: \
    Content.C \
    Content.h \
    multipart.h \
    gyn.h

Content.res: \
        Content.rc \
        version.h \
        gyn.h


Content.dll: \
        Content.obj \
        Content.def \
        Content.res \
    {$(LIB)}cppopa3.obj
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /DLL /DEBUG /STACK:1024000"
     /FeContent.dll
     Content.obj
     Content.def
     cppom30.lib
<<
        rc.exe Content.res Content.dll



### UUCode Support

UUCode.obj: \
    UUCode.C \
    UUCode.h \
    gyn.h

UUCode.res: \
        UUCode.rc \
        version.h \
        gyn.h


UUCode.dll: \
        UUCode.obj \
        UUCode.def \
        UUCode.res \
    {$(LIB)}cppopa3.obj
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /DLL /DEBUG /STACK:1024000"
     /FeUUCode.dll
     UUCode.obj
     UUCode.def
     cppom30.lib
<<
        rc.exe UUCode.res UUCode.dll

UUDeCode.obj: \
    UUDeCode.C \
    UUDeCode.h \
    gyn.h

UUDeCode.res: \
        UUDeCode.rc \
        version.h \
        gyn.h


UUDeCode.dll: \
        UUDeCode.obj \
        UUDeCode.def \
        UUDeCode.res \
    {$(LIB)}cppopa3.obj
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /DLL /DEBUG /STACK:1024000"
     /FeUUDeCode.dll
     UUDeCode.obj
     UUDeCode.def
     cppom30.lib
<<
        rc.exe UUDeCode.res UUDeCode.dll


### WinVN Support

WinVN.obj: \
    WinVN.C \
    WinVN.h \
    gyn.h

WinVN.res: \
        WinVN.rc \
        version.h \
        gyn.h


WinVN.dll: \
        WinVN.obj \
        WinVN.def \
        WinVN.res \
    {$(LIB)}cppopa3.obj
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /DLL /DEBUG /STACK:1024000"
     /FeWinVN.dll
     WinVN.obj
     WinVN.def
     cppom30.lib
<<
        rc.exe WinVN.res WinVN.dll


### yEnc Support

yEnc.obj: \
    yEnc.C \
    yEnc.h \
    gyn.h

yEnc.res: \
        yEnc.rc \
        version.h \
        gyn.h


yEnc.dll: \
        yEnc.obj \
        yEnc.def \
        yEnc.res \
    {$(LIB)}cppopa3.obj
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /DLL /DEBUG /STACK:1024000"
     /FeyEnc.dll
     yEnc.obj
     yEnc.def
     cppom30.lib
<<
        rc.exe yEnc.res yEnc.dll


### Standard Message Support

StandardMsg.obj: \
    StandardMsg.C \
    multipart.h \
    StandardMsg.h

StandardMsg.res: \
        StandardMsg.rc \
        version.h \
        gyn.h


Standard.dll: \
        StandardMsg.obj \
        StandardMsg.def \
        StandardMsg.res \
    {$(LIB)}cppopa3.obj
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /DLL /DEBUG /STACK:1024000"
     /FeStandard.dll
     StandardMsg.obj
     StandardMsg.def
     cppom30.lib
<<
        rc.exe StandardMsg.res Standard.dll

###  yEncPowerPost Support

yEncPowerPost.obj: \
    yEncPowerPost.C \
    yEncPowerPost.h \
    multipart.h \
    gyn.h

yEncPowerPost.res: \
        yEncPowerPost.rc \
        version.h \
        gyn.h

yEncPP.dll: \
        yEncPowerPost.obj \
        yEncPowerPost.def \
        yEncPowerPost.res \
    {$(LIB)}cppopa3.obj
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /DLL /DEBUG /STACK:1024000"
     /FeyEncPP.dll
     yEncPowerPost.obj
     yEncPowerPost.def
     cppom30.lib
<<
        rc.exe yEncPowerPost.res yEncPP.dll


###  yPost Support

yPost.obj: \
    yPost.C \
    yPost.h \
    multipart.h \
    gyn.h

yPost.res: \
        yPost.rc \
        version.h \
        gyn.h

yPOST.dll: \
        yPost.obj \
        yPost.def \
        yPost.res \
    {$(LIB)}cppopa3.obj
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /noe /DLL /DEBUG /STACK:1024000"
     /FeYPOST.dll
     yPost.obj
     yPost.def
     cppom30.lib
<<
        rc.exe yPost.res YPOST.dll




