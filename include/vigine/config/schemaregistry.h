#pragma once

#include "vigine/config/configkey.h"
#include "vigine/config/configvalue.h"
#include "vigine/config/vigineconfig_export.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace vigine::config
{
// One declared key: where it lives, which kind it holds, and what stands in when it is absent.
class VIGINECONFIG_API SchemaEntry
{
  public:
    SchemaEntry(ConfigKey key, ConfigType type, ConfigValue defaultValue)
        : _key(std::move(key)), _type(type), _defaultValue(std::move(defaultValue))
    {
    }

    [[nodiscard]] const ConfigKey &key() const noexcept { return _key; }
    [[nodiscard]] ConfigType type() const noexcept { return _type; }
    [[nodiscard]] const ConfigValue &defaultValue() const noexcept { return _defaultValue; }

  private:
    ConfigKey _key;
    ConfigType _type;
    ConfigValue _defaultValue;
};

// What validating a tree found: whether it is sound, the errors if not, and the tree with the
// defaults filled in.
class VIGINECONFIG_API ValidationResult
{
  public:
    ValidationResult(bool ok, std::vector<std::string> errors, ConfigValue resolved)
        : _ok(ok), _errors(std::move(errors)), _resolved(std::move(resolved))
    {
    }

    [[nodiscard]] bool ok() const noexcept { return _ok; }
    [[nodiscard]] const std::vector<std::string> &errors() const noexcept { return _errors; }
    [[nodiscard]] const ConfigValue &resolved() const noexcept { return _resolved; }

  private:
    bool _ok;
    std::vector<std::string> _errors;
    ConfigValue _resolved;
};

// Declares the expected shape of a configuration: which keys exist, their type,
// and a default. validate() type-checks a tree and fills defaults for absentees.
class VIGINECONFIG_API ISchemaRegistry
{
  public:
    virtual ~ISchemaRegistry() = default;

    // Register an expected key. Re-declaring the same key replaces its entry.
    virtual void declare(ConfigKey key, ConfigType type, ConfigValue defaultValue) = 0;

    [[nodiscard]] virtual const std::vector<SchemaEntry> &entries() const noexcept = 0;

    // Type-check `tree` against the schema and return a resolved copy with
    // defaults applied for missing keys. Type mismatches are reported as errors
    // and leave the offending value untouched in the resolved tree.
    [[nodiscard]] virtual ValidationResult validate(const ConfigValue &tree) const = 0;
};

// A registry with nothing declared.
[[nodiscard]] VIGINECONFIG_API std::unique_ptr<ISchemaRegistry> createSchemaRegistry();
} // namespace vigine::config
