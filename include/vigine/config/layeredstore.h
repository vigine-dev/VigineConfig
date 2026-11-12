#pragma once

#include "vigine/config/configkey.h"
#include "vigine/config/configvalue.h"
#include "vigine/config/vigineconfig_export.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

namespace vigine::config
{
// Ordered stack of configuration trees. Layers pushed later take precedence;
// lookups fall through to lower layers and flatten() deep-merges them.
class VIGINECONFIG_API ILayeredStore
{
  public:
    virtual ~ILayeredStore()                                      = default;

    virtual void pushLayer(ConfigValue tree)                      = 0;
    virtual void setLayer(std::size_t index, ConfigValue tree)    = 0;
    [[nodiscard]] virtual std::size_t layerCount() const noexcept = 0;

    // Highest-precedence value for `key`, or nullptr when no layer holds it.
    [[nodiscard]] virtual const ConfigValue *find(const ConfigKey &key) const = 0;

    // The highest-precedence value of that kind for `key`, or the fallback.
    [[nodiscard]] virtual bool booleanOr(const ConfigKey &key, bool fallback) const      = 0;
    [[nodiscard]] virtual std::int64_t integerOr(const ConfigKey &key,
                                                 std::int64_t fallback) const            = 0;
    [[nodiscard]] virtual double floatingOr(const ConfigKey &key, double fallback) const = 0;
    [[nodiscard]] virtual std::string stringOr(const ConfigKey &key,
                                               std::string fallback) const               = 0;

    // Merge every layer low-to-high into a single tree; tables merge recursively,
    // scalars from higher layers overwrite lower ones.
    [[nodiscard]] virtual ConfigValue flatten() const = 0;
};

// An empty store with no layers.
[[nodiscard]] VIGINECONFIG_API std::unique_ptr<ILayeredStore> createLayeredStore();
} // namespace vigine::config
