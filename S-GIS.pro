QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    qcanvas.cpp \
    qcanvasarea.cpp \
    qlayerview.cpp \
    sdocument.cpp \
    seditor.cpp \
    simage.cpp \
    slayermanager.cpp \
    slogger.cpp \
    sobject.cpp \
    sshape.cpp \
    sshapefactory.cpp \
    sslice.cpp \
    straymgr.cpp

HEADERS += \
    ClassAnnotationTemplate.h \
    global.h \
    mainwindow.h \
    qcanvas.h \
    qcanvasarea.h \
    qlayerview.h \
    sdocument.h \
    seditor.h \
    simage.h \
    slayermanager.h \
    slogger.h \
    sobject.h \
    sobjectfactory.h \
    sshape.h \
    sshapefactory.h \
    sslice.h \
    straymgr.h

FORMS += \
    mainwindow.ui \
    seditor.ui

TRANSLATIONS += \
    S-GIS_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resource/Icons.qrc
