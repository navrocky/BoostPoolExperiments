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

LIBS += -lboost_system

QMAKE_CXXFLAGS    = -std=c++11


SOURCES += main.cpp
