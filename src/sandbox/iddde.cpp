
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <filesystem>
#include <regex>
#include <algorithm>
#include <cstdlib>
#include <stdexcept>

namespace fs = std::filesystem;

// Project Model for C/C++
namespace borc::model {
    std::string join(const std::vector<std::string> &strings, const std::string &separator) {
        std::string str;

		for (std::size_t i=0; i<strings.size(); i++) {
			str += strings[i];

			if (i < strings.size() - 1) {
				str += separator;
			}
		}

        return str;
    }

    enum class ModuleType {
        Library,
        Executable
    };

    inline std::string toString(const ModuleType type) {
        switch (type) {
            case ModuleType::Executable: 
                return "executable";

            case ModuleType::Library:
                return "library";

            default:
                return "<unknown ModuleType>";
        }
    }

    class Module;
    class Project {
    public:
        Project(const std::string &name, const std::string &fullPath);

        Module* addModule(const std::string &name, ModuleType type, const std::string &path, const std::vector<std::string> &files);
        
        Module* addModule(const std::string &name, ModuleType type, const std::string &path, const std::vector<std::string> &files, const std::vector<Module*> &dependencies);

        std::string getName() const;

        fs::path getFullPath() const;

        std::vector<const Module*> getModules() const;

        fs::path computeOutputPath() const;

    private:
        //! The name of the project
        std::string name;

        //! The full path to the project directory
        std::string fullPath;

        //! The list of software modules this project has
        std::vector<std::unique_ptr<Module>> modules;
    };

    class Module {
    public:
        Module(Project *parent, const std::string &name, ModuleType type, const std::string &path, const std::vector<std::string> &files, const std::vector<Module*> &dependencies) {
            this->parentProject = parent;
            this->name = name;
            this->type = type;
            this->path = path;
            this->files = files;
            this->dependencies = dependencies;
        }

        std::string getName() const {
            return name;
        }

        std::string getPath() const {
            return path;
        }

        std::vector<std::string> getFiles() const {
            return files;
        }

        ModuleType getType() const {
            return type;
        }

        const Project *getParentProject() const {
            return parentProject;
        }

        std::vector<Module*> getDependencies() const {
            return dependencies;
        }

        fs::path computeOutputPath() const {
            return this->parentProject->computeOutputPath() / this->getPath();
        }

        fs::path computeFullPath() const {
            return this->parentProject->getFullPath() / this->getPath();
        }

        fs::path computeOutputPathFile() const {
            std::string moduleFileName = this->getName();

            if (this->getType() == ModuleType::Library) {
                // moduleFileName = "lib" + module->getName() + ".dylib";
                // moduleFileName = "lib" + moduleFileName + ".so";
				moduleFileName = moduleFileName + ".dll";
            }

            return this->computeOutputPath() / fs::path(moduleFileName);
        }

    private:
        //! parent project of the module
        Project *parentProject = nullptr;

        //! name of the target
        std::string name;

        //! the type of the module
        ModuleType type = ModuleType::Executable;

        //! relative path to the parent project
        std::string path;

        //! list of files that compose the module
        std::vector<std::string> files;

        //! list of module libraries that this module depends on
        std::vector<Module*> dependencies;
    };

    Project::Project(const std::string &name, const std::string &fullPath) {
        this->name = name;
        this->fullPath = fullPath;
    }

    Module* Project::addModule(const std::string &name, ModuleType type, const std::string &path, const std::vector<std::string> &files) {
        return this->addModule(name, type, path, files, {});
    }
    
    Module* Project::addModule(const std::string &name, ModuleType type, const std::string &path, const std::vector<std::string> &files, const std::vector<Module*> &dependencies) {
        auto module = new Module(this, name, type, path, files, dependencies);

        modules.emplace_back(module);

        return module;
    }

    std::string Project::getName() const {
        return name;
    }

    fs::path Project::getFullPath() const {
        return fs::path(fullPath);
    }

    std::vector<const Module*> Project::getModules() const {
        std::vector<const Module*> modules;

        for (const auto &module : this->modules) {
            modules.push_back(module.get());
        }

        return modules;
    }

    fs::path Project::computeOutputPath() const {
        return fs::path(this->getFullPath()) / fs::path(".borc");
    }

    class Command {
    public:
		virtual ~Command() {}
        
		virtual void execute() = 0;

		virtual void addOption(const std::string &option) = 0;

        template<typename Iterator>
        void addOptionRange(Iterator begin, Iterator end) {
			Iterator it = begin;

			while (it != end) {
				this->addOption(*it);
				it++;
			}
        }
    };

	class SystemCommand : public Command {
	public:
		explicit SystemCommand(const std::string &base)
			: _base(base) {}

		explicit SystemCommand(const std::string &base, const std::vector<std::string> &options)
			: _base(base), _options(options) {}

		virtual void execute() override {
			const std::string systemCommand = _base + " " + join(_options, " ");

			std::cout << systemCommand << std::endl;

			const int exitCode = std::system(systemCommand.c_str());

			if (exitCode != 0) {
				throw std::runtime_error("The command returned an erroneous exit code: " + std::to_string(exitCode));
			}
		}

		virtual void addOption(const std::string &option) override {
			_options.push_back(option);
		}

	private:
		const std::string _base;
		std::vector<std::string> _options;
	};

	class CommandFactory {
	public:
		Command* createCommand(const std::string &base, const std::vector<std::string> &options = {}) {
			auto command = new SystemCommand(base, options);

			_cachedCommands.emplace_back(command);

			return command;
		}

	private:
		std::vector<std::unique_ptr<Command>> _cachedCommands;
	};

    struct CompilerSwitches {
        std::string compile;
        std::string objectFileOutput;
        std::string includeDebug;
        std::string zeroOptimization;
    };

    class Compiler {
    public:
        explicit Compiler(CommandFactory *commandFactory, const std::string &commandPath, const CompilerSwitches &switches) {
			this->commandFactory = commandFactory;
            this->commandPath = commandPath;
            this->switches = switches;
        }

        std::string compile(const Project *project, const Module *module, const std::string &file) const {
            const fs::path sourceFilePath = module->computeFullPath() / fs::path(file);
            const fs::path objectFilePath = module->computeOutputPath() / fs::path(file);
            
            std::cout << "    " << file  << " ..." << std::endl;

            Command *command = commandFactory->createCommand(
                commandPath, {
                    switches.zeroOptimization, 
                    switches.includeDebug, 
                    switches.compile, 
                    sourceFilePath.string(),
                    switches.objectFileOutput + objectFilePath.string(),
                }
            );

            command->execute();

			return objectFilePath.string();
        }

    private:
		CommandFactory *commandFactory = nullptr;
        std::string commandPath;
        CompilerSwitches switches;
    };

    struct LinkerSwitches {
        std::string buildSharedLibrary;
        std::string moduleOutput;
        std::string importLibrary;
        std::string importLibraryPath;
    };

    class Linker {
    public:
        explicit Linker(CommandFactory *commandFactory, const std::string &commandPath, const LinkerSwitches &switches) {
			this->commandFactory = commandFactory;
            this->commandPath = commandPath;
            this->switches = switches;
        }

        std::string link(const Project *project, const Module *module, const std::vector<std::string> &objectFiles) const {
            std::cout << "Linking " << toString(module->getType()) << " module " << module->getName() << " ..." << std::endl;

            const std::string outputModuleFilePath = module->computeOutputPath().string();
            const auto librariesOptions = this->computeImportLibrariesOptions(project, module);

			Command *command = commandFactory->createCommand(commandPath);

            if (module->getType() == ModuleType::Library) {
                command->addOption(switches.buildSharedLibrary);
            }
            
            command->addOptionRange(objectFiles.begin(), objectFiles.end());
            command->addOptionRange(librariesOptions.begin(), librariesOptions.end());
            command->addOption(switches.moduleOutput + outputModuleFilePath);

            return outputModuleFilePath;
        }

    private:
        std::vector<std::string> computeImportLibrariesOptions(const Project *project, const Module *module) const {
            std::vector<std::string> options = {
                "-lstdc++", "-lstdc++fs"
            };

            const auto dependencies = module->getDependencies();

            for (const Module *dependency : dependencies) {
                const std::string importLibrary = dependency->getName();
                const std::string importLibraryDir = dependency->computeOutputPath().string();

                options.push_back(switches.importLibrary + importLibrary);
                options.push_back(switches.importLibraryPath + importLibraryDir);
            }

            return options;
        }

    private:
		CommandFactory *commandFactory = nullptr;
        std::string commandPath;
        LinkerSwitches switches;
    };

    class BuildService {
    private:
        const Compiler *compiler;
        const Linker *linker;

        //! list of extension wildcards that will trigger a source file compilation with the current compiler
        const std::vector<std::string> compilerWildcards = {
            "*.c", "*.cpp", "*.c++", "*.cc", "*.cxx", 
        };

    public:
        explicit BuildService(const Compiler *compiler, const Linker *linker) {
            this->compiler = compiler;
            this->linker = linker;
        }
            
        void buildProject(const Project *project) {
            auto modules = project->getModules();

            int builtModules = 0;

            for (const Module *module : modules) {
                std::cout << "Building module " << module->getName() << " ..." << std::endl;

                const auto files = module->getFiles();

                std::vector<std::string> objectFiles;
                std::copy_if(files.begin(), files.end(), std::back_inserter(objectFiles), [&](const auto &file) {
                    return this->isFileCompilable(file);
                });

                std::transform(objectFiles.begin(), objectFiles.end(), objectFiles.begin(), [&](const auto &file) {
                    return this->compiler->compile(project, module, file);
                });

                linker->link(project, module, objectFiles);

                builtModules++;
            }

            std::cout << "Built " << builtModules << " module(s)." << std::endl;
        }

    private:
        bool isFileCompilable(const std::string &file) const {
            const std::string ext = "*" + fs::path(file).extension().string();

            auto it = std::find(compilerWildcards.begin(), compilerWildcards.end(), ext);

            return it != compilerWildcards.end();
        }
    };

    class RunService {
    public:
        explicit RunService(const Compiler *compiler, const Linker *linker) {
            this->compiler = compiler;
            this->linker = linker;
        }
    
        void runModule(const Module *module) {
            // 
            auto deps = module->getDependencies();

            std::vector<std::string> paths;
            std::transform(deps.begin(), deps.end(), std::back_inserter(paths), [](const Module *dep) {
                return dep->computeOutputPath().string();
            });

            // TODO: Parametrize path separator
            const std::string pathEnv = join(paths, ":");
			const std::string moduleFilePath = module->computeOutputPathFile().string();

            std::cout << "ENV = " << pathEnv << std::endl;
            std::cout << "EXEC " << moduleFilePath << std::endl;
        }

    private:
        const Compiler *compiler;
        const Linker *linker;
    };
}

#define XSTR(a) STR(a)
#define STR(a) #a

std::string getFullPath() {
    return XSTR(PROJECT_SOURCE_DIR);
}

int main(int argc, char **argv) {
    using namespace borc::model;

    const std::string fullPath = getFullPath();

    Project borcProject{"borc", fullPath};

    Module *borcCoreModule = borcProject.addModule("borc.core", ModuleType::Library, "src/borc.core", {
        "BuildService.hpp", 
        "BuildService.cpp",
        "Module.hpp", 
        "Module.cpp",
        "Project.hpp", 
        "Project.cpp",
        "Compiler.hpp", 
        "Compiler.cpp",
        "Linker.hpp", 
        "Linker.cpp"
    });

    Module *borcCliModule = borcProject.addModule("borc.cli", ModuleType::Executable, "src/borc.cli", {
        "Main.cpp"
    }, {borcCoreModule});

    // const std::string commandBase = "/usr/local/Cellar/gcc/8.2.0/bin/gcc-8";
    // const std::string commandBase = "gcc";

    // const Compiler compiler { commandBase, {"-c", "-o", "-g", "-O0"} };
    // const Linker linker { commandBase, {"-shared", "-o", "-l", "-L"} };

	const std::string commandBasePath = "C:\\Program Files(x86)\\Microsoft Visual Studio\\2017\\Community\\VC\\Tools\\MSVC\\14.16.27023\\bin\\Hostx64\\x64\\";
	const std::string commandCompiler = commandBasePath + "cl.exe";
	const std::string commandLinker = commandBasePath + "link.exe";

	CommandFactory commandFactory;

	const Compiler compiler { &commandFactory, "\"" + commandCompiler + "\"", {"/c", "", "", "/O0"} };
	const Linker linker { &commandFactory, commandLinker, {"/DLL", "", "/IMPLIB:", "/LIBPATH:"} };

    BuildService buildService {&compiler, &linker};
    buildService.buildProject(&borcProject);

    RunService runService {&compiler, &linker};
    runService.runModule(borcCliModule);

    return 0;
}
