QT       += core gui
QT       += svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    modules/algorithm/sslice.cpp \
    modules/doc/sdocument.cpp \
    modules/doc/slayermanager.cpp \
    modules/main.cpp \
    modules/paint/sfragimage.cpp \
    modules/paint/sfragloader.cpp \
    modules/paint/sfragmatrix.cpp \
    modules/paint/simage.cpp \
    modules/paint/sobject.cpp \
    modules/paint/sshape.cpp \
    modules/paint/sshapefactory.cpp \
    modules/slogger.cpp \
    modules/widget/mainwindow.cpp \
    modules/widget/qbandselectdialog.cpp \
    modules/widget/qcanvas.cpp \
    modules/widget/qcanvasarea.cpp \
    modules/widget/qlayerview.cpp \
    modules/widget/seditor.cpp \
    modules/widget/straymgr.cpp \
    modules/widget/qbandselectdialog.cpp

HEADERS += \
    modules/ClassAnnotationTemplate.h \
    modules/algorithm/sequalizehist.h \
    modules/algorithm/sslice.h \
    modules/doc/sdocument.h \
    modules/doc/slayermanager.h \
    modules/global.h \
    modules/paint/sfragimage.h \
    modules/paint/sfragloader.h \
    modules/paint/sfragmatrix.h \
    modules/paint/simage.h \
    modules/paint/sobject.h \
    modules/paint/sobjectfactory.h \
    modules/paint/sshape.h \
    modules/paint/sshapefactory.h \
    modules/slogger.h \
    modules/widget/mainwindow.h \
    modules/widget/qbandselectdialog.h \
    modules/widget/qcanvas.h \
    modules/widget/qcanvasarea.h \
    modules/widget/qlayerview.h \
    modules/widget/seditor.h \
    modules/widget/straymgr.h \
    modules/widget/qbandselectdialog.h

FORMS += \
    modules/widget/mainwindow.ui \
    modules/widget/qbandselectdialog.ui \
    modules/widget/seditor.ui \

TRANSLATIONS += \
    S-GIS_zh_CN.ts


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += \
    D:/GDAL/release-1928-x64/include

LIBS += \
    D:/GDAL/release-1928-x64/lib/gdal_i.lib

RESOURCES += \
    Resource/Icons.qrc
