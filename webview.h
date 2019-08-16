//
// Created by Riley Quinn on 2019-07-15.
//

#ifndef WEBVIEW_WEBVIEW_H
#define WEBVIEW_WEBVIEW_H

#include "shared.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void *WebViewApplication;
typedef void *WebViewMenuItem;
typedef void *WebViewMenu;
typedef void *WebViewWindow;

typedef void (*CHandlerFunc)(void *, struct HandlerInfo);

WebViewMenuItem newMenuItem(const char *name, const char *key, void (*handler)());

WebViewMenu newMenu(const char *name);

void menuAddItem(WebViewMenu menu, WebViewMenuItem item);

WebViewApplication newApplication();

void applicationAddMenu(WebViewApplication app, WebViewMenu menu);

void applicationRun(WebViewApplication app);

WebViewWindow newWindow(const char *title, int width, int height);

void windowSetTitle(WebViewWindow window, const char *title);

void windowSetSize(WebViewWindow window, int width, int height);

void windowLoadHTMLString(WebViewWindow window, const char *html);

void windowLoadURL(WebViewWindow window, const char *url);

void windowReload(WebViewWindow window);

void windowEval(WebViewWindow window, const char *javaScript);

void windowAddHandler(WebViewWindow window, const char *name, CHandlerFunc handler);

void windowOrderFront(WebViewWindow window);

#ifdef __cplusplus
}
#endif

#endif //WEBVIEW_WEBVIEW_H
