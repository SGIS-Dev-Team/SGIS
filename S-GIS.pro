QT       += core gui
QT       += svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


RESOURCES += \
    Resource/Icons.qrc

FORMS += \
    modules/widget/mainwindow.ui \
    modules/widget/qbandselectdialog.ui \
    modules/widget/seditor.ui

DISTFILES += \
    Resource/TrayActionIcon/activate.png \
    Resource/TrayActionIcon/hide.png \
    Resource/TrayActionIcon/maximize.png \
    Resource/TrayActionIcon/notification_off.png \
    Resource/TrayActionIcon/notification_on.png \
    Resource/TrayActionIcon/quit.png \
    Resource/TrayActionIcon/show.png \
    Resource/TrayActionIcon/sleep.png \
    Resource/TrayIcon/active.png \
    Resource/TrayIcon/sleeping.png \
    Resource/WidgetIcon/rotate.svg \
    modules/paint/autoexp.dat

HEADERS += \
    modules/ClassAnnotationTemplate.h \
    modules/algorithm/sdecompression.h \
    modules/algorithm/sequalizehist.h \
    modules/algorithm/soverviewbuilder.h \
    modules/algorithm/sslice.h \
    modules/doc/sdocument.h \
    modules/doc/slayermanager.h \
    modules/global.h \
    modules/main.cpp.orig \
    modules/paint/sfragimage.cpp.orig \
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
    modules/widget/sdialog.h \
    modules/widget/seditor.h \
    modules/widget/sfileinfowidget.h \
    modules/widget/simageinfowidget.h \
    modules/widget/slayerinfowidget.h \
    modules/widget/sotherinfowidget.h \
    modules/widget/sprojectioninfowidget.h \
    modules/widget/straymgr.h

SOURCES += \
    modules/algorithm/sdecompression.cpp \
    modules/algorithm/soverviewbuilder.cpp \
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
    modules/widget/sdialog.cpp \
    modules/widget/seditor.cpp \
    modules/widget/sfileinfowidget.cpp \
    modules/widget/simageinfowidget.cpp \
    modules/widget/slayerinfowidget.cpp \
    modules/widget/sotherinfowidget.cpp \
    modules/widget/sprojectioninfowidget.cpp \
    modules/widget/straymgr.cpp


TRANSLATIONS += \
    S-GIS_zh_CN.ts


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resource/Icons.qrc

# GDAL Config

INCLUDEPATH += \
    D:/GDAL/release-1928-x64/include

LIBS += \
    D:/GDAL/release-1928-x64/lib/gdal_i.lib

# Visual Leak Detector Config

win32:CONFIG(release, debug|release): LIBS += -L'C:/Program Files (x86)/Visual Leak Detector/lib/Win64/' -lvld
else:win32:CONFIG(debug, debug|release): LIBS += -L'C:/Program Files (x86)/Visual Leak Detector/lib/Win64/' -lvld
else:unix: LIBS += -L'C:/Program Files (x86)/Visual Leak Detector/lib/Win64/' -lvld

INCLUDEPATH += 'C:/Program Files (x86)/Visual Leak Detector/include'
DEPENDPATH += 'C:/Program Files (x86)/Visual Leak Detector/include'

# LibArchive Config

win32:CONFIG(release, debug|release): LIBS += -LD:/LibArchive/lib/release/ -larchive
else:win32:CONFIG(debug, debug|release): LIBS += -LD:/LibArchive/lib/debug/ -larchive
else:unix: LIBS += -LD:/LibArchive/lib/ -larchive

INCLUDEPATH += D:/LibArchive/include
DEPENDPATH += D:/LibArchive/include
