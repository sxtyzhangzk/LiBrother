#-------------------------------------------------
#
# Project created by QtCreator 2015-11-24T17:16:29
#
#-------------------------------------------------

QT       -= core gui

TARGET = LiBrotherClientBackend
TEMPLATE = lib
CONFIG += staticlib
CONFIG += C++11

INCLUDEPATH +=	../../common/ \
				../../liblog/ \
				../../exlib/include/ \

SOURCES +=	../backend_main.cpp \
			../netclient.cpp \
			../authmanager.cpp \
			../book.cpp \
			../library.cpp \
			../user.cpp \
			../usermanager.cpp


HEADERS +=	../authmanager.h \
			../book.h \
			../class_factory_client.h \
			../client_interfaces.h \
			../config.h \
			../library.h \
			../netclient.h \
			../user.h \
			../usermanager.h \
			../utils.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
