
#include "Editor.hpp"

#include <cassert>
#include <iostream>
#include <QLayout>
#include "EditorManager.hpp"

namespace felide {
    Editor::Editor(QWidget *parent, EditorManager *editorManager) : QWidget(parent) {
        m_editorManager = editorManager;
        m_scintilla = new QsciScintilla(this);

        this->setupScintilla();
        this->setupLayout();

        m_scintilla->setFocus();
    }

    Editor::~Editor() {}

    void Editor::setupScintilla() {
        connect(m_scintilla, &QsciScintilla::textChanged, [this]() {
            contentChanged();
        });
    }

    void Editor::setupLayout() {
        QGridLayout *layout = new QGridLayout(this);
        layout->addWidget(m_scintilla);
        this->setLayout(layout);
    }

    void Editor::setTitle(const std::string &title) {
        m_editorManager->changeEditorTitle(this, title);
    }

    std::string Editor::getTitle() const {
        return m_title;
    }

    void Editor::setContent(const std::string &content)  {
        m_scintilla->setText(content.c_str());
    }

    std::string Editor::getContent() const  {
        return m_scintilla->text().toStdString();
    }

    void Editor::setConfig(const EditorConfig &config)  {
        assert(m_scintilla);

        auto font = QFont{config.fontName.c_str(), config.fontSize};

        m_scintilla->setFont(font);
        m_scintilla->setCaretLineVisible(config.caretLineVisible);
        m_scintilla->setTabWidth(config.tabWidth);

        if (config.showLineNumbers) {
            m_scintilla->setMarginType(1, QsciScintilla::NumberMargin);
        }

        m_config = config;
    }

    EditorConfig Editor::getConfig() const  {
        assert(m_scintilla);

        return m_config;
    }

    void Editor::undo()  {
        assert(m_scintilla);
        m_scintilla->undo();
    }

    void Editor::redo()  {
        assert(m_scintilla);
        m_scintilla->redo();
    }

    void Editor::cut()  {
        assert(m_scintilla);
        m_scintilla->cut();
    }

    void Editor::copy()  {
        assert(m_scintilla);
        m_scintilla->copy();
    }

    void Editor::paste()  {
        assert(m_scintilla);
        m_scintilla->paste();
    }

    void Editor::clearAll()  {
        // TODO: Agregar implementacion
    }

    void Editor::clearUndoBuffer()  {
        // TODO: Agregar implementacion
    }
}
