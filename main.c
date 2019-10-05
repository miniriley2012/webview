//
// Created by Riley Quinn on 2019-07-24.
//

#include <stdio.h>

#include "webview.h"

void handler(WebViewWindow window, struct CHandlerInfo info) {
    printf("Setting title to: %s\n", info.result);
    windowSetTitle(window, info.result);
}

void menuBarHandler() {
    printf("Menubar Test!\n");
}

int main() {
    WebViewApplication app = newApplication();

    WebViewMenu menu = newMenu("Thing");
    menuAddItem(menu, newMenuItem("Another Thing", "o", menuBarHandler));
    applicationAddMenu(app, menu);

    WebViewWindow window = newWindow("Test", 400, 400);
    windowLoadURL(window, "https://example.com");
    windowShow(window);

    WebViewWindow html = newWindow("HTML Test", 400, 400);
    windowLoadHTMLString(html,
                         "<form onsubmit=\"event.preventDefault();window.webkit.messageHandlers.title.postMessage(this['t'].value);\">\n"
                         "  <input type=\"text\" name=\"t\" placeholder=\"Set window title\">\n"
                         "  <input type=\"submit\">\n"
                         "</form>");
    windowAddHandler(html, "title", handler);
    windowEval(html, "window.webkit.messageHandlers.title.postMessage('My Window')");
    windowShow(html);
    applicationRun(app);
}