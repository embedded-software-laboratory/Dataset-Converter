#ifndef OSXHIDETITLEBAR_H
#define OSXHIDETITLEBAR_H

class OsxTitleBarHandler {
 public:
    /**
     * Function that injects into the MacOs X native rendering process and changes the window behaviour.
     * @param windowId Id of native window to inject.
     */
  static void HideTitleBar(unsigned long long windowId);
};

#endif //OSXHIDETITLEBAR_H
