#ifndef WEBVIEW_WEBVIEW_HPP
#define WEBVIEW_WEBVIEW_HPP

#include <variant>
#include <map>
#include <functional>
#include <memory>

#include "MenuBar.hpp"

#ifdef __APPLE__
#ifdef __OBJC__
#include <WebKit/WebKit.h>

#define WindowType NSWindow*
#define WebViewType WKWebView*
#define ApplicationType NSApplication*
#define MenuBarType NSMenu*

#else

#define OBJC_OLD_DISPATCH_PROTOTYPES 1

#include <objc/objc-runtime.h>

#define WindowType id
#define WebViewType id
#define ApplicationType id
#define MenuBarType id

#endif
#elif __linux__
#include <gtk/gtk.h>
#include <webkitgtk-4.0/webkit2/webkit2.h>

#define WindowType GtkWidget*
#define WebViewType WebKitWebView*
#define ApplicationType static GtkApplication*
#define MenuBarType static GMenu*
#endif

// Predeclared Window class for HandlerFunc
class Window;

typedef std::variant<std::string, int, nullptr_t> _JSPrimitiveResult;
typedef std::variant<std::string, int, std::vector<_JSPrimitiveResult>, std::map<std::string, _JSPrimitiveResult>, nullptr_t> JSResult;

/// Contains info about a handler
struct HandlerInfo {
    std::string name;
    JSResult result;
};

/// HandlerFunc for Window
typedef std::function<void(Window *, HandlerInfo)> HandlerFunc;

/// CloseHandler for Window
typedef std::function<bool(Window *)> WindowCloseHandler;

/// Defines WindowStyles to be used for Window decorations
enum class WindowStyle : unsigned int {
    Borderless = 0,
    Titled = 1u << 0u,
    Closable = 1u << 1u,
    Miniaturizable = 1u << 2u,
    Resizable = 1u << 3u,
    Default = Titled | Closable | Miniaturizable | Resizable
};

/// Application type
class Application {
#ifdef __APPLE__

    id appDelegate;

#endif
public:
    ApplicationType nativeApp;
    MenuBarType nativeMenubar;

    /// Creates a new Application
    Application();

    ~Application();

    /// Creates a Window and returns an std::unique_ptr to it.
    /// \param title title of the window
    /// \param width width of the window
    /// \param height height of the window
    /// \param style window functionality
    /// \return a pointer to the window
    std::unique_ptr<Window>
    newWindow(const char *title, int width, int height, WindowStyle style = WindowStyle::Default);

    /// Adds a Menu to the application's menubar
    /// \param menu menu to add
    void addMenu(Menu &menu);

    /// Run application
    void run();

    /// Quit application
    void quit();
};

/// Window type
class Window {
    friend class Application;

    WebViewType webView;

    std::string loadedHTML;

    Application *app;

#ifdef __APPLE__
    id handlerInstance;
#endif

    /// Orders a window to the front of the screen and grabs focus
    void orderFront();

    /// Creates a new Window
    /// \param title title of the window
    /// \param width width of the window
    /// \param height height of the window
    /// \param style window functionality
    Window(const char *title, int width, int height, WindowStyle style = WindowStyle::Default);

public:
    WindowType nativeWindow;

    WindowCloseHandler closeHandler;

    /// Sets the title of the window
    /// \param title title
    void setTitle(const char *title);

    /// Sets the size of the window
    /// \param width width
    /// \param height height
    void setSize(int width, int height);

    /// Enable developer tools
    /// \param enabled enabled
    void setDeveloperToolsEnabled(bool enabled);

    /// Loads an HTML string into the web view
    /// \param html string to load
    void loadHTMLString(const char *html);

    /// Loads the page at url into the web view
    /// \param url URL to load
    void loadURL(const char *url);

    /// Reloads the web view
    void reload();

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
    /// \param handler handler
    void addHandler(const char *name, HandlerFunc handler);

    /// Sets handler to be run when the window is closed. The handler returns a boolean determining if the window should be
    /// closed.
    /// \code
    /// // hides the window instead of closing it
    /// window.setCloseHandler([](w Window*) {
    ///     w.hide();
    ///     w.close();
    /// }
    /// \endcode
    /// \note The close handler is not run when Window::close is called in the Cocoa port.
    /// \param window window
    /// \param handler handler
    void setCloseHandler(const WindowCloseHandler &);

    /// Hides the window
    void hide();

    /// Orders a window to the front of the screen and grabs focus.
    /// If the window is minimized or hidden it will be shown.
    void show();

    /// Minimizes the window
    void minimize();

    /// Closes the window
    void close();
};

#endif //WEBVIEW_WEBVIEW_HPP