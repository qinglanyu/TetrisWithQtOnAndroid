QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

android {
QMAKE_LINK += -nostdlib++
QT += androidextras
ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android-sources
}



# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += USE_IMGBTN

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    CCell.cpp \
    CGame.cpp \
    CGameManager.cpp \
    CJBtn.cpp \
    CSizeUtil.cpp \
    Games/Tetris/CTetris.cpp \
    main.cpp \
    CWidget.cpp

HEADERS += \
    CCell.h \
    CGame.h \
    CGameManager.h \
    CJBtn.h \
    CSizeUtil.h \
    CWidget.h \
    Games/Tetris/CTetris.h \
    gameglobal.h \
    qaLog.h

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    audio.qrc \
    fontRes.qrc \
    img.qrc

DISTFILES += \
    android-sources/AndroidManifest.xml
