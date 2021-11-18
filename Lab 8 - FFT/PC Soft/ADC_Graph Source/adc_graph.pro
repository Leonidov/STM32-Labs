#-------------------------------------------------
#
# Project created by QtCreator 2015-04-10T17:45:06
#
#-------------------------------------------------

QT       += core gui
QT       += serialport
QT       += printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = adc_graph
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp

HEADERS  += mainwindow.h \
    qcustomplot.h

FORMS    += mainwindow.ui
