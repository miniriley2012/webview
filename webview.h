//
// Created by Riley Quinn on 2019-07-15.
//

#ifndef WEBVIEW_WEBVIEW_H
#define WEBVIEW_WEBVIEW_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void *WebViewApplication;
typedef void *WebViewMenuItem;
typedef void *WebViewMenu;
typedef void *WebViewWindow;

/// A filled CHandlerInfo is passed to handlers when they are executed
struct CHandlerInfo {
    const char *name;
    void *result;
};

/// Window decorations
enum CWindowStyle {
    Borderless = 0u,
    Titled = 1u << 0u,
    Closable = 1u << 1u,
    Miniaturizable = 1u << 2u,
    Resizable = 1u << 3u,
    Default = Titled | Closable | Miniaturizable | Resizable
};

/// Convenience typedef for a handler function
typedef void (*CHandlerFunc)(WebViewWindow, struct CHandlerInfo);

typedef bool (CloseHandler)(WebViewWindow);

/// Creates a new WebViewMenuItem
/// \param name label for MenuItem in the menubar
/// \param key the key bind to trigger the action
/// \param handler handler for menubar action
/// \return WebViewMenuItem
WebViewMenuItem newMenuItem(const char *name, const char *key, void (*handler)());

/// Deletes a MenuItem and calls destructors
/// \param item
void deleteMenuItem(WebViewMenuItem item);

/// Creates a new Menu
/// \param name label for Menu
/// \return WebViewMenu
WebViewMenu newMenu(const char *name);

/// Deletes a menu
/// \param menu menu
void deleteMenu(WebViewMenu menu);

/// Adds a MenuItem to the Menu
/// \param menu menu to add item to
/// \param item item to add
void menuAddItem(WebViewMenu menu, WebViewMenuItem item);

/// Creates a new WebViewApplication
/// \return WebViewApplication
WebViewApplication newApplication();

/// Deletes an Application
/// \param app application
void deleteApplication(WebViewApplication app);

/// Adds a Menu to the application's menubar
/// \param app application
/// \param menu menu to add
void applicationAddMenu(WebViewApplication app, WebViewMenu menu);

/// Creates a new WebViewWindow
/// \param title title of the window
/// \param width width of the window
/// \param height height of the window
/// \param style window decorations
/// \return WebViewWindow
WebViewWindow
applicationNewWindow(WebViewApplication app, const char *title, int width, int height, enum CWindowStyle style);

/// Run application
/// \param app application
void applicationRun(WebViewApplication app);

/// Quit application
/// \param app application
void applicationQuit(WebViewApplication app);

/// Deletes window and calls destructors.
/// \param window window
void deleteWindow(WebViewWindow window);

/// Sets the title of the window
/// \param window window
/// \param title title
void windowSetTitle(WebViewWindow window, const char *title);

/// Sets the size of the window
/// \param window window
/// \param width width
/// \param height height
void windowSetSize(WebViewWindow window, int width, int height);

/// Enables or disables developer tools (ex. inspect element)
/// \param window window
/// \param enabled enabled
void windowSetDeveloperToolsEnabled(WebViewWindow window, bool enabled);

/// Loads an HTML string into the web view
/// \param window window
/// \param html string to load
void windowLoadHTMLString(WebViewWindow window, const char *html);

/// Loads the page at url into the web view
/// \param window window
/// \param url URL to load
void windowLoadURL(WebViewWindow window, const char *url);

/// Reloads the web view
/// \param window window
void windowReload(WebViewWindow window);

/// Injects JavaScript into the web view
/// \param window window
/// \param javaScript string to inject
void windowEval(WebViewWindow window, const char *javaScript);

/// Adds a handler as a JS function to allow interaction with native code
/// \code
/// void handler(WebViewWindow window, struct CHandlerInfo info) {
///   printf("Received message: %s", info.result);
/// }
/// // ...
/// windowAddHandler(window, "thing", handler);
///
/// // Call in JS with: window.webkit.messageHandlers.thing.postMessage('Hello from JavaScript');
/// \endcode
/// \param window window
/// \param name name of handler
/// \param handler handler
void windowAddHandler(WebViewWindow window, const char *name, CHandlerFunc handler);

/// Sets handler to be run when the window is closed. The handler returns a boolean determining if the window should be
/// closed.
/// \param window window
/// \param handler handler
void windowSetCloseHandler(WebViewWindow window, CloseHandler handler);

/// Hides the window
/// \param window window
void windowHide(WebViewWindow window);

/// Orders a window to the front of the screen and grabs focus. If the window is minimized or hidden it will be shown.
/// \param window window
void windowShow(WebViewWindow window);

/// Minimizes the window
/// \param window window
void windowMinimize(WebViewWindow window);

/// Closes the window
/// \param window window
void windowClose(WebViewWindow window);

#ifdef __cplusplus
}
#endif

#endif //WEBVIEW_WEBVIEW_H
