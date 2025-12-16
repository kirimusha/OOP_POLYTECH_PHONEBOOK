cmake_minimum_required(VERSION 3.10)
project(PhoneBook)

set(CMAKE_CXX_STANDARD 11)

# НАЙТИ Qt5 Core (минимальная часть Qt для QFile)
find_package(Qt5 COMPONENTS Core REQUIRED)

# Включить директории
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Все исходные файлы
set(SOURCES
    source/Contact.cpp
    source/ContactManager.cpp
    source/FileRepository.cpp  # Теперь будет работать с Qt
    source/PhoneNumber.cpp
    source/Validators.cpp
    source/UI.cpp
    source/main.cpp
)

set(HEADERS
    include/Contact.h
    include/ContactManager.h
    include/FileRepository.h
    include/IContactRepository.h
    include/PhoneNumber.h
    include/UI.h
    include/Validators.h
)

add_executable(phonebook ${SOURCES} ${HEADERS})

# ПОДКЛЮЧИТЬ Qt5 Core
target_link_libraries(phonebook Qt5::Core)

# Флаги компиляции
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(phonebook PRIVATE -Wall -Wextra)
endif()

# Для работы с qDebug в консоли
target_compile_definitions(phonebook PRIVATE QT_MESSAGELOGCONTEXT)