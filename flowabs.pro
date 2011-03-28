CONFIG += precompile_header
QT += opengl

PRECOMPILED_HEADER = stable.h

HEADERS = \
    stable.h \
    glview.h \
    mainwindow.h \
    imageview.h \
    glsl_program.h \
    texture_2d.h \
    GLee.h

SOURCES = \ 
    main.cpp \
    mainwindow.cpp \
    imageview.cpp \
    glview.cpp \
    glsl_program.cpp \
    texture_2d.cpp \
    GLee.c

FORMS = mainwindow.ui

RESOURCES = resources.qrc

win32 {
    RC_FILE = flowabs.rc
    DEFINES += _CRT_SECURE_NO_WARNINGS
    exists( "$$(PROGRAMFILES)/QuickTime SDK/CIncludes" ) {
        DEFINES += HAVE_QUICKTIME
        INCLUDEPATH += "$$(PROGRAMFILES)/QuickTime SDK/CIncludes"
        LIBS += "$$(PROGRAMFILES)/QuickTime SDK/Libraries/QTMLClient.lib" Advapi32.lib
        QMAKE_LFLAGS += /NODEFAULTLIB:libcmt
        HEADERS += quicktime.h
        SOURCES += quicktime.cpp
    }
}

mac {
    CONFIG += x86
    ICON = flowabs.icns
    DEFINES += HAVE_QUICKTIME
    LIBS += -framework QuickTime -framework Carbon
    HEADERS += quicktime.h
    SOURCES += quicktime.cpp
}
