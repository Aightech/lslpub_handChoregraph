#-------------------------------------------------
#
# Project created by QtCreator 2019-04-02T18:03:42
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = lslpub_handChoregraph
TEMPLATE = app



CONFIG += c++11

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp

HEADERS += \
    include/mainwindow.h

FORMS += \
    forms/mainwindow.ui

LIBS += -L./lib

unix:!macx: LIBS += \
                    -Wl,-rpath,./lib \
                    -llsl64


win32: LIBS +=  -lliblsl64


INCLUDEPATH += include/lsl
DEPENDPATH += include/lsl

win32:DESTDIR = bin/WIN32/Window
win32:TARGET_CUSTOM_EXT = .exe
win32:DEPLOY_COMMAND = windeployqt
win32:DEPLOY_TARGET = $$shell_quote($$shell_path($${OUT_PWD}/$${DESTDIR}/$${TARGET}$${TARGET_CUSTOM_EXT}))

#warning($${DEPLOY_COMMAND} $${DEPLOY_TARGET})

CONFIG(release, debug|release)
{
    QMAKE_POST_LINK += $$quote($${DEPLOY_COMMAND} $${DEPLOY_TARGET})
    QMAKE_POST_LINK += $$quote($$escape_expand(\n\t) copy $$shell_path(lib/liblsl64.dll) $$shell_path($${DESTDIR}/liblsl64.dll))
    QMAKE_POST_LINK += $$quote($$escape_expand(\n\t) del Makefile)
    QMAKE_POST_LINK += $$quote($$escape_expand(\n\t) del Makefile.Debug)
    QMAKE_POST_LINK += $$quote($$escape_expand(\n\t) del Makefile.Release)
    QMAKE_POST_LINK += $$quote($$escape_expand(\n\t) del .qmake.stash)
    QMAKE_POST_LINK += $$quote($$escape_expand(\n\t) del ui_mainwindow.h)
    QMAKE_POST_LINK += $$quote($$escape_expand(\n\t) rmdir /S /Q release)
    QMAKE_POST_LINK += $$quote($$escape_expand(\n\t) rmdir /S /Q debug)
}

