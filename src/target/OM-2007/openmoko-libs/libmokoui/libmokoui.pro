TEMPLATE = lib
VERSION = 0.0.2
TARGET = mokoui

HEADERS = \
    moko-alignment.h \
    moko-application.h \
    moko-details-window.h \
    moko-dialog.h \
    moko-dialog-window.h \
    moko-finger-tool-box.h \
    moko-finger-wheel.h \
    moko-finger-window.h \
    moko-fixed.h \
    moko-menu-box.h \
    moko-message-dialog.h \
    moko-nativation-list.h \
    moko-panel-applet.h \
    moko-pixmap-button.h \
    moko-paned-window.h \
    moko-tool-box.h \
    moko-tree-view.h \
    moko-window.h

SOURCES = \
    moko-alignment.c \
    moko-application.c \
    moko-details-window.c \
    moko-dialog.c \
    moko-dialog-window.c \
    moko-finger-tool-box.c \
    moko-finger-wheel.c \
    moko-finger-window.c \
    moko-fixed.c \
    moko-menu-box.c \
    moko-message-dialog.c \
    moko-navigation-list.c \
    moko-panel-applet.c \
    moko-pixmap-button.c \
    moko-paned-window.c \
    moko-tree-view.c \
    moko-tool-box.c \
    moko-window.c

PKGCONFIG += gtk+-2.0

include ( $(OPENMOKODIR)/devel/qmake/openmoko-include.pro )
