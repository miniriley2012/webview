//
// Created by Riley Quinn on 2019-07-15.
//

#ifndef WEBVIEW_WEBVIEW_H
#define WEBVIEW_WEBVIEW_H

#include "webview.hpp"

#ifdef __cplusplus
extern "C" {
#endif

void *newApplication() {
    return new Application;
}

void applicationRun(void *app) {
    ((Application *) app)->run();
}

void *newWindow(const char *title, int width, int height) {
    return (void *) new Window(title, width, height);
}

void windowSetTitle(void *window, const char *title) {
    ((Window *) window)->setTitle(title);
}

void windowSetSize(void *window, int width, int height) {
    ((Window *) window)->setSize(width, height);
}

void windowLoadHTMString(void *window, const char *html) {
    ((Window *) window)->loadHTMLString(html);
}

void windowLoadURL(void *window, const char *url) {
    ((Window *) window)->loadURL(url);
}

void windowReload(void *window) {
    ((Window *) window)->reload();
}

void windowEval(void *window, const char *javaScript) {
    ((Window *) window)->eval(javaScript);
}

void windowAddHandler(void *window, const char *name, HandlerFunc handler) {
    ((Window *) window)->addHandler(name, handler);
}

void windowOrderFront(void *window) {
    ((Window *) window)->orderFront();
}

#ifdef __cplusplus
}
#endif

#endif //WEBVIEW_WEBVIEW_H
