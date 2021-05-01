QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    TestGit.cpp \
    main.cpp \
    mainwindow.cpp \
    qcanvas.cpp \
    qcanvasarea.cpp \
    sdocument.cpp \
    seditor.cpp \
    simage.cpp \
    slogger.cpp \
    sobject.cpp \
    sshape.cpp \
    sshapefactory.cpp \
    straymgr.cpp

HEADERS += \
    ClassAnotationTemplate.h \
    global.h \
    mainwindow.h \
    qcanvas.h \
    qcanvasarea.h \
    sdocument.h \
    seditor.h \
    simage.h \
    slogger.h \
    sobject.h \
    sshape.h \
    sshapefactory.h \
    straymgr.h

FORMS += \
    mainwindow.ui \
    seditor.ui

TRANSLATIONS += \
    S-GIF_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# OpenCV

win32:CONFIG(release, debug|release): LIBS += -L'D:/Program Files/OpenCV-4.5.0WithQt/x64/vc16/lib/' -lopencv_core450
else:win32:CONFIG(debug, debug|release): LIBS += -L'D:/Program Files/OpenCV-4.5.0WithQt/x64/vc16/lib/' -lopencv_core450d
else:unix: LIBS += -L'D:/Program Files/OpenCV-4.5.0WithQt/x64/vc16/lib/' -lopencv_core450

win32:CONFIG(release, debug|release): LIBS += -L'D:/Program Files/OpenCV-4.5.0WithQt/x64/vc16/lib/' -lopencv_highgui450
else:win32:CONFIG(debug, debug|release): LIBS += -L'D:/Program Files/OpenCV-4.5.0WithQt/x64/vc16/lib/' -lopencv_highgui450d
else:unix: LIBS += -L'D:/Program Files/OpenCV-4.5.0WithQt/x64/vc16/lib/' -lopencv_highgui450

INCLUDEPATH += 'D:/Program Files/OpenCV-4.5.0WithQt/include'
DEPENDPATH += 'D:/Program Files/OpenCV-4.5.0WithQt/include'

RESOURCES += \
    Resource/Icons.qrc
