
#include "MainWindow.hpp"

#include <vector>
#include <functional>
#include <QMessageBox>
#include <QDesktopWidget>

#include "Editor.hpp"
#include "DialogManager.hpp"
#include "../MainWindowPresenter.hpp"

namespace felide {
    static void setupMenu(QMenu *parentMenuPtr, const Menu &menu) {
        switch (menu.kind) {
            case MenuKind::SubMenu: {
                QMenu *menuPtr = parentMenuPtr->addMenu(menu.text.c_str());

                for (const Menu &childMenu : menu.childs) {
                    setupMenu(menuPtr, childMenu);
                }

                break;
            }

            case MenuKind::Action: {
                auto action = parentMenuPtr->addAction(menu.text.c_str(), nullptr, nullptr, QKeySequence(menu.shortcut.c_str()));

                if (menu.callback) {
                    QObject::connect(action, &QAction::triggered, menu.callback);
                }

                break;
            }

            case MenuKind::Separator: {
                parentMenuPtr->addSeparator();
                break;
            }
        }
    }

    static QMenuBar* createMenuBar(QWidget *parent, const Menu &menuBar) {
        auto menuBarPtr = new QMenuBar(parent);

        for (const Menu &menu : menuBar.childs) {
            QMenu *menuPtr = menuBarPtr->addMenu(menu.text.c_str());

            for (const Menu &childMenu : menu.childs) {
                setupMenu(menuPtr, childMenu);
            }
        }

        return menuBarPtr;
    }

    MainWindow::MainWindow() {
        this->setupMenuBar();
        this->setupEditorManager();

        m_dialogManager = std::make_unique<DialogManager>(this);

        QDesktopWidget desktopWidget;
        
        const QRect screenSize = desktopWidget.availableGeometry(desktopWidget.primaryScreen());
        
        this->setMinimumSize(screenSize.size() * 0.4);
        this->resize(screenSize.size() * 0.7);
        
        presenter.attachView(this);
    }

    void MainWindow::setupMenuBar() {
        auto mainMenu = Menu::menuBar({
            Menu::menu("&File", {
                Menu::action([this] () { presenter.fileNewTriggered(); }, "&New", "Ctrl+N"),
                Menu::action([this] () { presenter.fileOpenTriggered(); }, "&Open", "Ctrl+O"),
                Menu::action([this] () { presenter.fileSaveTriggered(); }, "&Save", "Ctrl+S"),
                Menu::action([this] () { presenter.fileSaveAsTriggered(); }, "Sa&ve As"),
                Menu::action([this] () { presenter.fileSaveAllTriggered(); }, "Save &All"),
                Menu::action([this] () { presenter.fileCloseTriggered(); }, "&Close"),
                Menu::action([this] () { presenter.fileExitTriggered(); }, "&Exit", "Alt+F4")
            }),
            Menu::menu("&Edit", {
                Menu::action([] () {}, "&Undo", "Ctrl+Z"),
                Menu::action([] () {}, "&Redo"),
                Menu::action([] () {}, "&Cut", "Ctrl+X"),
                Menu::action([] () {}, "C&opy", "Ctrl+C"),
                Menu::action([] () {}, "&Paste", "Ctrl+V")
            }),
            Menu::menu("&View", {
                Menu::action([] () {}, "&Output"), 
                Menu::action([] () {}, "&Workspace")
            }),
            Menu::menu("&Build", {
                Menu::action([] () {}, "&Clean"),
                Menu::action([] () {}, "&Execute")
            }),
            Menu::menu("&Help", {
                Menu::action([] () {}, "&About", "F1")
            }),
        });

        this->setMenuBar(createMenuBar(this, mainMenu));
    }

    void MainWindow::setupEditorManager() {
        m_editorManager = new EditorManager(this);

        connect(m_editorManager, &EditorManager::editorContentChanged, [&](Editor *editor) {
            assert(editor);
            presenter.editorContentModified(editor);
        });
        
        connect(m_editorManager, &EditorManager::editorCloseRequested, [&](Editor *editor) {
            assert(editor);
            presenter.editorCloseRequested(editor);
        });

        this->setCentralWidget(m_editorManager);
    }
}

namespace felide {
    EditorManagerView* MainWindow::getEditorManagerView() {
        return m_editorManager;
    }

    DialogManagerView* MainWindow::getDialogManagerView() {
        return m_dialogManager.get();
    }
}
