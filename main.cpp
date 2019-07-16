#include <iostream>

#include "webview.hpp"

int main() {
    auto app = Application();

    auto url = Window("URL Test", 400, 400);
    url.loadURL("https://example.com");
    url.orderFront();

    auto html = Window("HTML Test", 400, 400);

    html.loadHTMLString(R"(
<form onsubmit="event.preventDefault();window.webkit.messageHandlers.thing.postMessage(this['a'].value);">
<input type="text" name="a">
<input type="button" onclick="" value="submit">
)");

    html.addHandler("thing", [](const char *message) {
        std::cout << "Received message: " << message << std::endl;
    });

    html.eval("window.webkit.messageHandlers.thing.postMessage('Loaded web view');");

    html.orderFront();

    app.run();
}