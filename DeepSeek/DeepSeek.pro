QT += quick network

SOURCES += \
        DeepSeekclient.cpp \
        main.cpp

RESOURCES += qml.qrc

QML_IMPORT_PATH =

QML_DESIGNER_IMPORT_PATH =

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/ui/src/apps
!isEmpty(target.path): INSTALLS += target

include(../client/client.pri)
INCLUDEPATH += ../client

HEADERS += \
    DeepSeekclient.h
