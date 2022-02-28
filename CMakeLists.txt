cmake_minimum_required(VERSION 3.7.0)

project(TeaTimer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

set(WINDOW_HDRS source/window/window.h)
set(WINDOW_SRCS source/window/window.cpp)

add_executable(TeaTimer WIN32
    ${WINDOW_HDRS}
    ${WINDOW_SRCS}
    source/main.cpp
    README.md
    .gitignore
)

target_link_libraries(TeaTimer Qt5::Widgets Qt5::WinMain)

add_custom_command(TARGET TeaTimer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Core> $<TARGET_FILE_DIR:TeaTimer>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Gui> $<TARGET_FILE_DIR:TeaTimer>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Widgets> $<TARGET_FILE_DIR:TeaTimer>
    )
