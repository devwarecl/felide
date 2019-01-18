
#include "Toolchain.hpp"

#include <borc/core/Source.hpp>
#include <algorithm>

namespace borc {
    SourceType::SourceType(const std::initializer_list<std::string> &wildcards) {
        this->wildcards = wildcards;
    }

    bool SourceType::match(const std::filesystem::path &filePath) const {
        const std::string ext = "*" + filePath.extension().string();

        auto it = std::find(wildcards.begin(), wildcards.end(), ext);

        return it != wildcards.end();
    }
}

namespace borc {
    Toolchain::Toolchain(const std::vector<std::pair<SourceType, const Compiler*>> &compilers, const Linker *linker) {
        this->compilers = compilers;
        this->linker = linker;
    }

    Toolchain::~Toolchain() {}

    const Compiler* Toolchain::selectCompiler(const Source *source) const {
        for (auto &pair : compilers) {
            if (pair.first.match(source->getFilePath())) {
                return pair.second;
            }
        }

        return nullptr;
    }

    const Linker* Toolchain::selectLinker(const Module *module) const {
        return linker;
    }
}
