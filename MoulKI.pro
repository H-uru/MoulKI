# -------------------------------------------------
# Project created by QtCreator 2010-03-21T02:06:00
# -------------------------------------------------
TARGET = MoulKI
TEMPLATE = app
QT += widgets
CONFIG += c++11
INCLUDEPATH += D:/Development/libhsplasma/core \
    D:/Development/libhsplasma/net \
    D:/Development/openssl-1.0.0g/include
LIBS += -lHSPlasma \
    -lHSPlasmaNet \
    -LD:/Development/libhsplasma/net \
    -LD:/Development/libhsplasma/core \
    -LD:/Development/zlib-1.2.6 \
    -LD:/Development/openssl-1.0.0g/out32dll
SOURCES += main.cpp \
    MoulKI.cpp \
    qtAuthClient.cpp \
    qtGameClient.cpp \
    qtVault.cpp \
    LoginDialog.cpp \
    SetActiveDialog.cpp \
    RefDialog.cpp \
    FetchDialog.cpp \
    qtNodeEdit.cpp \
    CreateNodeDialog.cpp \
    pfConsoleParser.cpp \
    qtSDLTreeModel.cpp
HEADERS += MoulKI.h \
    qtAuthClient.h \
    qtGameClient.h \
    qtVault.h \
    LoginDialog.h \
    SetActiveDialog.h \
    RefDialog.h \
    FetchDialog.h \
    qtNodeEdit.h \
    CreateNodeDialog.h \
    pfConsoleParser.h \
    qtSDLTreeModel.h
FORMS += MoulKI.ui \
    LoginDialog.ui \
    SetActiveDialog.ui \
    RefDialog.ui \
    FetchDialog.ui \
    qtNodeEdit.ui \
    CreateNodeDialog.ui
RESOURCES += Resources.qrc
