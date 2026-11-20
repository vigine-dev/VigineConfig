#include "vigine/config/layeredstore.h"

#include "vigine/config/configtree.h"

#include <memory>
#include <utility>
#include <vector>

namespace vigine::config
{
ILayeredStore::~ILayeredStore() = default;

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

class LayeredStore final : public ILayeredStore
{
  public:
    void pushLayer(ConfigValue tree) override { _layers.push_back(std::move(tree)); }

    void setLayer(std::size_t index, ConfigValue tree) override
    {
        if (index >= _layers.size())
        {
            _layers.resize(index + 1);
        }
        _layers[index] = std::move(tree);
    }

    [[nodiscard]] std::size_t layerCount() const noexcept override { return _layers.size(); }

    [[nodiscard]] const ConfigValue *find(const ConfigKey &key) const override
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

    [[nodiscard]] bool booleanOr(const ConfigKey &key, bool fallback) const override
    {
        const ConfigValue *found = find(key);
        return found != nullptr ? found->booleanOr(fallback) : fallback;
    }

    [[nodiscard]] std::int64_t integerOr(const ConfigKey &key, std::int64_t fallback) const override
    {
        const ConfigValue *found = find(key);
        return found != nullptr ? found->integerOr(fallback) : fallback;
    }

    [[nodiscard]] double floatingOr(const ConfigKey &key, double fallback) const override
    {
        const ConfigValue *found = find(key);
        return found != nullptr ? found->floatingOr(fallback) : fallback;
    }

    [[nodiscard]] std::string stringOr(const ConfigKey &key, std::string fallback) const override
    {
        const ConfigValue *found = find(key);
        return found != nullptr ? found->stringOr(std::move(fallback)) : fallback;
    }

    [[nodiscard]] ConfigValue flatten() const override
    {
        ConfigValue result{ConfigTable{}};
        for (const ConfigValue &layer : _layers)
        {
            mergeInto(result, layer);
        }
        return result;
    }

  private:
    std::vector<ConfigValue> _layers;
};
} // namespace

std::unique_ptr<ILayeredStore> createLayeredStore() { return std::make_unique<LayeredStore>(); }
} // namespace vigine::config
