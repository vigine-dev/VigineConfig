#include "vigine/config/configtree.h"

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

namespace vigine::config
{
const ConfigValue *findValue(const ConfigValue &tree, const ConfigKey &key)
{
    const ConfigValue *current = &tree;
    for (const std::string &segment : key.segments())
    {
        const ConfigTable *table = current->getIf<ConfigTable>();
        if (table == nullptr)
        {
            return nullptr;
        }
        const auto found = table->find(segment);
        if (found == table->end())
        {
            return nullptr;
        }
        current = &found->second;
    }
    return current;
}

void setValue(ConfigValue &tree, const ConfigKey &key, ConfigValue value)
{
    if (key.empty())
    {
        tree = std::move(value);
        return;
    }

    ConfigValue *current = &tree;
    const std::vector<std::string> &segments = key.segments();
    for (std::size_t index = 0; index < segments.size(); ++index)
    {
        if (!current->is<ConfigTable>())
        {
            *current = ConfigValue{ConfigTable{}};
        }
        ConfigTable *table = current->getIf<ConfigTable>();
        const std::string &segment = segments[index];
        if (index + 1 == segments.size())
        {
            (*table)[segment] = std::move(value);
            return;
        }
        current = &(*table)[segment];
    }
}
}
