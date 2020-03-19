#include <engine/core/Application.hpp>

int main() {
    using namespace caelus::core;
    Application app;
    app.load();
    app.run();
    return 0;
}