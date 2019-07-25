//
// Created by Riley Quinn on 2019-07-24.
//

#include "webview.h"

int main() {
    WebViewApplication app = newApplication();
    WebViewWindow window = newWindow("Hello", 400, 400);
    windowLoadURL(window, "https://example.com");
    applicationRun(app);
}