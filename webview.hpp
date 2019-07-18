#ifndef WEBVIEW_WEBVIEW_HPP
#define WEBVIEW_WEBVIEW_HPP

#include <vector>
#include <objc/objc-runtime.h>

#ifdef __OBJC__
#include <WebKit/WebKit.h>

#define WindowType NSWindow*
#define WebViewType WKWebView*

#else

#define WindowType id
#define WebViewType id

#endif

class Window;

typedef void (*HandlerFunc)(Window, const char *);

enum class WindowStyle : unsigned int {
    Borderless = 0,
    Titled = 1u << 0u,
    Closable = 1u << 1u,
    Miniaturizable = 1u << 2u,
    Resizable = 1u << 3u,
    Default = Titled | Closable | Miniaturizable | Resizable
};

/// Application type for Cocoa Application
class Application {
    id app;
    id appDelegate;
    id menubar;

    void addDefaultMenus();
public:
    Application();

    /// Run application
    void run();
};

/// Window type for NSWindow
class Window {
    WindowType window;
    WebViewType webView;

public:
    Window() = default;

    /// Creates a new Window
    /// \param title title of the window
    /// \param width width of the window
    /// \param height height of the window
    /// \param style window functionality
    Window(const char *title, int width, int height, WindowStyle style = WindowStyle::Default);

    /// Sets the title of the window
    /// \param title title
    void setTitle(const char *title);

    /// Sets the size of the window
    /// \param width width
    /// \param height height
    void setSize(int width, int height);

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