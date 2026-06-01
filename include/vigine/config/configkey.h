#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace vigine::config
{
// Dotted path into a configuration tree, e.g. "window.size.width". Segments are
// non-empty; an empty path is the root.
class ConfigKey
{
public:
    ConfigKey() = default;
    explicit ConfigKey(std::string_view dotted);

    [[nodiscard]] const std::vector<std::string> &segments() const noexcept { return _segments; }
    [[nodiscard]] std::size_t size() const noexcept { return _segments.size(); }
    [[nodiscard]] bool empty() const noexcept { return _segments.empty(); }

    [[nodiscard]] std::string toString() const;
    [[nodiscard]] ConfigKey parent() const;
    [[nodiscard]] std::string_view leaf() const noexcept;

    ConfigKey &append(std::string_view segment);

    bool operator==(const ConfigKey &other) const = default;

private:
    std::vector<std::string> _segments;
};
}
