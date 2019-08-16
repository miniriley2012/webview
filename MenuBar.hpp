//
// Created by Riley Quinn on 2019-08-15.
//

#ifndef WEBVIEW_MENUBAR_HPP
#define WEBVIEW_MENUBAR_HPP

#include <utility>
#include <vector>
#include <string>

/// Convenience type for Menubar handlers
typedef void (*MenuBarHandler)();

/// MenuItem represents an item in a Menu
struct MenuItem {
    std::string name;
    const char *key;
    MenuBarHandler handler;

    /// Constructs a MenuItem
    /// \param name label for MenuItem in the menubar
    /// \param key the key bind to trigger the action
    /// \param handler handler for menubar action
    MenuItem(std::string name, const char *key, void (*handler)()) : name(std::move(name)), key(key),
                                                                     handler(handler) {}
};

/// Menu represents a menu in the menubar
struct Menu {
    std::string name;
    std::vector<MenuItem> items;

    /// Constructs a Menu
    /// \param name label for Menu
    explicit Menu(std::string name) : name(std::move(name)) {}

    /// Adds a MenuItem to the Menu
    /// \param item item to add
    void addItem(const MenuItem &item) {
        items.push_back(item);
    }
};

#endif //WEBVIEW_MENUBAR_HPP
