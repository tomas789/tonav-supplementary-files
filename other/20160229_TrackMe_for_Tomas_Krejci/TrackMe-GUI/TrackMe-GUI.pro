QMAKE_CXXFLAGS += -std=c++11 -pedantic -Wall -Wno-missing-field-initializers
QMAKE_CXXFLAGS_RELEASE += -O3 -fomit-frame-pointer -DNDEBUG
QMAKE_CXXFLAGS_DEBUG += -D_GLIBCXX_DEBUG -DDEBUG

TARGET = TrackMe-GUI
TEMPLATE = app
CONFIG += release
DESTDIR = bin

QT   += core gui widgets network printsupport svg
LIBS += -lpthread -lqwt
INCLUDEPATH += "../TrackMe"

# User-specific settings. Change or ignore.
unix:INCLUDEPATH += "/home/floop/src/qwt/src"
unix:LIBS += -L"/home/floop/src/qwt/lib"
win32:INCLUDEPATH += "C:/qwt-6.1.0/src"
win32:LIBS += -L"C:/qwt-6.1.0/lib"

SOURCES +=\
    main.cpp\
    mainwindow.cpp \
    eventfeeder.cpp \
    recorder.cpp \
    config.cpp \
\
    plots/sensorplot.cpp \
    plots/fixedaspectratioplot.cpp \
    plots/pathplot.cpp \
\
    tabs/trackmetab.cpp \
    tabs/rawsensorstab.cpp \
    tabs/filteredsensorstab.cpp \
    tabs/velocitytab.cpp \
    tabs/displacementtab.cpp \
    tabs/orientationtab.cpp \
    tabs/infotab.cpp \

HEADERS  +=\
    mainwindow.h \
    eventfeeder.h \
    config.h \
    utilities.h \
    recorder.h \
\
    plots/sensorplot.h \
    plots/fixedaspectratioplot.h \
    plots/pathplot.h \
\
    tabs/trackmetab.h \
    tabs/rawsensorstab.h \
    tabs/filteredsensorstab.h \
    tabs/velocitytab.h \
    tabs/displacementtab.h \
    tabs/orientationtab.h \
    tabs/infotab.h \
