#ifndef CAELUS_UTIL_HPP
#define CAELUS_UTIL_HPP

#include <engine/Types.hpp>

#include <glm/vec4.hpp>

#include <string>
#include <sstream>
#include <utility>

#if _WIN32
    #include <Windows.h>
    #include <libloaderapi.h>
#elif __linux__
    #include <dlfcn.h>
#endif

#define CAELUS_MAKE_VERSION(major, minor, patch) (((major) << 22u) | ((minor) << 12u) | (patch))

namespace caelus::util {
#if _WIN32
    constexpr inline const char* vulkan_module = "vulkan-1.dll";
    HMODULE load_module(LPCSTR);
    void (*load_symbol(HMODULE, LPCSTR))();
    void close_module(HMODULE);
#elif __linux__
    constexpr inline const char* vulkan_module = "libvulkan.so";
    void* load_module(const char*);
    void (*load_symbol(void*, const char*))();
    void close_module(void*);
#endif
    template <typename ...Args>
    [[nodiscard]] std::string format(const std::string& str, Args&& ...args) {
        /* Check if argument count and format specifiers match */ {
            u64 start = 0;
            u64 count = 0;

            while ((start = str.find("{}", start)) != std::string::npos) {
                start += 2;
                ++count;
            }

            if (count != sizeof...(args)) {
                throw std::runtime_error("Format specifiers and argument count mismatch");
            }
        }

        auto internal_fmt = [](const std::string& istr, std::stringstream& oss, size_t& start_idx, const auto& var) {
            size_t index = istr.find("{}", start_idx);

            if (index == std::string::npos) {
                return;
            }

            oss << istr.substr(start_idx, index - start_idx) << var;
            start_idx = index + 2;
        };

        std::stringstream oss{};
        size_t start_idx = 0;

        (internal_fmt(str, oss, start_idx, std::forward<Args>(args)), ...);

        return oss << str.substr(start_idx, str.length()), oss.str();
    }

    [[nodiscard]] std::string get_current_timestamp();
    void print(const std::string&);
    void print(const char*);
    void print(const void* addr);

    template <typename Ty, std::enable_if_t<std::is_arithmetic_v<Ty>>* = nullptr>
    void print(const Ty val) {
        print(std::to_string(val));
    }

    template <typename Ty>
    static inline void hash_combine(usize& seed, const Ty& v) {
        std::hash<Ty> hasher{};
        seed ^= hasher(v) + 0x9e3779b9 + (seed << 6u) + (seed >> 2u);
    }

    template <typename Ty, typename... Rest>
    inline void hash_combine(usize& seed, const Ty& v, Rest... rest) {
        std::hash<Ty> hasher{};
        seed ^= hasher(v) + 0x9e3779b9 + (seed << 6u) + (seed >> 2u);
        (hash_combine(seed, rest), ...);
    }
} // namespace caelus::util

namespace std {
    template <>
    struct hash<glm::vec4> {
        caelus::usize operator ()(const glm::vec4& v) const noexcept {
            caelus::usize id{};

            caelus::util::hash_combine(id, v.x, v.y, v.z, v.w);

            return id;
        }
    };
} // namespace std

#endif //CAELUS_UTIL_HPP
