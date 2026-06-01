#include "vigine/config/layeredstore.h"

#include <utility>

#include "vigine/config/configtree.h"

namespace vigine::config
{
namespace
{
void mergeInto(ConfigValue &base, const ConfigValue &overlay)
{
    if (overlay.isNone())
    {
        return;
    }
    const ConfigTable *overlayTable = overlay.getIf<ConfigTable>();
    ConfigTable *baseTable = base.getIf<ConfigTable>();
    if (overlayTable == nullptr || baseTable == nullptr)
    {
        base = overlay;
        return;
    }
    for (const auto &[key, value] : *overlayTable)
    {
        mergeInto((*baseTable)[key], value);
    }
}
}

void LayeredStore::pushLayer(ConfigValue tree)
{
    _layers.push_back(std::move(tree));
}

void LayeredStore::setLayer(std::size_t index, ConfigValue tree)
{
    if (index >= _layers.size())
    {
        _layers.resize(index + 1);
    }
    _layers[index] = std::move(tree);
}

const ConfigValue *LayeredStore::find(const ConfigKey &key) const
{
    for (auto layer = _layers.rbegin(); layer != _layers.rend(); ++layer)
    {
        const ConfigValue *found = findValue(*layer, key);
        if (found != nullptr && !found->isNone())
        {
            return found;
        }
    }
    return nullptr;
}

ConfigValue LayeredStore::flatten() const
{
    ConfigValue result{ConfigTable{}};
    for (const ConfigValue &layer : _layers)
    {
        mergeInto(result, layer);
    }
    return result;
}
}
