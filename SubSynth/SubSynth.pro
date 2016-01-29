#-------------------------------------------------
#
# Project created by QtCreator 2014-01-08T19:44:10
#
#-------------------------------------------------

QT       += core gui multimedia
LIBS 	 += -lportaudio

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SubSynth
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    Settings.cpp \
    AudioPlayer.cpp \
    Wav.cpp

HEADERS  += mainwindow.h \
    Settings.h \
    AudioPlayer.h \
    Wav.h

FORMS    += mainwindow.ui

RESOURCES += \
    images.qrc
