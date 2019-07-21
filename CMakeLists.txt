cmake_minimum_required(VERSION 3.14)
project(webview)

set(CMAKE_CXX_STANDARD 17)

message("Building for ${PORT}")
if (${PORT} MATCHES "Cocoa")
    find_library(WEBKIT WebKit)

    add_library(webview main.cpp webview.hpp webview.cpp)

    target_link_libraries(webview ${WEBKIT})
    target_compile_options(webview PUBLIC -DPORT_COCOA)
elseif (${PORT} MATCHES "GTK")
    add_library(webview main.cpp webview.hpp webview_gtk.cpp)
    if (${APPLE}) # Homebrew stuff
        set(ENV{PKG_CONFIG_PATH} "/usr/local/opt/libffi/lib/pkgconfig")
        target_include_directories(webview PUBLIC /usr/local/opt/gettext/include)
        target_link_directories(webview PUBLIC /usr/local/opt/gettext/lib)
    endif ()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK REQUIRED gtk+-3.0)
    target_link_libraries(webview ${GTK_LIBRARIES})
    target_include_directories(webview PUBLIC ${GTK_INCLUDE_DIRS})
    target_compile_options(webview PUBLIC ${GTK_CFLAGS_OTHER} -DPORT_GTK)
else ()
    message(WARNING "PORT should be \"Cocoa\" or \"GTK\"")
endif ()