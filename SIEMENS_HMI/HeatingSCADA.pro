QT += quick quickcontrols2 remoteobjects
CONFIG += c++11
TARGET = HeatingSCADA

# SystemUI integration
include(../client/client.pri)
INCLUDEPATH += ../client

# snap7 static lib
LIBS += $$PWD/snap7/libsnap7.a
INCLUDEPATH += $$PWD/snap7

# link system libs needed by snap7
LIBS += -lpthread -lrt

SOURCES += \
    src/main.cpp \
    src/snap7client.cpp \
    src/plcservice.cpp

HEADERS += \
    src/snap7client.h \
    src/plcdata.h \
    src/plcservice.h

# QML files
RESOURCES += \
    qml/qml.qrc

# Assets (images)
RESOURCES += \
    assets/assets.qrc

# Deploy to SystemUI apps directory
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/ui/src/apps
!isEmpty(target.path): INSTALLS += target
