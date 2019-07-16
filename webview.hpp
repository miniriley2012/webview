#ifndef WEBVIEW_WEBVIEW_HPP
#define WEBVIEW_WEBVIEW_HPP

#include <functional>
#include <objc/objc.h>
#include <WebKit/WebKit.h>


typedef void (*HandlerFunc)(const char *);

enum class WindowStyle : unsigned int {
    Titled = NSWindowStyleMaskTitled,
    Closable = NSWindowStyleMaskClosable,
    Miniaturizable = NSWindowStyleMaskMiniaturizable,
    Resizable = NSWindowStyleMaskResizable,
    Default = Titled | Closable | Miniaturizable | Resizable
};

/// Application type for Cocoa Application
class Application {
    id menubar;
public:
    Application();

    /// Run application
    void run();
};

/// Window type for NSWindow
class Window {
    NSWindow *window;
    WKWebView *webView;

public:
    /// Creates a new Window
    /// \param title Title of the window
    /// \param width width of the window
    /// \param height height of the window
    /// \param style window functionality
    Window(const char *title, int width, int height, WindowStyle style = WindowStyle::Default);

    /// Loads an HTML string into the web view
    /// \param html string to load
    void loadHTMLString(const char *html);

    /// Loads the page at url into the web view
    /// \param url URL to load
    void loadURL(const char *url);

    /// Injects JavaScript into the web view
    /// \param javaScript string to inject
    void eval(const char *javaScript);

    /// Adds a handler as a JS function to allow interaction with native code
    /// \code
    /// window.addHandler("thing", [](const char *message) {
    ///     std::cout << "Received message: " << message << std::endl;
    /// });
    /// // Call in JS with: window.webkit.messageHandlers.thing.postMessage('Hello from JavaScript');
    /// \endcode
    /// \param name name of handler
    /// \param handler function of type void (const char*)
    void addHandler(const char *name, HandlerFunc handler);

    /// Orders a Window to the front
    void orderFront();
};

#endif //WEBVIEW_WEBVIEW_HPP