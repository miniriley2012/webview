#include <iostream>
#include "webview.hpp"

int main() {
    auto app = Application();

    auto menu = Menu("Thing");
    menu.addItem(MenuItem{"Another thing", "o", +[]() {
        std::cout << "Menubar Test!" << std::endl;
    }});
    app.addMenu(menu);

    Window window("Test", 400, 400);
    window.loadURL("https://example.com");

    auto html = Window("HTML Test", 400, 400);

    html.loadHTMLString(R"(
<form onsubmit="event.preventDefault();window.webkit.messageHandlers.title.postMessage(this['t'].value);">
  <input type="text" name="t" placeholder="Set window title">
  <input type="submit">
</form>
)");

    html.addHandler("title", [](Window window, HandlerInfo info) {
        std::cout << "Setting title to: " << info.result << std::endl;
        window.setTitle(info.result);
    });

    html.eval("window.webkit.messageHandlers.title.postMessage('My Window');");
    app.run();
}