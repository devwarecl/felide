
#ifndef __FELIDE_FILESEARCHDIALOGITEMWIDGETQT_HPP__
#define __FELIDE_FILESEARCHDIALOGITEMWIDGETQT_HPP__

#include <QWidget>
#include <QLabel>
#include <QListWidgetItem>

namespace felide {
    class FileSearchDialogItemWidgetQt : public QWidget {
        Q_OBJECT
        
    public:
        FileSearchDialogItemWidgetQt(QWidget *parent, const QString &fileTitle, const QString &fileFolder, const QString &filePath);

        QString getFilePath() const {
            return filePath;
        }

    private:
        QString filePath;
    };
}

#endif
