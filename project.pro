VERSION = 0.4
TARGET = ImageUpload
DEFINES += 'VERSION=\\"$${VERSION}\\"'
DEFINES += QT_NO_CAST_TO_ASCII
DEFINES += QT_NO_URL_CAST_FROM_STRING
QMAKE_DISTCLEAN += -r .build
OBJECTS_DIR = .build/intermediate
MOC_DIR     = .build/intermediate
RCC_DIR     = .build/intermediate
UI_DIR      = .build/intermediate
QT = core gui network
SOURCES += main.cpp
