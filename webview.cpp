//
// Created by Riley Quinn on 2019-07-15.
//

#include "webview.hpp"

#include <cmath>
#include <objc/objc-runtime.h>
#include <CoreGraphics/CGGeometry.h>

Class handlerClass;

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

Window::Window(const char *title, int width, int height, WindowStyle style) {
    // Create NSWindow
    window = objc_msgSend("NSWindow"_cls, "alloc"_sel);
    objc_msgSend(window, "initWithContentRect:styleMask:backing:defer:"_sel, CGRectMake(0, 0, width, height),
                 style, 2, 0);
    objc_msgSend(window, "autorelease"_sel);

    // Set Window title
    objc_msgSend(window, "setTitle:"_sel, asNSString(title));

    // Create WKWebViewConfiguration
    id configuration = objc_msgSend("WKWebViewConfiguration"_cls, "alloc"_sel);
    objc_msgSend(configuration, "init"_sel);
    objc_msgSend(configuration, "autorelease"_sel);

    // Create web view
    webView = objc_msgSend("WKWebView"_cls, "alloc"_sel);

    objc_msgSend(webView, "initWithFrame:configuration:"_sel, CGRectMake(0, 0, 0, 0), configuration);
    objc_msgSend(webView, "autorelease"_sel);

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

void Window::loadHTMLString(const char *html) {
    objc_msgSend(webView, "loadHTMLString:baseURL:"_sel, asNSString(html), nullptr);
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
}

void Window::eval(const char *javaScript) {
    // Create WKUserScript
    id script = objc_msgSend("WKUserScript"_cls, "alloc"_sel);
    objc_msgSend(script, "initWithSource:injectionTime:forMainFrameOnly:"_sel, asNSString(javaScript), 1, 1);
    objc_msgSend(script, "autorelease"_sel);

    // Add Script to web view
    objc_msgSend(objc_msgSend(objc_msgSend(webView, "configuration"_sel), "userContentController"_sel),
                 "addUserScript:"_sel, script);
}

void Window::addHandler(const char *name, HandlerFunc handler) {
    // Create Handler
    id handle = objc_msgSend((id) handlerClass, "alloc"_sel);
    objc_msgSend(handle, "initWithHandler:window:"_sel, handler, this);
    objc_msgSend(handle, "autorelease"_sel);

    // Add handler to web view
    objc_msgSend(objc_msgSend(objc_msgSend(webView, "configuration"_sel), "userContentController"_sel),
                 "addScriptMessageHandler:name:"_sel, handle, asNSString(name));
}

void Window::orderFront() {
    objc_msgSend(window, "makeKeyAndOrderFront:"_sel, nullptr);
}

Application::Application() {
    // Create Autorelease pool
    objc_msgSend("NSAutoreleasePool"_cls, "new"_sel);
    app = objc_msgSend("NSApplication"_cls, "sharedApplication"_sel);
    objc_msgSend(app, "setActivationPolicy:"_sel, 0);

    // Create Menu
    menubar = objc_msgSend("NSMenu"_cls, "new"_sel);
    objc_msgSend(menubar, "autorelease"_sel);
    objc_msgSend(app, "setMainMenu:"_sel, menubar);

    addDefaultMenus();

    // Create AppDelegate
    Class appDelegateClass = objc_allocateClassPair((Class) "NSObject"_cls, "AppDelegate", 0);

    class_addMethod(appDelegateClass, "applicationShouldTerminateAfterLastWindowClosed:"_sel,
                    (IMP) +[]() { return true; }, "c@:@");
    class_addMethod(appDelegateClass, "closeWindow"_sel, (IMP) +[]() {
        objc_msgSend(objc_msgSend(objc_msgSend("NSApplication"_cls, "sharedApplication"_sel), "mainWindow"_sel),
                     "close"_sel);
    }, "v@:@");
    class_addMethod(appDelegateClass, "reloadPage"_sel, (IMP) +[] {
        id subview = objc_msgSend(objc_msgSend(
                objc_msgSend(objc_msgSend(objc_msgSend("NSApplication"_cls, "sharedApplication"_sel), "mainWindow"_sel),
                             "contentView"_sel), "subviews"_sel), "firstObject"_sel);
        if (subview) {
            objc_msgSend(subview, "reload"_sel);
        }
    }, "v@");

    objc_registerClassPair(appDelegateClass);

    appDelegate = objc_msgSend("AppDelegate"_cls, "new"_sel);
    objc_msgSend(appDelegate, "autorelease"_sel);

    objc_msgSend(app, "setDelegate:"_sel, appDelegate);

    // Create Handler class
    handlerClass = objc_allocateClassPair((Class) "NSObject"_cls, "Handler", 0);
    class_addProtocol(handlerClass, objc_getProtocol("WKScriptMessageHandler"));
    class_addIvar(handlerClass, "handler", sizeof(HandlerFunc), static_cast<unsigned char>(log2(_Alignof(HandlerFunc))),
                  "?");
    class_addIvar(handlerClass, "window", sizeof(Window), static_cast<unsigned char>(log2(_Alignof(Window))), "?");
    class_addMethod(handlerClass, "initWithHandler:window:"_sel, (IMP) +[](id self, SEL cmd, id handler, id window) {
        object_setInstanceVariable(self, "handler", handler);
        object_setInstanceVariable(self, "window", window);
    }, "v@:@");
    class_addMethod(handlerClass, "userContentController:didReceiveScriptMessage:"_sel,
                    (IMP) +[](id self, SEL cmd, id userContentController, id message) {
                        void *handler, *window;
                        object_getInstanceVariable(self, "handler", &handler);
                        object_getInstanceVariable(self, "window", &window);
                        ((HandlerFunc) handler)(*((Window *) window),
                                                (const char *) objc_msgSend(objc_msgSend(message, "body"_sel),
                                                                            "UTF8String"_sel));
                    }, "v@:@@");
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

void Application::addDefaultMenus() {
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
                 "setKeyEquivalentModifierMask:"_sel, 1u << 18u);

    addMenuItem(createMenu(menubar, "Window"_str), "Minimize"_str, "performMiniaturize:"_sel, "m"_str);
}

void Application::run() {
    objc_msgSend(app, "activateIgnoringOtherApps:"_sel, 1);
    objc_msgSend(app, "run"_sel);
}