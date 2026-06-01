#include "vigine/config/schemaregistry.h"

#include <sstream>
#include <utility>

#include "vigine/config/configtree.h"

namespace vigine::config
{
void SchemaRegistry::declare(ConfigKey key, ConfigType type, ConfigValue defaultValue)
{
    for (SchemaEntry &entry : _entries)
    {
        if (entry.key == key)
        {
            entry.type = type;
            entry.defaultValue = std::move(defaultValue);
            return;
        }
    }
    _entries.push_back(SchemaEntry{std::move(key), type, std::move(defaultValue)});
}

ValidationResult SchemaRegistry::validate(const ConfigValue &tree) const
{
    ValidationResult result;
    result.resolved = tree.is<ConfigTable>() ? tree : ConfigValue{ConfigTable{}};

    for (const SchemaEntry &entry : _entries)
    {
        const ConfigValue *found = findValue(tree, entry.key);
        if (found == nullptr || found->isNone())
        {
            setValue(result.resolved, entry.key, entry.defaultValue);
            continue;
        }
        if (found->type() != entry.type)
        {
            std::ostringstream stream;
            stream << "key \"" << entry.key.toString() << "\" has the wrong type";
            result.errors.push_back(stream.str());
            result.ok = false;
        }
    }
    return result;
}
}
