//
// Created by Riley Quinn on 2019-07-24.
//

#include "webview.h"
#include "webview.hpp"

void *newApplication() {
    return static_cast<void *>(new Application);
}

void applicationRun(void *app) {
    static_cast<Application *>(app)->run();
}

void *newWindow(const char *title, int width, int height) {
    return static_cast<void *>(new Window(title, width, height));
}

void windowSetTitle(void *window, const char *title) {
    static_cast<Window *>(window)->setTitle(title);
}

void windowSetSize(void *window, int width, int height) {
    static_cast<Window *>(window)->setSize(width, height);
}

void windowLoadHTMString(void *window, const char *html) {
    static_cast<Window *>(window)->loadHTMLString(html);
}

void windowLoadURL(void *window, const char *url) {
    static_cast<Window *>(window)->loadURL(url);
}

void windowReload(void *window) {
    static_cast<Window *>(window)->reload();
}

void windowEval(void *window, const char *javaScript) {
    static_cast<Window *>(window)->eval(javaScript);
}

void windowAddHandler(void *window, const char *name, CHandlerFunc handler) {
    static_cast<Window *>(window)->addHandler(name, (HandlerFunc) handler);
}

void windowOrderFront(void *window) {
    static_cast<Window *>(window)->orderFront();
}
