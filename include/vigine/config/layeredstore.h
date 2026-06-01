#pragma once

#include <cstddef>
#include <vector>

#include "vigine/config/configkey.h"
#include "vigine/config/configvalue.h"

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

    template <typename ValueType>
    [[nodiscard]] ValueType valueOr(const ConfigKey &key, ValueType fallback) const
    {
        if (const ConfigValue *found = find(key))
        {
            return found->valueOr<ValueType>(fallback);
        }
        return fallback;
    }

    // Merge every layer low-to-high into a single tree; tables merge recursively,
    // scalars from higher layers overwrite lower ones.
    [[nodiscard]] ConfigValue flatten() const;

private:
    std::vector<ConfigValue> _layers;
};
}
