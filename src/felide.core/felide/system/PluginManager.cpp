
#include "PluginManager.hpp"

#include <stdexcept>
#include <iostream>
#include <boost/dll.hpp>
#include <boost/filesystem/path.hpp>
#include "Plugin.hpp"

namespace felide {
    typedef felide::Plugin* (*PluginCreateProc)();
    typedef void (*PluginDestroyProc)(Plugin *plugin);

    class PluginProxy : public Plugin {
    public:
        explicit PluginProxy(const boost::filesystem::path &libraryPath) {
            const auto loadMode = boost::dll::load_mode::default_mode;
            m_library = boost::dll::shared_library(libraryPath, loadMode);

            m_pluginCreate = m_library.get<Plugin*()>("felide_plugin_create");
            m_pluginDestroy = m_library.get<void (Plugin*)>("felide_plugin_destroy");

            m_plugin = m_pluginCreate();
        }

        virtual ~PluginProxy() {
            if (m_pluginDestroy && m_plugin) {
                m_pluginDestroy(m_plugin);
            }
        }

        virtual void start(Core *core) override {
            return m_plugin->start(core);
        }

        virtual void stop(Core *core) override {
            return m_plugin->stop(core);
        }

        virtual PluginInformation getInformation() const override {
            return m_plugin->getInformation();
        }

    private:
        PluginCreateProc m_pluginCreate = nullptr;
        PluginDestroyProc m_pluginDestroy = nullptr;
        Plugin *m_plugin = nullptr;
        boost::dll::shared_library m_library;
    };

    struct PluginManager::Private {
        Core *core = nullptr;
        std::vector<std::unique_ptr<Plugin>> plugins;
    };

    PluginManager::PluginManager(Core *core) {
        m_impl = new PluginManager::Private();
        m_impl->core = core;
    }

    PluginManager::~PluginManager() {
        delete m_impl;
    }

    std::string mapNameToNative(const std::string &name) {
#if defined(_WINDOWS)
        return name + ".dll";
#else 
        return name;
#endif
    }
    
    void PluginManager::loadPlugin(const std::string &name) {
        // TODO: Export this to a configuration file
        // const boost::filesystem::path pluginFolder = "/usr/local/lib";
        const boost::filesystem::path pluginFolder = "build/Debug/";

        try {
            std::cout << "Loading " << name << std::endl;
            const auto mappedName = mapNameToNative(name);
            auto plugin = new PluginProxy(pluginFolder / mappedName);

            m_impl->plugins.emplace_back(plugin);

            plugin->start(m_impl->core);

            std::cout << "Load OK " << name << std::endl;
        } catch (const std::exception &exp) {
            const std::string msg = exp.what();
            std::cout << "Load failed:" << std::endl << msg << std::endl;
        }
    }
}
