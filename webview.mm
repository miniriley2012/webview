#include "webview.hpp"

@interface Handler : NSObject <WKScriptMessageHandler>
- (instancetype)initWithHandler:(HandlerFunc)aHandler window:(Window)aWindow;
@end

@implementation Handler {
    Window window;
    HandlerFunc handler;
}

- (instancetype)initWithHandler:(HandlerFunc)aHandler window:(Window)aWindow {
    self = [super init];
    if (self) {
        handler = aHandler;
        window = aWindow;
    }

    return self;
}

- (void)userContentController:(WKUserContentController *)userContentController didReceiveScriptMessage:(WKScriptMessage *)message {
    handler(window, [(NSString *) [message body] UTF8String]);
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

void Window::setTitle(const char *title) {
    [window setTitle:@(title)];
}

void Window::setSize(int width, int height) {
    auto frame = [window frame];
    frame.size.width = width;
    frame.size.height = height;
    [window setFrame:frame display:true];
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
    auto handle = [[[Handler alloc] initWithHandler:handler window:*this] autorelease];
    [[[webView configuration] userContentController] addScriptMessageHandler:handle name:@(name)];
}

void Window::orderFront() {
    [window makeKeyAndOrderFront:nil];
}

Application::Application() {
    [NSAutoreleasePool new];
    app = [NSApplication sharedApplication];
    [app setActivationPolicy:NSApplicationActivationPolicyRegular];

    menubar = [[NSMenu new] autorelease];
    id appMenuItem = [[NSMenuItem new] autorelease];
    [menubar addItem:appMenuItem];
    [app setMainMenu:menubar];
    id appMenu = [[NSMenu new] autorelease];
    id appName = [[NSProcessInfo processInfo] processName];
    id quitTitle = [@"Quit " stringByAppendingString:appName];
    id quitMenuItem = [[[NSMenuItem alloc] initWithTitle:quitTitle
                                                  action:@selector(terminate:) keyEquivalent:@"q"] autorelease];
    [appMenu addItem:quitMenuItem];
    [appMenuItem setSubmenu:appMenu];
}

void Application::run() {
    [app activateIgnoringOtherApps:YES];
    [app run];
}
