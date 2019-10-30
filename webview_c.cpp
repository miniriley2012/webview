//
// Created by Riley Quinn on 2019-07-24.
//

#include "webview.h"
#include "webview.hpp"

WebViewMenuItem newMenuItem(const char *name, const char *key, void (*handler)()) {
    return static_cast<WebViewMenuItem>(new MenuItem(name, key, handler));
}

void deleteMenuItem(WebViewMenuItem item) {
    delete static_cast<MenuItem *>(item);
    item = nullptr;
}

WebViewMenu newMenu(const char *name) {
    return static_cast<WebViewMenu>(new Menu(name));
}

void deleteMenu(WebViewMenu menu) {
    delete static_cast<Menu *>(menu);
    menu = nullptr;
}

void menuAddItem(WebViewMenu menu, WebViewMenuItem item) {
    static_cast<Menu *>(menu)->addItem((*static_cast<MenuItem *>(item)));
}

WebViewApplication newApplication() {
    return static_cast<WebViewApplication>(new Application);
}

void deleteApplication(WebViewApplication app) {
    delete static_cast<Application *>(app);
    app = nullptr;
}

void applicationAddMenu(WebViewApplication app, WebViewMenu menu) {
    static_cast<Application *>(app)->addMenu(*static_cast<Menu *>(menu));
}

void applicationRun(WebViewApplication app) {
    static_cast<Application *>(app)->run();
}

void applicationQuit(WebViewApplication app) {
    static_cast<Application *>(app)->quit();
}

WebViewWindow newWindow(const char *title, int width, int height, enum CWindowStyle style) {
    return static_cast<WebViewWindow>(new Window(title, width, height, (WindowStyle) style));
}

void deleteWindow(WebViewWindow window) {
    delete static_cast<Window *>(window);
    window = nullptr;
}

void windowSetTitle(WebViewWindow window, const char *title) {
    static_cast<Window *>(window)->setTitle(title);
}

void windowSetSize(WebViewWindow window, int width, int height) {
    static_cast<Window *>(window)->setSize(width, height);
}

void windowSetDeveloperToolsEnabled(WebViewWindow window, bool enabled) {
    static_cast<Window *>(window)->setDeveloperToolsEnabled(enabled);
}

void windowLoadHTMLString(WebViewWindow window, const char *html) {
    static_cast<Window *>(window)->loadHTMLString(html);
}

void windowLoadURL(WebViewWindow window, const char *url) {
    static_cast<Window *>(window)->loadURL(url);
}

void windowReload(WebViewWindow window) {
    static_cast<Window *>(window)->reload();
}

void windowEval(WebViewWindow window, const char *javaScript) {
    static_cast<Window *>(window)->eval(javaScript);
}

namespace {
    template<typename ...T>
    struct overload : T ... {
        using T::operator()...;
    };
    template<typename ...T>
    overload(T...) -> overload<T...>;
}

void windowAddHandler(WebViewWindow window, const char *name, CHandlerFunc handler) {
    static_cast<Window *>(window)->addHandler(name, [=](Window window, const HandlerInfo &info) {
        void *result;
        std::visit(overload{
                [&](const std::string &str) { result = (void *) str.c_str(); },
                [&](int i) { result = (void *) &i; },
                [&](auto a) { result = nullptr; }
        }, info.result);
        handler(static_cast<WebViewWindow>(&window), CHandlerInfo{info.name.c_str(), result});
    });
}

void windowSetCloseHandler(WebViewWindow window, CloseHandler handler) {
    static_cast<Window *>(window)->setCloseHandler(handler);
}

void windowHide(WebViewWindow window) {
    static_cast<Window *>(window)->hide();
}

void windowShow(WebViewWindow window) {
    static_cast<Window *>(window)->show();
}

void windowMinimize(WebViewWindow window) {
    static_cast<Window *>(window)->minimize();
}

void windowClose(WebViewWindow window) {
    static_cast<Window *>(window)->close();
}
