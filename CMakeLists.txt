cmake_minimum_required(VERSION 3.14)
project(webview)

set(CMAKE_CXX_STANDARD 17)

if (${APPLE})
    add_compile_definitions(OBJC_OLD_DISPATCH_PROTOTYPES)

    find_library(WEBKIT WebKit)

    add_library(webview webview_cocoa.cpp)
    target_link_libraries(webview ${WEBKIT} ${APPKIT})

    add_library(webview_c webview_c.cpp webview_cocoa.cpp)
    target_link_libraries(webview_c ${WEBKIT})

    add_executable(webview_test main.cpp)
    target_link_libraries(webview_test webview)

    add_executable(webview_c_test main.c)
    target_link_libraries(webview_c_test webview_c)
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

    add_executable(webview_test main.cpp)
    target_link_libraries(webview_test webview)
else ()
    message(WARNING "Unsupported Operating System")
endif ()