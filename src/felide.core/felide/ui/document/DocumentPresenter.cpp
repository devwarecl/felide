
#include "DocumentPresenter.hpp"

#include "DocumentModel.hpp"
#include "Document.hpp"

#include <boost/filesystem/path.hpp>

#include <felide/ui/DialogManager.hpp>
#include <felide/util/FileService.hpp>

namespace felide {
    DocumentPresenter::DocumentPresenter(DocumentModel *model) {
        this->model = model;
    }

    DocumentPresenter::~DocumentPresenter() {}

    void DocumentPresenter::onInitialized(Document *view, DialogManager *dialogView) {
        this->view = view;
        this->dialogView = dialogView;

        if (model->hasFilePath()) {
            auto fileService = FileService::create();
            auto content = fileService->load(model->getFilePath());

            model->setContent(content);
        }

        const std::string title = this->computeTitle(model);
        view->setTitle(title);
        view->setContent(model->getContent());
    }

    void DocumentPresenter::onContentChanged() {
        model->modify();

        const std::string title = this->computeTitle(model);
        view->setTitle(title);
    }

    void DocumentPresenter::onSave() {
        // TODO: Put FileFilter getters from a PresenterService

        if (!model->hasFilePath()) {
            auto fileDialog = FileDialogData {};
            fileDialog.title = "Save File";
            fileDialog.type = FileDialogType::SaveFile;
            fileDialog.defaultPath = this->computeFileTitle(model);
            fileDialog.filters = {
                FileFilter{"All Files", {"*.*"}}
            };

            if (auto filePath = dialogView->showFileDialog(fileDialog)) {
                model->setFilePath(filePath.get().string());
            } else {
                return;
            }
        }

        auto fileService = FileService::create();

        model->setContent(view->getContent());

        const std::string fileName = model->getFilePath();
        const std::string content = model->getContent();
        
        fileService->save(fileName, content);

        model->setFilePath(fileName);
        model->setModifiedFlag(false);

        const std::string title = this->computeTitle(model);

        view->setTitle(title);
    }

    void DocumentPresenter::onSaveAs() {
        auto fileDialog = FileDialogData {};

        fileDialog.title = "Save File";
        fileDialog.type = FileDialogType::SaveFile;
        fileDialog.filters = {
            FileFilter{"All Files", {"*.*"}}
        };

        if (auto filePath = dialogView->showFileDialog(fileDialog)) {
            model->setFilePath(filePath.get().string());

            this->onSave();
        } else {
            return;
        }
    }

    void DocumentPresenter::onTitleChanged() {
        const std::string title = this->computeTitle(model);
        view->setTitle(title);
    }

    std::string DocumentPresenter::computeFileTitle(DocumentModel *model) const {
        if (model->hasFilePath()) {
            return boost::filesystem::path(model->getFilePath()).filename().string();
        }

        return "Untitled " + std::to_string(model->getTag());
    }

    std::string DocumentPresenter::computeTitle(DocumentModel *model) const {
        const std::string prefix = (model->getModifiedFlag() ? "[*]" : "");
        const std::string fileTitle = this->computeFileTitle(model);

        return prefix + fileTitle;
    }

    void DocumentPresenter::onClose() {
        // TODO: Add implementation
    }

    void DocumentPresenter::onShow() {
        // TODO: Add implementation
    }

    bool DocumentPresenter::hasFilePath(const boost::filesystem::path &filePath) const {
        return boost::filesystem::path(model->getFilePath()) == filePath;
    }
    
    Document* DocumentPresenter::getView() const {
        return view;
    }

    DocumentModel* DocumentPresenter::getModel() const {
        return model;
    }
}
