//
// Created by Riley Quinn on 2019-07-18.
//

#include <string>
#include <map>
#include "webview.hpp"

GtkApplication *Application::app = nullptr;

struct HandlerInfo {
    const char *name;
    HandlerFunc handler;
    Window *window;
};

std::map<std::string, HandlerInfo> handlers;

Application::Application() {
    gtk_init_check(nullptr, nullptr);
    if (!app) app = gtk_application_new(nullptr, G_APPLICATION_FLAGS_NONE);
    GActionEntry actions[] = {"quit",
                              +[](GSimpleAction *, GVariant *, void *app) { g_application_quit(G_APPLICATION(app)); },
                              nullptr, nullptr,
                              nullptr};
    g_action_map_add_action_entries(G_ACTION_MAP(app), actions, 1, app);
}

Application::~Application() {
    g_object_unref(app);
}

void Application::run() {
    g_application_run(G_APPLICATION(app), 0, nullptr);
}

Window::Window(const char *title, int width, int height, WindowStyle style) {
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_window_set_title(GTK_WINDOW(window), title);
    gtk_window_set_default_size(GTK_WINDOW(window), width, height);
    gtk_window_set_resizable(GTK_WINDOW(window), (unsigned int) style & (unsigned int) WindowStyle::Resizable);
    gtk_window_set_decorated(GTK_WINDOW(window), !((unsigned int) style & (unsigned int) WindowStyle::Borderless));
    gtk_window_set_deletable(GTK_WINDOW(window), (unsigned int) style & (unsigned int) WindowStyle::Closable);

    webView = WEBKIT_WEB_VIEW(webkit_web_view_new());

    gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(webView));

    gtk_widget_show_all(window);

    g_signal_connect(g_application_get_default(), "activate",
                     (GCallback) + [](GtkApplication *app, gpointer user_data) {
                         gtk_application_add_window(app, (GtkWindow *) user_data);
                     }, window);
}

void Window::setTitle(const char *title) {
    gtk_window_set_title(GTK_WINDOW(window), title);
}

void Window::setSize(int width, int height) {
    gtk_window_set_default_size(GTK_WINDOW(window), width, height);
}

void Window::loadHTMLString(const char *html) {
    webkit_web_view_load_html(webView, html, nullptr);
}

void Window::loadURL(const char *url) {
    webkit_web_view_load_uri(webView, url);
}

void Window::reload() {
    webkit_web_view_reload(webView);
}

void Window::eval(const char *javaScript) {
    WebKitUserScript *script = webkit_user_script_new(javaScript, WEBKIT_USER_CONTENT_INJECT_ALL_FRAMES,
                                                      WEBKIT_USER_SCRIPT_INJECT_AT_DOCUMENT_END,
                                                      nullptr, nullptr);
    webkit_user_content_manager_add_script(webkit_web_view_get_user_content_manager(webView), script);
}

void Window::addHandler(const char *name, HandlerFunc handler) {
    WebKitUserContentManager *manager = webkit_web_view_get_user_content_manager(webView);
    handlers[name] = HandlerInfo{name, handler, this};
    g_signal_connect(manager, (std::string("script-message-received::") + name).c_str(),
                     (GCallback) + [](WebKitUserContentManager *, WebKitJavascriptResult *result, gpointer user_data) {
                         auto handler = reinterpret_cast<HandlerInfo *>(user_data);
                         JSCValue *value = webkit_javascript_result_get_js_value(result);
                         if (jsc_value_is_string(value)) {
                             JSCException *exception;
                             gchar *str_value;

                             str_value = jsc_value_to_string(value);
                             exception = jsc_context_get_exception(jsc_value_get_context(value));
                             if (exception) {
                                 handler->handler(*handler->window, jsc_exception_get_message(exception));
                             } else {
                                 handler->handler(*handler->window, str_value);
                             }
                             g_free(str_value);
                         } else {
                             handler->handler(*handler->window, "Value is not a string");
                         }
                         webkit_javascript_result_unref(result);
                     }, &handlers[name]);
    webkit_user_content_manager_register_script_message_handler(manager, name);
}

void Window::orderFront() {
    gtk_widget_grab_focus(window);
}

