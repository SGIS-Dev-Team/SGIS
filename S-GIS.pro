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
    modules/widget/qcanvas.cpp.orig \
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
