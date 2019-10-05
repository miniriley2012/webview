#import <objc/runtime.h>
#include "webview.hpp"

@interface AppDelegate : NSObject <NSApplicationDelegate>
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender;

- (void)closeWindow;

- (void)reloadPage;

- (void)addHandler:(std::string)name handler:(MenuBarHandler)handler;

- (MenuBarHandler)getHandler:(std::string)name;
@end

@implementation AppDelegate {
    std::map <std::string, MenuBarHandler> handlers;
}
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

- (void)addHandler:(std::string)name handler:(MenuBarHandler)handler {
    handlers[name] = handler;
}

- (MenuBarHandler)getHandler:(std::string)name {
    return handlers[name];
}

@end

enum JSType {
    Number,
    String,
    Date,
    Array,
    Dictionary,
    Null
};

JSType getJSType(const std::string &name) {
    if (name == "__NSCFNumber") {
        return JSType::Number;
    } else if (name == "NSTaggedPointerString" || name == "__NSCFConstantString") {
        return JSType::String;
    } else if (name == "__NSTaggedDate") {
        return JSType::Date;
    } else if (name == "__NSArrayI") {
        return JSType::Array;
    } else if (name == "__NSFrozenDictionaryM") {
        return JSType::Dictionary;
    }
    return JSType::Null;
}

@interface ScriptHandler : NSObject <WKScriptMessageHandler>
- (instancetype)initWithWindow:(Window)aWindow;

- (void)addJSHandler:(std::string)name handler:(HandlerFunc)handler;

@end

std::vector <_JSPrimitiveResult> array_to_vector(id array) {
    std::vector <_JSPrimitiveResult> array_vector;
    array_vector.reserve([array count]);
    for (id object in array) {
        array_vector.emplace_back([object intValue]);
    }
    return array_vector;
}

std::map <std::string, _JSPrimitiveResult> dictionary_to_map(id) {
    std::map <std::string, _JSPrimitiveResult> dictionary_map;
    return dictionary_map;
}

@implementation ScriptHandler {
    Window window;
    std::map <std::string, HandlerFunc> handlers;
}

- (instancetype)initWithWindow:(Window)aWindow {
    self = [super init];
    if (self) {
        window = aWindow;
    }

    return self;
}

- (void)addJSHandler:(std::string)name handler:(HandlerFunc)handler {
    handlers[name] = handler;
}


- (void)userContentController:(WKUserContentController *)userContentController didReceiveScriptMessage:(WKScriptMessage *)message {
//    handler(window, HandlerInfo{[[message name] UTF8String], [(NSString *) [message body] UTF8String]});
    NSLog(@("%s"), "Received Message");
    JSResult result;
    switch (getJSType([[[message body] className] UTF8String])) {
        case Number:
            result = [[message body] intValue];
            break;
        case String:
            result = [[message body] UTF8String];
            break;
        case Date:
            result = [[NSDateFormatter localizedStringFromDate:[message body] dateStyle:NSDateFormatterShortStyle timeStyle:NSDateFormatterFullStyle] UTF8String];
            break;
        case Array:
            result = array_to_vector([message body]);
            break;
        case Dictionary:
            result = dictionary_to_map([message body]);
            break;
        case Null:
            result = nullptr;
            break;
    }
    std::string name = [[message name] UTF8String];
    handlers[name](window, HandlerInfo{name, result});
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

    handlerInstance = [[[ScriptHandler alloc] initWithWindow:*this] autorelease];
}

void Window::orderFront() {
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
//    auto handle = [[[Handler alloc] initWithHandler:handler window:*this] autorelease];

    [handlerInstance addJSHandler:name handler:handler];
    [[[webView configuration] userContentController] addScriptMessageHandler:(ScriptHandler *) handlerInstance name:@(name)];
}

void Window::hide() {
    [window orderOut:nullptr];
}

void Window::show() {
    if ([window isMiniaturized]) {
        [window deminiaturize:nullptr];
    }
    orderFront();
}

void Window::minimize() {
    [window miniaturize:nullptr];
}

void Window::close() {
    [window close];
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

Application::~Application() = default;

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

void Application::addMenu(const Menu &menu) {
    id menuItem = [[NSMenuItem new] autorelease];
    [menubar addItem:menuItem];
    id menuItemMenu = [[[NSMenu alloc] initWithTitle:@(menu.name.c_str())] autorelease];
    for (const auto &item : menu.items) {
        [appDelegate addHandler:item.name handler:item.handler];
        class_addMethod([appDelegate class], NSSelectorFromString(@(item.name.c_str())), (IMP) + [](id self, SEL cmd) {
            [self getHandler:[NSStringFromSelector(cmd) UTF8String]]();
        }, "v@:@");
        [menuItemMenu addItemWithTitle:@(item.name.c_str()) action:NSSelectorFromString(
                @(item.name.c_str()))                       keyEquivalent:@(item.key)];
    }
    [menuItem setSubmenu:menuItemMenu];
}

void Application::run() {
    [app activateIgnoringOtherApps:YES];
    [app run];
}

void Application::quit() {
    [app terminate:nullptr];
}