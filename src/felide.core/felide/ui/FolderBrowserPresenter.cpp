

#include "FolderBrowserPresenter.hpp"

#include <fstream>
#include <iostream>
#include <boost/filesystem.hpp>

#include "FolderBrowser.hpp"
#include "DialogManager.hpp"
#include "IDEFramePresenter.hpp"

namespace felide {
    static std::string describePath(const boost::filesystem::path &path) {
        if (boost::filesystem::is_directory(path)) {
            return "directory";
        } else {
            return "file";
        }
    }

    FolderBrowserPresenter::FolderBrowserPresenter(IDEFramePresenter *ideFramePresenter) {
        this->ideFramePresenter = ideFramePresenter;
    }

    FolderBrowserPresenter::~FolderBrowserPresenter() {}

    void FolderBrowserPresenter::attachView(FolderBrowser *folderBrowser, DialogManager *dialogManager) {
        m_folderBrowser = folderBrowser;
        m_dialogManager = dialogManager;
    }

    void FolderBrowserPresenter::detachView() {
        m_folderBrowser = nullptr;
    }

    void FolderBrowserPresenter::browseFolder() {
        auto folderPathOptional = m_dialogManager->showFolderDialog("Open Folder");

        if (!folderPathOptional) {
            return;
        }

        m_folderBrowser->displayFolder(*folderPathOptional);
    }

    void FolderBrowserPresenter::createFile(const std::string &filePath) {
        std::ofstream os;
        os.open(filePath.c_str(), std::ios_base::out);
        os.close();
    }

    void FolderBrowserPresenter::createFolder(const std::string &folderPath) {
        namespace fs = boost::filesystem;
        boost::system::error_code errorCode;

        fs::create_directory(fs::path(folderPath), errorCode);
    }

    void FolderBrowserPresenter::openSelectedFile() {
        // determine the currently selected path
        const auto selectedPathOptional = m_folderBrowser->getSelectedPath();
        if (!selectedPathOptional) {
            return;
        }

        ideFramePresenter->editorShow(*selectedPathOptional);
    }

    void FolderBrowserPresenter::renameSelectedPath() {
        // TODO: Notify to the other views
        namespace fs = boost::filesystem;

        // determine the currently selected path
        const auto selectedPathOptional = m_folderBrowser->getSelectedPath();
        if (!selectedPathOptional) {
            return;
        }

        const auto selectedPath = fs::path(*selectedPathOptional);
        const auto pathKind = describePath(selectedPath);
        
        // prompt the user for a new path
        boost::optional<std::string> newFilenameOptional;

        int attemped = 0;

        while (true) {
            const std::string prefix = attemped > 0 ? ("Invalid " + pathKind + " name. ") : "";

            newFilenameOptional = m_dialogManager->showInputDialog (
                "felide", 
                prefix + "Please, enter a new name for the \"" + selectedPath.filename().string() + "\" " + pathKind,
                selectedPath.filename().string()
            );

            if (!newFilenameOptional) {
                return;
            }

            if (fs::native(*newFilenameOptional)) {
                break;
            }

            ++attemped;
        }

        const auto newFilename = fs::path(*newFilenameOptional);
        const auto newPath = selectedPath.parent_path() / newFilename;

        // do the rename
        boost::filesystem::rename(selectedPath, newPath);
    }

    void FolderBrowserPresenter::deleteSelectedPath() {
        // TODO: Notify to the other views
        namespace fs = boost::filesystem;

        // determine the currently selected path
        const auto selectedPathOptional = m_folderBrowser->getSelectedPath();
        if (!selectedPathOptional) {
            return;
        }

        const auto selectedPath = fs::path(*selectedPathOptional);
        
        // prompt the user confirmation
        auto selectedButton = m_dialogManager->showMessageDialog (
            "felide", 
            "Delete the \"" + selectedPath.filename().string() + "\" " + describePath(selectedPath) + "?", 
            DialogIcon::Warning, DialogButton::OkCancel
        );

        if (selectedButton == DialogButton::Cancel) {
            return;
        }
        
        // do the delete
        boost::filesystem::remove(selectedPath);
    }
} 
