
#include "EditorModel.hpp"

namespace felide {
    int EditorModel::count = 0;

    EditorModel::EditorModel(int tag) {
        tag = tag;
        id = ++count;
    }

    EditorModel::EditorModel(const std::string &filePath) {
        id = ++count;

        this->setFilePath(filePath);
    }

    EditorModel::EditorModel(const std::string &filePath, const std::string &content) {
        id = ++count;

        this->setFilePath(filePath);
        this->setContent(content);
    }

    EditorModel::~EditorModel() {}

    int EditorModel::getTag() const {
        return tag;
    }

    int EditorModel::getId() const {
        return id;
    }

    void EditorModel::setModifiedFlag(const bool value) {
        modified = value;
    }

    bool EditorModel::getModifiedFlag() const {
        return modified;
    }

    void EditorModel::modify() {
        modified = true;
    }

    void EditorModel::setFilePath(const std::string &value) {
        filePath = value;
    }

    std::string EditorModel::getFilePath() const {
        return filePath;
    }

    bool EditorModel::hasFilePath() const {
        return filePath != "";
    }

    void EditorModel::setContent(const std::string &value) {
        content = value;
    }

    std::string EditorModel::getContent() const {
        return content;        
    }
}