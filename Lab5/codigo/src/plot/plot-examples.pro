
QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = plot
TEMPLATE = app
INCLUDEPATH += /usr/include/c++/4.3

SOURCES += main.cpp\
           mainwindow.cpp \
           qcustomplot.cpp

HEADERS  += mainwindow.h \
            qcustomplot.h

FORMS    += mainwindow.ui

