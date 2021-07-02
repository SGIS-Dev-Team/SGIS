QT       += core gui
QT       += svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    modules/algorithm/soverviewbuilder.cpp \
    modules/algorithm/sslice.cpp \
    modules/doc/sdocument.cpp \
    modules/doc/simagestreammeta.cpp \
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
    modules/widget/qcanvas.cpp \
    modules/widget/qcanvasarea.cpp \
    modules/widget/qdataimportwizard.cpp \
    modules/widget/qimagelistview.cpp \
    modules/widget/qlayerview.cpp \
    modules/widget/seditor.cpp \
    modules/widget/straymgr.cpp \
    modules/widget/wizard/extract/qextractwizard.cpp \
    modules/widget/wizard/extract/qextractwizardpagefile.cpp \
    modules/widget/wizard/extract/qextractwizardpageintro.cpp \
    modules/widget/wizard/extract/qextractwizardpageprogress.cpp \
    modules/widget/wizard/extract/qextractwizardpagetempdir.cpp \
    modules/widget/wizard/extract/qfilelistmodel.cpp \
    modules/widget/wizard/extract/qfilelistview.cpp \
    modules/widget/wizard/extract/qfiletreeview.cpp

HEADERS += \
    modules/ClassAnnotationTemplate.h \
    modules/algorithm/scoordinate.h \
    modules/algorithm/sequalizehist.h \
    modules/algorithm/soverviewbuilder.h \
    modules/algorithm/sslice.h \
    modules/doc/sdocument.h \
    modules/doc/simagestreammeta.h \
    modules/doc/slayermanager.h \
    modules/global.h \
    modules/paint/sfragimage.h \
    modules/paint/sfragloader.h \
    modules/paint/sfragmatrix.h \
    modules/paint/simage.h \
    modules/paint/simagemeta.h \
    modules/paint/sobject.h \
    modules/paint/sobjectfactory.h \
    modules/paint/sshape.h \
    modules/paint/sshapefactory.h \
    modules/slogger.h \
    modules/widget/mainwindow.h \
    modules/widget/qcanvas.h \
    modules/widget/qcanvasarea.h \
    modules/widget/qdataimportwizard.h \
    modules/widget/qimagelistview.h \
    modules/widget/qlayerview.h \
    modules/widget/seditor.h \
    modules/widget/straymgr.h \
    modules/widget/wizard/extract/qextractwizard.h \
    modules/widget/wizard/extract/qextractwizardpagefile.h \
    modules/widget/wizard/extract/qextractwizardpageintro.h \
    modules/widget/wizard/extract/qextractwizardpageprogress.h \
    modules/widget/wizard/extract/qextractwizardpagetempdir.h \
    modules/widget/wizard/extract/qfilelistmodel.h \
    modules/widget/wizard/extract/qfilelistview.h \
    modules/widget/wizard/extract/qfiletreeview.h

FORMS += \
    modules/widget/mainwindow.ui \
    modules/widget/seditor.ui \
    modules/widget/qdataimportwizard.ui \
    modules/widget/wizard/extract/qextractwizard.ui \
    modules/widget/wizard/extract/qextractwizardpagefile.ui \
    modules/widget/wizard/extract/qextractwizardpageintro.ui \
    modules/widget/wizard/extract/qextractwizardpageprogress.ui \
    modules/widget/wizard/extract/qextractwizardpagetempdir.ui

TRANSLATIONS += \
    S-GIS_zh_CN.ts


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += \
    E:/study/GDAL1928/release-1928-x64/include

LIBS += \
    E:/study/GDAL1928/release-1928-x64/lib/gdal_i.lib

RESOURCES += \
    Resource/Icons.qrc

win32:CONFIG(release, debug|release): LIBS += -L'C:/Program Files (x86)/Visual Leak Detector/lib/Win64/' -lvld
else:win32:CONFIG(debug, debug|release): LIBS += -L'C:/Program Files (x86)/Visual Leak Detector/lib/Win64/' -lvld
else:unix: LIBS += -L'C:/Program Files (x86)/Visual Leak Detector/lib/Win64/' -lvld

INCLUDEPATH += 'C:/Program Files (x86)/Visual Leak Detector/include'
DEPENDPATH += 'C:/Program Files (x86)/Visual Leak Detector/include'
