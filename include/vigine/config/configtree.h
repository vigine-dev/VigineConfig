#pragma once

#include "vigine/config/configkey.h"
#include "vigine/config/configvalue.h"
#include "vigine/config/vigineconfig_export.h"

namespace vigine::config
{
// Pointer to the value at `key` within `tree`, or nullptr when any path segment
// is absent or not a table. An empty key returns the tree itself.
[[nodiscard]] VIGINECONFIG_API const ConfigValue *findValue(const ConfigValue &tree,
                                                            const ConfigKey &key);

// Store `value` at `key`, creating (and, when needed, replacing non-table nodes
// with) intermediate tables along the path. An empty key replaces the tree.
VIGINECONFIG_API void setValue(ConfigValue &tree, const ConfigKey &key, ConfigValue value);
} // namespace vigine::config
