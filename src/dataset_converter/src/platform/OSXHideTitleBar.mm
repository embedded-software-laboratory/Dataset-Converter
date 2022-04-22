#include "OSXHideTitleBar.h"
#include <Cocoa/Cocoa.h>

void OsxTitleBarHandler::HideTitleBar(unsigned long long windowId)
{
    auto *nativeView = reinterpret_cast<NSView *>(windowId);
    NSWindow *nativeWindow = [nativeView window];
    // Removes native title bar
    nativeWindow.styleMask = nativeWindow.styleMask | NSWindowStyleMaskFullSizeContentView;
    nativeWindow.titlebarAppearsTransparent = true;
    nativeWindow.titleVisibility = NSWindowTitleHidden;
    nativeView.wantsLayer = true;

    //[nativeWindow setMovableByWindowBackground: true];
    //nativeWindow.opaque = false;
    //nativeWindow.backgroundColor = NSColor.clearColor;
    //nativeView.layerUsesCoreImageFilters = true;

    // Increases the distance between buttons and border by injecting a empty toolbar
    NSToolbar *toolbar = [[NSToolbar alloc] initWithIdentifier:@"Toolbar"];
    toolbar.showsBaselineSeparator = false;
    toolbar.displayMode = NSToolbarDisplayMode::NSToolbarDisplayModeIconOnly;
    [nativeWindow setToolbar:toolbar];
    [toolbar release];

    //NSVisualEffectView *blurView = [[NSVisualEffectView alloc] initWithFrame:NSMakeRect(0,0,800,600)];
    //blurView.blendingMode = N
    //NSVisualEffectBlendingMode::NSVisualEffectBlendingModeBehindWindow;
    //blurView.material = NSVisualEffectMaterial::NSVisualEffectMaterialFullScreenUI;
    //blurView.state = NSVisualEffectState::NSVisualEffectStateActive;
    //[blurView release];
}