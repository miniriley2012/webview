//
// Created by Riley Quinn on 2019-07-15.
//

#include "webview.hpp"

#include <cmath>
#include <CoreGraphics/CoreGraphics.h>

Class handlerClass;
id windowDelegate;
std::map<std::string, MenuBarHandler> menuBarHandlers;
std::map<std::string, HandlerFunc> JSHandlers;

enum JSType {
    Number,
    String,
    Date,
    Array,
    Dictionary,
    Null
};

/// Helper operator
/// @return Class from string literal
id operator "" _cls(const char *s, std::size_t) {
    return (id) objc_getClass(s);
}

/// Helper operator
/// @return selector from string literal
SEL operator "" _sel(const char *s, std::size_t) {
    return sel_registerName(s);
}

/// Helper function
/// @returns string as NSString
id asNSString(const char *str) {
    return (id) objc_msgSend("NSString"_cls, "stringWithUTF8String:"_sel, str);
}

/// Helper operator
/// @returns NSString from string literal
id operator "" _str(const char *s, std::size_t) {
    return asNSString(s);
}

void addUserScript(id webView, const char *javaScript) {
    // Create WKUserScript
    id script = objc_msgSend("WKUserScript"_cls, "alloc"_sel);
    objc_msgSend(script, "initWithSource:injectionTime:forMainFrameOnly:"_sel, asNSString(javaScript), 0, false);
    objc_msgSend(script, "autorelease"_sel);

    // Add Script to web view
    objc_msgSend(objc_msgSend(objc_msgSend(webView, "configuration"_sel), "userContentController"_sel),
                 "addUserScript:"_sel, script);
}

Window::Window(const char *title, int width, int height, WindowStyle style) {
    // Create NSWindow
    window = objc_msgSend("NSWindow"_cls, "alloc"_sel);
    objc_msgSend(window, "initWithContentRect:styleMask:backing:defer:"_sel, CGRectMake(0, 0, width, height),
                 style, 2, 0);
    objc_msgSend(window, "autorelease"_sel);

    objc_setAssociatedObject(window, "window", reinterpret_cast<id>(this), OBJC_ASSOCIATION_ASSIGN);

    objc_msgSend(window, "setDelegate:"_sel, windowDelegate);

    // Set Window title
    objc_msgSend(window, "setTitle:"_sel, asNSString(title));

    objc_msgSend(window, "cascadeTopLeftFromPoint:"_sel, CGPointMake(1, 0));

    // Create WKWebViewConfiguration
    id configuration = objc_msgSend("WKWebViewConfiguration"_cls, "alloc"_sel);
    objc_msgSend(configuration, "init"_sel);
    objc_msgSend(configuration, "autorelease"_sel);

    // Create web view
    webView = objc_msgSend("WKWebView"_cls, "alloc"_sel);

    objc_msgSend(webView, "initWithFrame:configuration:"_sel, CGRectMake(0, 0, 0, 0), configuration);
    objc_msgSend(webView, "autorelease"_sel);

    objc_setAssociatedObject(webView, "loadedHTML", (id) &loadedHTML, OBJC_ASSOCIATION_ASSIGN);

    objc_msgSend(webView, "setTranslatesAutoresizingMaskIntoConstraints:"_sel, 0);

    // Add web view to window
    objc_msgSend(objc_msgSend(window, "contentView"_sel), "addSubview:"_sel, webView);

    // Set width anchor of web view to be equal to window
    objc_msgSend(objc_msgSend(objc_msgSend(webView, "widthAnchor"_sel), "constraintEqualToAnchor:"_sel,
                              objc_msgSend(objc_msgSend(window, "contentView"_sel), "widthAnchor"_sel)),
                 "setActive:"_sel, 1);

    // Set height anchor of web view to be equal to window
    objc_msgSend(objc_msgSend(objc_msgSend(webView, "heightAnchor"_sel), "constraintEqualToAnchor:"_sel,
                              objc_msgSend(objc_msgSend(window, "contentView"_sel), "heightAnchor"_sel)),
                 "setActive:"_sel, 1);

    handlerInstance = objc_msgSend((id) handlerClass, "alloc"_sel);
    objc_msgSend(handlerInstance, "initWithWindow:"_sel, this);
    objc_msgSend(handlerInstance, "autorelease"_sel);

    addUserScript(webView, "window.webview = {handlers: window.webkit.messageHandlers}");
}

void Window::orderFront() {
    objc_msgSend(window, "makeKeyAndOrderFront:"_sel, nullptr);
}

void Window::setTitle(const char *title) {
    objc_msgSend(window, "setTitle:"_sel, asNSString(title));
}

void Window::setSize(int width, int height) {
    auto frame = (CGRect *) objc_msgSend(window, "valueForKey:"_sel, "frame"_str);
    frame->size.width = width;
    frame->size.height = height;
    objc_msgSend(window, "setFrame:display:"_sel, *frame, true);
}

void Window::setDeveloperToolsEnabled(bool enabled) {
    objc_msgSend(objc_msgSend(objc_msgSend(webView, "configuration"_sel), "preferences"_sel),
                 "_setDeveloperExtrasEnabled:"_sel, enabled);
}

void Window::loadHTMLString(const char *html) {
    objc_msgSend(webView, "loadHTMLString:baseURL:"_sel, asNSString(html), nullptr);
    loadedHTML = html;
}

void Window::loadURL(const char *url) {
    // Create NSURL
    id requestURL = objc_msgSend("NSURL"_cls, "URLWithString:"_sel, asNSString(url));
    objc_msgSend(requestURL, "autorelease"_sel);

    // Create NSURLRequest with NSURL
    id request = objc_msgSend("NSURLRequest"_cls, "requestWithURL:"_sel, requestURL);
    objc_msgSend(request, "autorelease"_sel);

    // Load request
    objc_msgSend(webView, "loadRequest:"_sel, request);

    if (!loadedHTML.empty()) loadedHTML.clear();
}

void Window::reload() {
    objc_msgSend(webView, "reload"_sel);
    if (!loadedHTML.empty()) loadHTMLString(loadedHTML.c_str());
}

void Window::eval(const char *javaScript) {
    objc_msgSend(webView, "evaluateJavaScript:completionHandler:"_sel, asNSString(javaScript), nullptr);
}

void Window::addHandler(const char *name, HandlerFunc handler) {
    // Add Handler
    JSHandlers[name] = std::move(handler);

    // Add handler to web view
    objc_msgSend(objc_msgSend(objc_msgSend(webView, "configuration"_sel), "userContentController"_sel),
                 "addScriptMessageHandler:name:"_sel, handlerInstance, asNSString(name));
}

void Window::setCloseHandler(const WindowCloseHandler &handler) {
    closeHandler = handler;
}

void Window::hide() {
    objc_msgSend(window, "orderOut:"_sel, nullptr);
}

void Window::show() {
    if (objc_msgSend(window, "isMiniaturized"_sel)) {
        objc_msgSend(window, "deminiaturize:"_sel, nullptr);
    }
    orderFront();
}

void Window::minimize() {
    objc_msgSend(window, "miniaturize:"_sel, nullptr);
}

void Window::close() {
    objc_msgSend(window, "close"_sel);
}

JSType getJSType(const std::string &name) {
    if (name == "__NSCFNumber") {
        return JSType::Number;
    } else if (name == "NSTaggedPointerString" || name == "__NSCFConstantString" || name == "__NSCFString") {
        return JSType::String;
    } else if (name == "__NSTaggedDate") {
        return JSType::Date;
    } else if (name == "__NSArrayI") {
        return JSType::Array;
    } else if (name == "__NSFrozenDictionaryM") {
        return JSType::Dictionary;
    }
    return JSType::Null;
}

std::vector<_JSPrimitiveResult> array_to_vector(id array) {
    std::vector<_JSPrimitiveResult> array_vector;
    int array_size = (int) (uintptr_t) objc_msgSend(array, "count"_sel);
    array_vector.reserve(array_size);
    for (int i = 0; i < array_size; i++) {
        array_vector.emplace_back(
                (int) (intptr_t) objc_msgSend(objc_msgSend(array, "objectAtIndex:"_sel, i),
                                              "intValue"_sel));
    }
    return array_vector;
}

std::map<std::string, _JSPrimitiveResult> dictionary_to_map(id dict) {
    std::map<std::string, _JSPrimitiveResult> dictionary_map;
    id keys = objc_msgSend(dict, "allKeys"_sel);
    int size = (int) (uintptr_t) objc_msgSend(keys, "count"_sel);
    for (int i = 0; i < size; i++) {
        id obj = objc_msgSend(dict, "objectForKey:"_sel, objc_msgSend(keys, "objectAtIndex:"_sel, i));
        dictionary_map[std::to_string(i)] = (int) (intptr_t) objc_msgSend(obj, "intValue"_sel);
    }
    return dictionary_map;
}

id createMenu(id menubar, id title) {
    id menuItem = objc_msgSend("NSMenuItem"_cls, "new"_sel);
    objc_msgSend(menuItem, "autorelease"_sel);
    objc_msgSend(menubar, "addItem:"_sel, menuItem);
    id menu = objc_msgSend(objc_msgSend("NSMenu"_cls, "alloc"_sel), "initWithTitle:"_sel, title);
    objc_msgSend(menuItem, "autorelease"_sel);
    objc_msgSend(menuItem, "setSubmenu:"_sel, menu);
    return menu;
}

id addMenuItem(id menu, id title, SEL action, id keyEquivalent) {
    return objc_msgSend(menu, "addItemWithTitle:action:keyEquivalent:"_sel, title, action, keyEquivalent);
}

void addDefaultMenus(id menubar) {
    Method method = class_getClassMethod((Class) "NSMenuItem"_cls, "separatorItem"_sel);

    id appName = objc_msgSend(objc_msgSend("NSProcessInfo"_cls, "processInfo"_sel), "processName"_sel);
    id quitTitle = objc_msgSend("Quit "_str, "stringByAppendingString:"_sel, appName);
    id hideTitle = objc_msgSend("Hide "_str, "stringByAppendingString:"_sel, appName);
    id appMenu = createMenu(menubar, ""_str);
    addMenuItem(appMenu, hideTitle, "hide:"_sel, "h"_str);
    objc_msgSend(appMenu, "addItem:"_sel, method_getImplementation(method)("NSMenuItem"_cls, "separatorItem"_sel));
    addMenuItem(appMenu, quitTitle, "terminate:"_sel, "q"_str);

    addMenuItem(createMenu(menubar, "File"_str), "Close Window"_str, "closeWindow"_sel, "w"_str);

    id editMenu = createMenu(menubar, "Edit"_str);
    addMenuItem(editMenu, "Undo"_str, "undo:"_sel, "z"_str);
    addMenuItem(editMenu, "Redo"_str, "redo:"_sel, "Z"_str);
    objc_msgSend(editMenu, "addItem:"_sel, method_getImplementation(method)("NSMenuItem"_cls, "separatorItem"_sel));
    addMenuItem(editMenu, "Cut"_str, "cut:"_sel, "x"_str);
    addMenuItem(editMenu, "Copy"_str, "copy:"_sel, "c"_str);
    addMenuItem(editMenu, "Paste"_str, "paste:"_sel, "v"_str);
    objc_msgSend(addMenuItem(editMenu, "Paste and Match Style"_str, "redo:"_sel, ""_str),
                 "setKeyEquivalentModifierMask:"_sel, 1u << 19u); // 1u << 19u is alt
    addMenuItem(editMenu, "Delete"_str, "delete:"_sel, "Z"_str);
    addMenuItem(editMenu, "Select All"_str, "selectAll:"_sel, "a"_str);

    id viewMenu = createMenu(menubar, "View"_str);
    addMenuItem(viewMenu, "Reload Page"_str, "reloadPage"_sel, "r"_str);
    objc_msgSend(addMenuItem(viewMenu, "Enter Full Screen"_str, "toggleFullScreen:"_sel, "f"_str),
                 "setKeyEquivalentModifierMask:"_sel, 1u << 18u | 1u << 20u); // 18 is control, 20 is command

    addMenuItem(createMenu(menubar, "Window"_str), "Minimize"_str, "performMiniaturize:"_sel, "m"_str);
}

Application::Application() {
    // Create Autorelease pool
    objc_msgSend("NSAutoreleasePool"_cls, "new"_sel);
    app = objc_msgSend("NSApplication"_cls, "sharedApplication"_sel);

    // Create Menu
    menubar = objc_msgSend("NSMenu"_cls, "new"_sel);
    objc_msgSend(menubar, "autorelease"_sel);
    objc_msgSend(app, "setMainMenu:"_sel, menubar);

    addDefaultMenus(menubar);

    // Create AppDelegate
    Class appDelegateClass = objc_allocateClassPair((Class) "NSObject"_cls, "AppDelegate", 0);

    class_addMethod(appDelegateClass, "applicationShouldTerminateAfterLastWindowClosed:"_sel,
                    (IMP) +[]() { return true; }, "c@:@");
    class_addMethod(appDelegateClass, "applicationShouldTerminate:"_sel, (IMP) +[](id, SEL, id ns_app) {
        objc_msgSend(ns_app, "stop:"_sel, nullptr);
        return false;
    }, "@@:@");
    class_addMethod(appDelegateClass, "applicationWillFinishLaunching:"_sel, (IMP) +[](id self) {
        objc_msgSend(objc_msgSend("NSApplication"_cls, "sharedApplication"_sel), "setActivationPolicy:"_sel, 0);
    }, "@@:@");
    class_addMethod(appDelegateClass, "applicationDidFinishLaunching:"_sel, (IMP) +[]() {
        objc_msgSend(objc_msgSend("NSApplication"_cls, "sharedApplication"_sel), "activateIgnoringOtherApps:"_sel, YES);
    }, "@@:@");
    class_addMethod(appDelegateClass, "closeWindow"_sel, (IMP) +[]() {
        id window = objc_msgSend(objc_msgSend("NSApplication"_cls, "sharedApplication"_sel), "mainWindow"_sel);
        objc_msgSend(window, "performClose:"_sel, window);
    }, "v@:@");
    class_addMethod(appDelegateClass, "reloadPage"_sel, (IMP) +[] {
        id subview = objc_msgSend(objc_msgSend(
                objc_msgSend(objc_msgSend(objc_msgSend("NSApplication"_cls, "sharedApplication"_sel), "mainWindow"_sel),
                             "contentView"_sel), "subviews"_sel), "firstObject"_sel);
        if (subview) {
            objc_msgSend(subview, "reload"_sel);
            std::string loadedHTML = *reinterpret_cast<std::string *>(objc_getAssociatedObject(subview, "loadedHTML"));
            objc_msgSend(subview, "loadHTMLString:baseURL:"_sel, asNSString(loadedHTML.c_str()), nullptr);
        }
    }, "v@");

    objc_registerClassPair(appDelegateClass);

    appDelegate = objc_msgSend("AppDelegate"_cls, "new"_sel);
    objc_msgSend(appDelegate, "autorelease"_sel);

    objc_msgSend(app, "setDelegate:"_sel, appDelegate);

    Class windowDelegateClass = objc_allocateClassPair((Class) "NSObject"_cls, "WindowDelegate", 0);

    class_addMethod(windowDelegateClass, "windowShouldClose:"_sel, (IMP) +[](id self, id, id sender) {
        auto window = (Window *) objc_getAssociatedObject(sender, "window");
        return window->closeHandler ? window->closeHandler(window) : true;
    }, "c:@");

    objc_registerClassPair(windowDelegateClass);

    windowDelegate = objc_msgSend(objc_msgSend("WindowDelegate"_cls, "new"_sel), "autorelease"_sel);

    // Create Handler class
    handlerClass = objc_allocateClassPair((Class) "NSObject"_cls, "Handler", 0);
    class_addProtocol(handlerClass, objc_getProtocol("WKScriptMessageHandler"));
    class_addIvar(handlerClass, "handler", sizeof(HandlerFunc), static_cast<unsigned char>(log2(_Alignof(HandlerFunc))),
                  "?");
    class_addIvar(handlerClass, "window", sizeof(Window), static_cast<unsigned char>(log2(_Alignof(Window))), "?");
    class_addMethod(handlerClass, "initWithWindow:"_sel, (IMP) +[](id self, SEL cmd, id window) {
        object_setInstanceVariable(self, "window", window);
    }, "v@:@");
    class_addMethod(handlerClass, "userContentController:didReceiveScriptMessage:"_sel,
                    (IMP) +[](id self, SEL cmd, id userContentController, id message) {
                        Window *window = nullptr;
                        object_getInstanceVariable(self, "window", (void **) &window);
                        id body = objc_msgSend(message, "body"_sel);
                        JSResult result;
                        switch (getJSType(object_getClassName(body))) {
                            case Number:
                                result = static_cast<int>(reinterpret_cast<intptr_t>(objc_msgSend(body,
                                                                                                  "intValue"_sel)));
                                break;
                            case String:
                                result = (const char *) objc_msgSend(body, "UTF8String"_sel);
                                break;
                            case Date:
                                result = (const char *) objc_msgSend(
                                        objc_msgSend("NSString"_cls, "stringFromDate:"_sel, body), "UTF8String"_sel);
                                break;
                            case Array:
                                result = array_to_vector(body);
                                break;
                            case Dictionary:
                                result = dictionary_to_map(body);
                                break;
                            case Null:
                                result = nullptr;
                                break;
                        }
                        std::string name = (const char *) objc_msgSend(objc_msgSend(message, "name"_sel),
                                                                       "UTF8String"_sel);
                        JSHandlers[name](*window, HandlerInfo{name, result});
                    }, "v@:@@");
}

Application::~Application() = default;

void Application::addMenu(Menu &menu) {
    id menuItem = createMenu(menubar, asNSString(menu.name.c_str()));
    for (const auto &item : menu.items) {
        menuBarHandlers[item.name] = item.handler;
        class_addMethod(object_getClass(appDelegate), sel_registerName(item.name.c_str()), (IMP) +[](id self, SEL cmd) {
            menuBarHandlers[std::string(sel_getName(cmd))]();
        }, "v@:@");
        addMenuItem(menuItem, asNSString(item.name.c_str()), sel_registerName(item.name.c_str()), asNSString(item.key));
    }

}

void Application::run() {
    objc_msgSend(app, "run"_sel);
}

void Application::quit() {
    objc_msgSend(app, "terminate:"_sel, nullptr);
}