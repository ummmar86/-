QT += core sql testlib
CONFIG += c++17
CONFIG -= app_bundle
QMAKE_CXXFLAGS += -Wno-error=implicit-function-declaration
TARGET = MathServerTests
TEMPLATE = app

HEADERS += ../MathServer/mathsolver.h \
           ../MathServer/database.h \
           ../MathServer/parsing.h
SOURCES += main.cpp \
           ../MathServer/mathsolver.cpp \
           ../MathServer/database.cpp \
           ../MathServer/parsing.cpp
