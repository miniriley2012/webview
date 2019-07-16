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

void runApplication(void *app) {
    ((Application *) app)->run();
}

void *newWindow(const char *title, int width, int height) {
    return (void *) new Window(title, width, height);
}

void loadHTML(void *window, const char *html) {
    ((Window *) window)->loadHTMLString(html);
}

void loadURL(void *window, const char *url) {
    ((Window *) window)->loadURL(url);
}

void orderFront(void *window) {
    ((Window *) window)->orderFront();
}

#ifdef __cplusplus
};
#endif

#endif //WEBVIEW_WEBVIEW_H
