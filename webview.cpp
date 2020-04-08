//
// Created by Riley Quinn on 4/7/20.
//

#include "webview.hpp"

std::unique_ptr<Window> Application::newWindow(const char *title, int width, int height, WindowStyle style) {
    auto ptr = std::unique_ptr<Window>(nullptr);
    ptr.reset(new Window(title, width, height, style));
    ptr->app = this;
    return ptr;
}