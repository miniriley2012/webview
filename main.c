//
// Created by Riley Quinn on 2019-07-24.
//

#include <stdio.h>

#include "webview.h"

void handler(WebViewWindow window, struct HandlerInfo info) {
    printf("Setting title to: %s\n", info.result);
    windowSetTitle(window, info.result);
}

int main() {
    WebViewApplication app = newApplication();
    WebViewWindow window = newWindow("Test", 400, 400);
    windowLoadURL(window, "https://example.com");

    WebViewWindow html = newWindow("HTML Test", 400, 400);
    windowLoadHTMLString(html,
                         "<form onsubmit=\"event.preventDefault();window.webkit.messageHandlers.title.postMessage(this['t'].value);\">\n"
                         "  <input type=\"text\" name=\"t\" placeholder=\"Set window title\">\n"
                         "  <input type=\"submit\">\n"
                         "</form>");
    windowAddHandler(html, "title", handler);
    windowEval(html, "window.webkit.messageHandlers.title.postMessage('My Window')");
    applicationRun(app);
}