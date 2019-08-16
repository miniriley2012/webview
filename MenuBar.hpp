//
// Created by Riley Quinn on 2019-08-15.
//

#ifndef WEBVIEW_MENUBAR_HPP
#define WEBVIEW_MENUBAR_HPP

#include <utility>
#include <vector>
#include <string>

typedef void (*MenuBarHandler)();

struct MenuItem {
    std::string name;
    const char *key;
    MenuBarHandler handler;

    MenuItem(std::string name, const char *key, void (*handler)()) : name(std::move(name)), key(key),
                                                                     handler(handler) {}
};

struct Menu {
    std::string name;
    std::vector<MenuItem> items;

    explicit Menu(std::string name) : name(std::move(name)) {}

    void addItem(const MenuItem &item) {
        items.push_back(item);
    }
};

#endif //WEBVIEW_MENUBAR_HPP
