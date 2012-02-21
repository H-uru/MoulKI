# -------------------------------------------------
# Project created by QtCreator 2010-03-21T02:06:00
# -------------------------------------------------
TARGET = MoulKI
TEMPLATE = app
INCLUDEPATH += C:/Development/libhsplasma/core \
    C:/Development/libhsplasma/net \
    C:/Development/openssl-1.0.0g/include
LIBS += -lHSPlasma \
    -lHSPlasmaNet \
    -LC:/Development/libhsplasma/net/Debug \
    -LC:/Development/libhsplasma/core/Debug
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
    pfConsoleParser.cpp
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
    pfConsoleParser.h
FORMS += MoulKI.ui \
    LoginDialog.ui \
    SetActiveDialog.ui \
    RefDialog.ui \
    FetchDialog.ui \
    qtNodeEdit.ui \
    CreateNodeDialog.ui
RESOURCES += Resources.qrc
