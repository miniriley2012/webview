cmake_minimum_required(VERSION 3.14)
project(webview)

set(CMAKE_CXX_STANDARD 17)

message("Building for ${PORT}")
if (${PORT} MATCHES "Cocoa")
    find_library(WEBKIT WebKit)

    add_library(webview MenuBar.cpp webview.cpp)
    target_link_libraries(webview ${WEBKIT})
    target_compile_options(webview PUBLIC -DPORT_COCOA)

    add_library(webview_c webview_c.cpp webview.cpp)
    target_link_libraries(webview_c ${WEBKIT})
    target_compile_definitions(webview_c PUBLIC -DPORT_COCOA)

    add_executable(cpp main.cpp)
    target_link_libraries(cpp webview)
    target_compile_definitions(cpp PUBLIC -DPORT_COCOA)

    add_executable(c main.c)
    target_link_libraries(c webview_c)
    target_compile_definitions(c PUBLIC -DPORT_COCOA)

    enable_testing()
    add_test(cpp_test cpp)
    add_test(c_test c)
elseif (${PORT} MATCHES "GTK")
    add_library(webview webview_gtk.cpp)
    if (${APPLE}) # Homebrew stuff
        set(ENV{PKG_CONFIG_PATH} "/usr/local/opt/libffi/lib/pkgconfig")
        target_include_directories(webview PUBLIC /usr/local/opt/gettext/include)
        target_link_directories(webview PUBLIC /usr/local/opt/gettext/lib)
    endif ()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK REQUIRED gtk+-3.0)
    pkg_check_modules(WEBKIT2GTK webkit2gtk-4.0)
    target_link_libraries(webview ${GTK_LIBRARIES} ${WEBKIT2GTK_LIBRARIES})
    target_include_directories(webview PUBLIC ${GTK_INCLUDE_DIRS} ${WEBKIT2GTK_INCLUDE_DIRS})
    target_compile_options(webview PUBLIC ${GTK_CFLAGS_OTHER} ${WEBKIT2GTK_CFLAGS_OTHER} -DPORT_GTK)

    add_executable(cpp main.cpp webview_gtk.cpp)
    target_link_libraries(cpp webview)
    target_compile_definitions(cpp PUBLIC -DPORT_GTK)

    enable_testing()
    add_test(cpp_test cpp)
else ()
    message(WARNING "PORT should be \"Cocoa\" or \"GTK\"")
endif ()