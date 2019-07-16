#include "webview.hpp"

#define NSApp NSApp

@interface Handler : NSObject <WKScriptMessageHandler>
- (instancetype)initWithHandler:(HandlerFunc)aHandler;
@end

@implementation Handler {
    std::function<void(const char *message)> handler;
}

- (instancetype)initWithHandler:(HandlerFunc)aHandler {
    self = [super init];
    if (self) {
        handler = aHandler;
    }

    return self;
}

- (void)userContentController:(WKUserContentController *)userContentController didReceiveScriptMessage:(WKScriptMessage *)message {
    handler([(NSString *) [message body] UTF8String]);
}

@end

Window::Window(const char *title, int width, int height, WindowStyle style) {
    window = (id) [[[NSWindow alloc]                                                                                                                       initWithContentRect:NSMakeRect(0, 0, width,
                                                                    height) styleMask:static_cast<NSWindowStyleMask>(style) backing:NSBackingStoreBuffered defer:NO] autorelease];
    [window setTitle:@(title)];
    WKWebViewConfiguration *configuration = [[[WKWebViewConfiguration alloc] init] autorelease];

    webView = (id) [[[WKWebView alloc] initWithFrame:[[window contentView] frame] configuration:configuration] autorelease];
    [webView setTranslatesAutoresizingMaskIntoConstraints:NO];
    [[window contentView] addSubview:webView];

    [[webView widthAnchor] constraintEqualToAnchor:[[window contentView] widthAnchor]].active = YES;
    [[webView heightAnchor] constraintEqualToAnchor:[[window contentView] heightAnchor]].active = YES;
}

void Window::loadHTMLString(const char *html) {
    [webView loadHTMLString:@(html) baseURL:nil];
}

void Window::loadURL(const char *url) {
    NSURL *requestURL = [[NSURL URLWithString:@(url)] autorelease];
    NSURLRequest *request = [[NSURLRequest requestWithURL:requestURL] autorelease];
    [webView loadRequest:request];
}

void Window::eval(const char *javaScript) {
    WKUserScript *script = [[[WKUserScript alloc] initWithSource:@(javaScript) injectionTime:WKUserScriptInjectionTime::WKUserScriptInjectionTimeAtDocumentEnd forMainFrameOnly:YES] autorelease];
    [[[webView configuration] userContentController] addUserScript:script];
}

void Window::addHandler(const char *name, HandlerFunc handler) {
    auto handle = [[[Handler alloc] initWithHandler:handler] autorelease];
    [[[webView configuration] userContentController] addScriptMessageHandler:handle name:@(name)];
}

void Window::orderFront() {
    [window makeKeyAndOrderFront:nil];
}

Application::Application() {
    [[NSAutoreleasePool new] autorelease];
    [NSApplication sharedApplication];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

    menubar = [[NSMenu new] autorelease];
    id appMenuItem = [[NSMenuItem new] autorelease];
    [menubar addItem:appMenuItem];
    [NSApp setMainMenu:menubar];
    id appMenu = [[NSMenu new] autorelease];
    id appName = [[NSProcessInfo processInfo] processName];
    id quitTitle = [@"Quit " stringByAppendingString:appName];
    id quitMenuItem = [[[NSMenuItem alloc] initWithTitle:quitTitle
                                                  action:@selector(terminate:) keyEquivalent:@"q"] autorelease];
    [appMenu addItem:quitMenuItem];
    [appMenuItem setSubmenu:appMenu];
}

void Application::run() {
    [NSApp activateIgnoringOtherApps:YES];
    [NSApp run];
}
