QT += core network testlib widgets
CONFIG += c++17 console
CONFIG -= app_bundle
QMAKE_CXXFLAGS += -Wno-error=implicit-function-declaration
TARGET = MathClientTests
TEMPLATE = app

HEADERS += ../MathClient/clientcontroller.h
SOURCES += main.cpp \
           ../MathClient/clientcontroller.cpp
