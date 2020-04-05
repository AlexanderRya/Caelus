#ifndef CAELUS_IMGUICONTEXT_HPP
#define CAELUS_IMGUICONTEXT_HPP

#include <engine/Forwards.hpp>

namespace caelus::core::api {
    struct ImGuiContext{}; // Empty for now

    /*[[nodiscard]]*/ ImGuiContext make_imgui_context(const VulkanContext&, const Window&);
} // namespace caelus::core::api

#endif //CAELUS_IMGUICONTEXT_HPP
