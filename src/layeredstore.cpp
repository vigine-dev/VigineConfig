#include "vigine/config/layeredstore.h"

#include "vigine/config/configtree.h"

#include <utility>

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
    const ConfigTable *overlayTable = overlay.getIfTable();
    ConfigTable *baseTable          = base.getIfTable();
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
} // namespace

bool LayeredStore::booleanOr(const ConfigKey &key, bool fallback) const
{
    const ConfigValue *found = find(key);
    return found != nullptr ? found->booleanOr(fallback) : fallback;
}

std::int64_t LayeredStore::integerOr(const ConfigKey &key, std::int64_t fallback) const
{
    const ConfigValue *found = find(key);
    return found != nullptr ? found->integerOr(fallback) : fallback;
}

double LayeredStore::floatingOr(const ConfigKey &key, double fallback) const
{
    const ConfigValue *found = find(key);
    return found != nullptr ? found->floatingOr(fallback) : fallback;
}

std::string LayeredStore::stringOr(const ConfigKey &key, std::string fallback) const
{
    const ConfigValue *found = find(key);
    return found != nullptr ? found->stringOr(std::move(fallback)) : fallback;
}

void LayeredStore::pushLayer(ConfigValue tree) { _layers.push_back(std::move(tree)); }

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
} // namespace vigine::config
