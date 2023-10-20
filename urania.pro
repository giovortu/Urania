QT       += core gui xml sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET=urania

DESTDIR=$$PWD/build

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $$PWD/src $$PWD/libs/tidy/include

LIBS+=$$PWD/libs/tidy/tidy_static.lib

include( $$PWD/src/settings/settings.pri )


SOURCES +=  $$PWD/main.cpp \
            $$PWD/src/Library.cpp \
            $$PWD/src/Book.cpp \
            $$PWD/src/MainWindow.cpp \
            $$PWD/src/DbManager.cpp \
            $$PWD/src/SearchResultDialog.cpp \
            $$PWD/src/StarRating.cpp \
            $$PWD/src/Statistics.cpp

HEADERS += \
            $$PWD/src/Library.h \
            $$PWD/src/Book.h \
            $$PWD/src/MainWindow.h \
            $$PWD/src/DbManager.h \
            $$PWD/src/SearchResultDialog.h \
            $$PWD/src/StarRating.h \
            $$PWD/src/Statistics.h

FORMS += \
            $$PWD/ui/MainWindow.ui \
            $$PWD/ui/SearchResultDialog.ui \
            $$PWD/ui/Statistics.ui

RESOURCES+= $$PWD/res/qdarkstyle/qdarkstyle.qrc \
            $$PWD/res/img/img.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
