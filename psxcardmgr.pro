



CONFIG += release c++11
TEMPLATE = app
TARGET = psxcardmgr
DEPENDPATH += .
INCLUDEPATH += .

QT += core gui widgets

DEFINES += VERSION=0.9.1 QT_DEPRECATED_WARNINGS

win32 {
	DEFINES += _PSXPB_WIN32_
} else {
	DEFINES += _PSXPB_LINUX_ _PSX_LINUX_
}

QMAKE_CXXFLAGS += -Wno-unused-parameter
QMAKE_CFLAGS += -Wno-unused-parameter

HEADERS += card_link.h CardSlots.h MainWindow.h PSX_memory_card.h psxpblib.h
SOURCES += card_link.c main.cpp PSX_memory_card.cpp CardSlots.cpp MainWindow.cpp psxpblib.c

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
