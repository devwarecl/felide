
#include <cassert>
#include <felide/Core.hpp>
#include <felide/ui/UIToolkit.hpp>
#include <felide/system/PluginManager.hpp>

#include <iostream>

int main(int argc, char **argv) {
    felide::Core core;

    auto pluginManager = core.getPluginManager();

    pluginManager->loadPlugin("felide.ui.qt5");

    auto toolkit = core.getToolkit();

    if (toolkit) {
        return toolkit->runApp(argc, argv);
    } else {
        std::cout << "felide: No UI Toolkit found" << std::endl;
        return 0;
    }
}
