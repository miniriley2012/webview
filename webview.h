//
// Created by Riley Quinn on 2019-07-15.
//

#ifndef WEBVIEW_WEBVIEW_H
#define WEBVIEW_WEBVIEW_H

#include "shared.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void *WebViewApplication;
typedef void *WebViewMenuItem;
typedef void *WebViewMenu;
typedef void *WebViewWindow;

typedef void (*CHandlerFunc)(void *, struct HandlerInfo);

/// Creates a new WebViewMenuItem
/// \param name label for MenuItem in the menubar
/// \param key the key bind to trigger the action
/// \param handler handler for menubar action
/// \return WebViewMenuItem
WebViewMenuItem newMenuItem(const char *name, const char *key, void (*handler)());

/// Creates a new Menu
/// \param name label for Menu
/// \return Menu
WebViewMenu newMenu(const char *name);

/// Adds a MenuItem to the Menu
/// \param menu menu to add item to
/// \param item item to add
void menuAddItem(WebViewMenu menu, WebViewMenuItem item);

/// Creates a new WebViewApplication
WebViewApplication newApplication();

/// adds a Menu to the application's menubar
/// \param app application
/// \param menu menu to add
void applicationAddMenu(WebViewApplication app, WebViewMenu menu);

/// Run application
/// \param app application
void applicationRun(WebViewApplication app);

/// Quit application
/// \param app application
void applicationQuit(WebViewApplication app);

/// Creates a new WebViewWindow
/// \param title title of the window
/// \param width width of the window
/// \param height height of the window
/// \return WebViewWindow
WebViewWindow newWindow(const char *title, int width, int height);

/// Sets the title of the window
/// \param window window
/// \param title title
void windowSetTitle(WebViewWindow window, const char *title);

/// Sets the size of the window
/// \param window window
/// \param width width
/// \param height height
void windowSetSize(WebViewWindow window, int width, int height);

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
/// window.addHandler("thing", [](const char *message) {
///     std::cout << "Received message: " << message << std::endl;
/// });
/// // Call in JS with: window.webkit.messageHandlers.thing.postMessage('Hello from JavaScript');
/// \endcode
/// \param window window
/// \param name name of handler
/// \param handler handler
void windowAddHandler(WebViewWindow window, const char *name, CHandlerFunc handler);

/// Hides the window
void windowHide(WebViewWindow window);

/// Orders a window to the front of the screen and grabs focus. If the window is minimized or hidden it will be shown.
void windowShow(WebViewWindow window);

/// Minimizes the window
void windowMinimize(WebViewWindow window);

/// Closes the window
void windowClose(WebViewWindow window);

#ifdef __cplusplus
}
#endif

#endif //WEBVIEW_WEBVIEW_H
