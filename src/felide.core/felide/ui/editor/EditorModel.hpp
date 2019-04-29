
#ifndef __FELIDE_UI_DOCUMENTMODEL_HPP__
#define __FELIDE_UI_DOCUMENTMODEL_HPP__

#include <string>

namespace felide {
    class DocumentModel {
    public:
        explicit DocumentModel(int tag);

        explicit DocumentModel(const std::string &filePath);

        explicit DocumentModel(const std::string &filePath, const std::string &content);

        ~DocumentModel();

        int getTag() const;

        int getId() const;

        void setModifiedFlag(const bool value);

        bool getModifiedFlag() const;

        void modify();

        void setFilePath(const std::string &value);

        std::string getFilePath() const;

        bool hasFilePath() const;

        void setContent(const std::string &value);

        std::string getContent() const;

    private:
        int tag = 0;
        int id = 0;
        bool modified = false;
        std::string filePath;
        std::string content;

    private:
        static int count;
    };
}

#endif
