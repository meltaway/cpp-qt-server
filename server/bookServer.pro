QT += gui

QT += network
QT += sql

CONFIG += c++1z console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp \
    server.cpp \
    sqlite_storage.cpp

HEADERS += \
    server.h \
    sqlite_storage.h




win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../json_utils/release/ -ljson_utils
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../json_utils/debug/ -ljson_utils
else:unix: LIBS += -L$$OUT_PWD/../json_utils/ -ljson_utils

INCLUDEPATH += $$PWD/../json_utils
DEPENDPATH += $$PWD/../json_utils

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../libstorage/release/ -llibstorage
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../libstorage/debug/ -llibstorage
else:unix: LIBS += -L$$OUT_PWD/../libstorage/ -llibstorage

INCLUDEPATH += $$PWD/../libstorage
DEPENDPATH += $$PWD/../libstorage

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../json_utils/release/ -ljson_utils
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../json_utils/debug/ -ljson_utils
else:unix: LIBS += -L$$OUT_PWD/../json_utils/ -ljson_utils

INCLUDEPATH += $$PWD/../json_utils
DEPENDPATH += $$PWD/../json_utils

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../libstorage/release/ -llibstorage
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../libstorage/debug/ -llibstorage
else:unix: LIBS += -L$$OUT_PWD/../libstorage/ -llibstorage

INCLUDEPATH += $$PWD/../libstorage
DEPENDPATH += $$PWD/../libstorage
