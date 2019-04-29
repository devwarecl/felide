
#include "Document.hpp"

#include <felide/util/OS.hpp>

namespace felide {
    DocumentConfig DocumentConfig::Default() {
        switch (getCurrentOS()) {
            case OS::Linux: return {"Inconsolata", 8, 4, true, true};
            case OS::Windows: return {"Consolas", 8, 4, true, true};
            default: return {"Consolas", 12, 4, true, true};
        }
    }

    Document::~Document() {}
}
