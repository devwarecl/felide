
#ifndef __FELIDE_UI_QT5_UTILITIESQT_HPP__
#define __FELIDE_UI_QT5_UTILITIESQT_HPP__

#include <QMenuBar>
#include <felide/ui/Menu.hpp>

namespace felide {
    extern QKeySequence mapShortcut(const Shortcut &shortcut);

    extern void setupMenu(QMenu *parentMenuPtr, const Menu &menu);

    extern QMenuBar* createMenuBar(QWidget *parent, const Menu &menuBar);
}

#endif