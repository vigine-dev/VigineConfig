#pragma once

#include "vigine/config/configkey.h"
#include "vigine/config/configvalue.h"

namespace vigine::config
{
// Pointer to the value at `key` within `tree`, or nullptr when any path segment
// is absent or not a table. An empty key returns the tree itself.
[[nodiscard]] const ConfigValue *findValue(const ConfigValue &tree, const ConfigKey &key);

// Store `value` at `key`, creating (and, when needed, replacing non-table nodes
// with) intermediate tables along the path. An empty key replaces the tree.
void setValue(ConfigValue &tree, const ConfigKey &key, ConfigValue value);
}
