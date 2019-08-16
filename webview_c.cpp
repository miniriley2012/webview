//
// Created by Riley Quinn on 2019-07-24.
//

#include <map>
#include "webview.h"
#include "webview.hpp"

// TODO Find some better way to do this
std::map<std::string, CHandlerFunc> handlers;

WebViewMenuItem newMenuItem(const char *name, const char *key, void (*handler)()) {
    return static_cast<WebViewMenuItem>(new MenuItem(name, key, handler));
}

WebViewMenu newMenu(const char *name) {
    return static_cast<WebViewMenu>(new Menu(name));
}

void menuAddItem(WebViewMenu menu, WebViewMenuItem item) {
    static_cast<Menu *>(menu)->addItem((*static_cast<MenuItem *>(item)));
}

WebViewApplication newApplication() {
    return static_cast<WebViewApplication>(new Application);
}

void applicationAddMenu(WebViewApplication app, WebViewMenu menu) {
    static_cast<Application *>(app)->addMenu(*static_cast<Menu *>(menu));
}

void applicationRun(WebViewApplication app) {
    static_cast<Application *>(app)->run();
}

WebViewWindow newWindow(const char *title, int width, int height) {
    return static_cast<WebViewWindow>(new Window(title, width, height));
}

void windowSetTitle(WebViewWindow window, const char *title) {
    static_cast<Window *>(window)->setTitle(title);
}

void windowSetSize(WebViewWindow window, int width, int height) {
    static_cast<Window *>(window)->setSize(width, height);
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

void windowAddHandler(WebViewWindow window, const char *name, CHandlerFunc handler) {
    handlers[std::string(name)] = handler;
    static_cast<Window *>(window)->addHandler(name, +[](Window window, HandlerInfo info) {
        handlers[info.name](static_cast<WebViewWindow>(&window), info);
    });
}

void windowOrderFront(WebViewWindow window) {
    static_cast<Window *>(window)->orderFront();
}
