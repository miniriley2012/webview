#include "webview.hpp"

@interface AppDelegate : NSObject <NSApplicationDelegate>
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender;

- (void)closeWindow;

- (void)reloadPage;
@end

@implementation AppDelegate
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {
    return YES;
}

- (void)closeWindow {
    [[[NSApplication sharedApplication] mainWindow] close];
}

- (void)reloadPage {
    __kindof WKWebView *subview = [[[[[NSApplication sharedApplication] mainWindow] contentView] subviews] firstObject];
    if (subview) {
        [subview reload];
    }
}
@end

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
    [window makeKeyAndOrderFront:nil];
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

void Window::reload() {
    [webView reload];
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
    [app setMainMenu:menubar];

    addDefaultMenus();

    appDelegate = [[AppDelegate new] autorelease];
    [app setDelegate:appDelegate];
}

void Application::addDefaultMenus() {
    id appMenuItem = [[NSMenuItem new] autorelease];
    [menubar addItem:appMenuItem];
    id appMenu = [[NSMenu new] autorelease];
    id appName = [[NSProcessInfo processInfo] processName];
    id hideMenuItem = [[[NSMenuItem alloc] initWithTitle:[@"Hide " stringByAppendingString:appName] action:@selector(hide:) keyEquivalent:@"h"] autorelease];
    [appMenu addItem:hideMenuItem];
    [appMenu addItem:NSMenuItem.separatorItem];
    id quitTitle = [@"Quit " stringByAppendingString:appName];
    id quitMenuItem = [[[NSMenuItem alloc] initWithTitle:quitTitle
                                                  action:@selector(terminate:) keyEquivalent:@"q"] autorelease];
    [appMenu addItem:quitMenuItem];
    [appMenuItem setSubmenu:appMenu];

    id fileMenuItem = [[NSMenuItem new] autorelease];
    [menubar addItem:fileMenuItem];
    id fileMenu = [[[NSMenu alloc] initWithTitle:@"File"] autorelease];
    [fileMenu addItemWithTitle:@"Close Window" action:@selector(closeWindow) keyEquivalent:@"w"];
    [fileMenuItem setSubmenu:fileMenu];

    id editMenuItem = [[NSMenuItem new] autorelease];
    [menubar addItem:editMenuItem];
    id editMenu = [[[NSMenu alloc] initWithTitle:@"Edit"] autorelease];
    [editMenu addItemWithTitle:@"Undo" action:@selector(undo:) keyEquivalent:@"z"];
    [editMenu addItemWithTitle:@"Redo" action:@selector(redo:) keyEquivalent:@"Z"];
    [editMenu addItem:NSMenuItem.separatorItem];
    [editMenu addItemWithTitle:@"Cut" action:@selector(cut:) keyEquivalent:@"x"];
    [editMenu addItemWithTitle:@"Copy" action:@selector(copy:) keyEquivalent:@"c"];
    [editMenu addItemWithTitle:@"Paste" action:@selector(paste:) keyEquivalent:@"v"];
    [[editMenu addItemWithTitle:@"Paste and Match Style" action:@selector(pasteAsPlainText:) keyEquivalent:@"V"] setKeyEquivalentModifierMask:NSEventModifierFlagOption];
    [editMenu addItemWithTitle:@"Delete" action:@selector(delete:) keyEquivalent:@""];
    [editMenu addItemWithTitle:@"Select All" action:@selector(selectAll:) keyEquivalent:@"a"];
    [editMenuItem setSubmenu:editMenu];

    id viewMenuItem = [[NSMenuItem new] autorelease];
    [menubar addItem:viewMenuItem];
    id viewMenu = [[[NSMenu alloc] initWithTitle:@"View"] autorelease];
    [viewMenu addItemWithTitle:@"Reload Page" action:@selector(reloadPage) keyEquivalent:@"r"];
    [[viewMenu addItemWithTitle:@"Enter Full Screen" action:@selector(toggleFullScreen:) keyEquivalent:@"f"] setKeyEquivalentModifierMask:NSEventModifierFlagControl];
    [viewMenuItem setSubmenu:viewMenu];

    id windowMenuItem = [[NSMenuItem new] autorelease];
    [menubar addItem:windowMenuItem];
    id windowMenu = [[[NSMenu alloc] initWithTitle:@"Window"] autorelease];
    [windowMenu addItemWithTitle:@"Minimize" action:@selector(performMiniaturize:) keyEquivalent:@"m"];
    [windowMenuItem setSubmenu:windowMenu];
}

void Application::run() {
    [app activateIgnoringOtherApps:YES];
    [app run];
}
