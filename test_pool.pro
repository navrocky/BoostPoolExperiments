#-------------------------------------------------
#
# Project created by QtCreator 2015-04-09T19:40:25
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = test_pool
CONFIG   += console c++11
CONFIG   -= app_bundle

TEMPLATE = app


LIBS += -Lc:/Boost_1_56/lib/ -llibboost_system-vc100-mt-gd-1_56
#LIBS += -lboost_system

QMAKE_CXXFLAGS    = -std=c++11

INCLUDEPATH += c:/Boost_1_56/include/boost-1_56/

SOURCES += main.cpp
