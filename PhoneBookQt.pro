QT += core gui widgets

CONFIG += c++17

TARGET = PhoneBookQt
TEMPLATE = app

SOURCES += \
    source/Qt_main.cpp \
    source/Qt.cpp \
    source/Contact.cpp \
    source/ContactManager.cpp \
    source/FileRepository.cpp \
    source/PhoneNumber.cpp \
    source/UI.cpp \
    source/Validators.cpp

HEADERS += \
    include/Qt.h \
    include/Contact.h \
    include/ContactManager.h \
    include/FileRepository.h \
    include/PhoneNumber.h \
    include/UI.h \
    include/Validators.h

INCLUDEPATH += include
DEPENDPATH += include

# Для JSON библиотеки
INCLUDEPATH += third_party

# Компиляция в режиме отладки
CONFIG += debug

# Флаги компилятора
QMAKE_CXXFLAGS += -std=c++17

# Подключение JSON
HEADERS += third_party/json.hpp

FORMS += \
    mainwindow.ui