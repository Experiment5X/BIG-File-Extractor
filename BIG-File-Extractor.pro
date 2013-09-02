#-------------------------------------------------
#
# Project created by QtCreator 2013-09-01T20:08:36
#
#-------------------------------------------------

QT       += core gui

TARGET = BIG-File-Extractor
TEMPLATE = app

win32 {
    LIBS += -LC:/XboxInternals/lib/ -lXboxInternals
    INCLUDEPATH += C:/XboxInternals/include
}

win32::RC_FILE = BIG-File-Extractor.rc

SOURCES += main.cpp\
        MainWindow.cpp \
    AboutDialog.cpp

HEADERS  += MainWindow.h \
    AboutDialog.h

FORMS    += MainWindow.ui \
    AboutDialog.ui

RESOURCES += \
    resources.qrc
