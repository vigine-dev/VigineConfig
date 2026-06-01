#pragma once

#include <string>
#include <vector>

#include "vigine/config/configkey.h"
#include "vigine/config/configvalue.h"

namespace vigine::config
{
struct SchemaEntry
{
    ConfigKey key;
    ConfigType type;
    ConfigValue defaultValue;
};

struct ValidationResult
{
    bool ok = true;
    std::vector<std::string> errors;
    ConfigValue resolved;
};

// Declares the expected shape of a configuration: which keys exist, their type,
// and a default. validate() type-checks a tree and fills defaults for absentees.
class SchemaRegistry
{
public:
    // Register an expected key. Re-declaring the same key replaces its entry.
    void declare(ConfigKey key, ConfigType type, ConfigValue defaultValue);

    [[nodiscard]] const std::vector<SchemaEntry> &entries() const noexcept { return _entries; }

    // Type-check `tree` against the schema and return a resolved copy with
    // defaults applied for missing keys. Type mismatches are reported as errors
    // and leave the offending value untouched in the resolved tree.
    [[nodiscard]] ValidationResult validate(const ConfigValue &tree) const;

private:
    std::vector<SchemaEntry> _entries;
};
}
