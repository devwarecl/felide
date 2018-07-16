
#include "ModuleLinker.hpp"

#include <felide/TreeNode.hpp>
#include <felide/util/Strings.hpp>
#include <felide/pom/Source.hpp>
#include <felide/pom/Target.hpp>
#include <felide/tasks/LogTask.hpp>
#include <felide/tasks/Task.hpp>

#include <experimental/filesystem>
#include <fmt/format.h>

namespace felide {
    ModuleLinker::ModuleLinker(const LinkerDescription &description) {
        m_description = description;
    }

    ModuleLinker::~ModuleLinker() {}

    bool ModuleLinker::isLinkable(const Target *target) const {
        return target != nullptr;
    }

    std::string ModuleLinker::getToolName() const {
        return "m_toolName";
    }

    std::string ModuleLinker::getPath() const {
        return "m_path";
    }

    std::unique_ptr<TreeNode<Task>> ModuleLinker::createTask(const Target *target, const std::vector<std::string> &objectFiles) {
        assert(target);

        if (objectFiles.size() == 0) {
            throw std::runtime_error("At linking stage we should have one or more object files");
        }

        const std::string joinedObjectFiles = join(objectFiles, " ");
        const std::string targetName = target->getName();

        std::string cmd = m_description.linkTemplate;

        cmd = replace(cmd, "${ObjectFiles}", joinedObjectFiles);
        cmd = replace(cmd, "${TargetName}", targetName);

        return TreeNode<Task>::create(std::make_unique<LogTask>(cmd));
    }
}