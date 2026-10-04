#pragma once

#include "vigine/config/configkey.h"
#include "vigine/config/configvalue.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace vigine::config
{
// Ordered stack of configuration trees. Layers pushed later take precedence;
// lookups fall through to lower layers and flatten() deep-merges them.
class LayeredStore
{
  public:
    void pushLayer(ConfigValue tree);
    void setLayer(std::size_t index, ConfigValue tree);
    [[nodiscard]] std::size_t layerCount() const noexcept { return _layers.size(); }

    // Highest-precedence value for `key`, or nullptr when no layer holds it.
    [[nodiscard]] const ConfigValue *find(const ConfigKey &key) const;

    // The highest-precedence value of that kind for `key`, or the fallback.
    [[nodiscard]] bool booleanOr(const ConfigKey &key, bool fallback) const;
    [[nodiscard]] std::int64_t integerOr(const ConfigKey &key, std::int64_t fallback) const;
    [[nodiscard]] double floatingOr(const ConfigKey &key, double fallback) const;
    [[nodiscard]] std::string stringOr(const ConfigKey &key, std::string fallback) const;

    // Merge every layer low-to-high into a single tree; tables merge recursively,
    // scalars from higher layers overwrite lower ones.
    [[nodiscard]] ConfigValue flatten() const;

  private:
    std::vector<ConfigValue> _layers;
};
} // namespace vigine::config
