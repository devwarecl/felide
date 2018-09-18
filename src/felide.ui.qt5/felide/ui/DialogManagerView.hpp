
#ifndef __FELIDE_UI_DIALOGMANAGERVIEW_HPP__
#define __FELIDE_UI_DIALOGMANAGERVIEW_HPP__

#include <string>
#include <vector>
#include <boost/optional.hpp>

namespace felide {
    enum class DialogIcon {
        Information,
        Warning,
        Error,
        Question
    };

    enum class DialogButton {
        Ok = 1,
        Cancel = 2,
        Yes = 4,
        No = 8,
        OkCancel = Ok | Cancel,
        YesNoCancel = Yes | No | Cancel,
    };

    inline DialogButton operator| (const DialogButton f1, const DialogButton f2) {
        return static_cast<DialogButton>(static_cast<int>(f1) | static_cast<int>(f2));
    }

    struct FileFilter {
        std::string description;
        std::vector<std::string> wildcards;
    };

    enum class FileDialogType {
        OpenFile,
        SaveFile
    };

    class DialogManagerView {
    public:
        virtual ~DialogManagerView();
        virtual DialogButton showMessageDialog(const std::string &title, const std::string &msg, const DialogIcon icon, const DialogButton buttons) const = 0;
		virtual boost::optional<std::string> showFileDialog(const std::string &title, const FileDialogType dialogType, const std::vector<FileFilter> &filters) const = 0;
    };
}

#endif
