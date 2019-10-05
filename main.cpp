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

    window.show();

    auto html = Window("HTML Test", 400, 400);

    html.loadHTMLString(R"(
<form onsubmit="event.preventDefault();window.webview.handlers.title.postMessage(this['t'].value);">
  <input type="text" name="t" placeholder="Set window title">
  <input type="submit">
</form>
<button onclick="window.webview.handlers.minimize.postMessage('');window.setTimeout(() => {window.webview.handlers.show.postMessage('')}, 3000);">Minimize</button>
<button onclick="window.webview.handlers.hide.postMessage('');window.setTimeout(() => {window.webview.handlers.show.postMessage('')}, 3000);">Hide</button>
<button onclick="window.webview.handlers.quit.postMessage('');">Quit</button>
<button onclick="window.webview.handlers.close.postMessage(new Date());">Close</button>
)");

    html.addHandler("title", [](Window window, const HandlerInfo &info) {
        auto title = std::get<std::string>(info.result);
        std::cout << "Setting title to: " << title << std::endl;
        window.setTitle(title.c_str());
    });

    html.addHandler("minimize", [](Window window, const HandlerInfo &) {
        std::cout << "Minimized" << std::endl;
        window.minimize();
    });

    html.addHandler("hide", [](Window window, const HandlerInfo &) {
        std::cout << "Hidden" << std::endl;
        window.hide();
    });

    html.addHandler("show", [](Window window, const HandlerInfo &) {
        std::cout << "Shown" << std::endl;
        window.show();
    });

    html.addHandler("quit", [&](Window, const HandlerInfo &) {
        app.quit();
    });

    html.addHandler("close", [](Window window, const HandlerInfo &) {
        window.close();
    });

    html.show();

    html.eval("window.webview.messageHandlers.title.postMessage('My Window');");
    app.run();
}