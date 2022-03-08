cmake_minimum_required(VERSION 3.7.0)

project(TeaTimer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/source)

set(GUI_HDRS
    source/gui/window.h
    source/gui/taskbar.h
    )

set(GUI_SRCS
    source/gui/window.cpp
    source/gui/taskbar.cpp
    )

set(LOCKER_HDRS source/locker/lock_session.h)

set(LOCKER_SRCS source/locker/lock_session.cpp)

add_executable(TeaTimer WIN32
    ${GUI_HDRS}
    ${GUI_SRCS}
    ${LOCKER_HDRS}
    ${LOCKER_SRCS}
    source/main.cpp
    README.md
    .gitignore
)

target_link_libraries(TeaTimer Qt5::Widgets)

if(WIN32)
    target_link_libraries(TeaTimer Qt5::WinMain)
    add_custom_command(TARGET TeaTimer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Core> $<TARGET_FILE_DIR:TeaTimer>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Gui> $<TARGET_FILE_DIR:TeaTimer>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Widgets> $<TARGET_FILE_DIR:TeaTimer>
        )
endif (WIN32)
