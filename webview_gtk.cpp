//
// Created by Riley Quinn on 2019-07-18.
//

#include "webview.hpp"

GtkApplication *Application::app = nullptr;

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

void Window::orderFront() {
    gtk_widget_grab_focus(window);
}
