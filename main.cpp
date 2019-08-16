#include <iostream>
#include "webview.hpp"

namespace {
    Application *app = nullptr;
}

int main() {
    app = new Application;

    auto menu = Menu("Thing");
    menu.addItem(MenuItem{"Another thing", "o", +[]() {
        std::cout << "Menubar Test!" << std::endl;
    }});
    app->addMenu(menu);

    Window window("Test", 400, 400);
    window.loadURL("https://example.com");

    window.show();

    auto html = Window("HTML Test", 400, 400);

    html.loadHTMLString(R"(
<form onsubmit="event.preventDefault();window.webkit.messageHandlers.title.postMessage(this['t'].value);">
  <input type="text" name="t" placeholder="Set window title">
  <input type="submit">
</form>
<button onclick="window.webkit.messageHandlers.minimize.postMessage('');window.setTimeout(() => {window.webkit.messageHandlers.show.postMessage('')}, 3000);">Minimize</button>
<button onclick="window.webkit.messageHandlers.hide.postMessage('');window.setTimeout(() => {window.webkit.messageHandlers.show.postMessage('')}, 3000);">Hide</button>
<button onclick="window.webkit.messageHandlers.quit.postMessage('');">Quit</button>
<button onclick="window.webkit.messageHandlers.close.postMessage('');">Close</button>
)");

    html.addHandler("title", [](Window window, HandlerInfo info) {
        std::cout << "Setting title to: " << info.result << std::endl;
        window.setTitle(info.result);
    });

    html.addHandler("minimize", [](Window window, HandlerInfo info) {
        std::cout << "Minimized" << std::endl;
        window.minimize();
    });

    html.addHandler("hide", [](Window window, HandlerInfo) {
        std::cout << "Hidden" << std::endl;
        window.hide();
    });

    html.addHandler("show", [](Window window, HandlerInfo) {
        std::cout << "Shown" << std::endl;
        window.show();
    });

    html.addHandler("quit", [](Window, HandlerInfo) {
        app->quit();
    });

    html.addHandler("close", [](Window window, HandlerInfo) {
        window.close();
    });

    html.show();

    html.eval("window.webkit.messageHandlers.title.postMessage('My Window');");
    app->run();

    delete app;
}