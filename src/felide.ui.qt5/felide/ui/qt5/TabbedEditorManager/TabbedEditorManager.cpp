
#include "TabbedEditorManager.hpp"

#include <QGridLayout>
#include "Editor.hpp"

namespace felide {
    TabbedEditorManager::TabbedEditorManager(QWidget *parent) : QWidget(parent) {
        m_tabWidget = new QTabWidget(this);
        m_tabWidget->setTabsClosable(true);

        QGridLayout *layout = new QGridLayout(this);
        layout->addWidget(m_tabWidget);
        this->setLayout(layout);
    }

    TabbedEditorManager::~TabbedEditorManager() {}

    EditorView* TabbedEditorManager::appendEditor() {
        auto view = new Editor(m_tabWidget);

        m_tabWidget->addTab(view, "test title");
        m_tabWidget->setCurrentWidget(view);

        return view;
    }

    EditorView* TabbedEditorManager::getCurrentEditor() {
        QWidget *widget = m_tabWidget->currentWidget();

        if (!widget) {
            return nullptr;
        }

        return dynamic_cast<EditorView*>(widget);
    }
}
