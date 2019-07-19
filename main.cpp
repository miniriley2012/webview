#include <iostream>

#include "webview.hpp"

int main() {
    auto app = Application();

    Window("Test", 400, 400);
    /* Not implemented in GTK yet
    window.loadURL("https://example.com");

    auto html = Window("HTML Test", 400, 400);

    html.loadHTMLString(R"(
<form onsubmit="event.preventDefault();window.webkit.messageHandlers.title.postMessage(this['t'].value);">
  <input type="text" name="t" placeholder="Set window title">
  <input type="submit">
</form>
)");

    html.addHandler("title", [](Window window, const char *message) {
        std::cout << "Setting title to: " << message << std::endl;
        window.setTitle(message);
    });

    html.eval("window.webkit.messageHandlers.title.postMessage('My Window');");
    */
    app.run();
}