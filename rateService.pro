#-------------------------------------------------
#
# Project created by QtCreator 2013-11-13T20:24:52
#
#-------------------------------------------------
QT       += core gui
QT       += network
QT       += script

#QT       -= gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = rateService
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    httprequest.cpp

HEADERS += \
    httprequest.h
