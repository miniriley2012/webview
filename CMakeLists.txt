cmake_minimum_required(VERSION 3.14)
project(webview)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS -ObjC++)

find_library(FOUNDATION Foundation)
find_library(COCOA Cocoa)
find_library(WEBKIT WebKit)

add_executable(objc main.cpp webview.hpp webview.mm)
add_executable(cpp main.cpp webview.hpp webview.cpp)

target_link_libraries(objc ${FOUNDATION} ${COCOA} ${WEBKIT})
target_link_libraries(cpp ${FOUNDATION} ${COCOA} ${WEBKIT})