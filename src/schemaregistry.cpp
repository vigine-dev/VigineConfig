#include "vigine/config/schemaregistry.h"

#include "vigine/config/configtree.h"

#include <memory>
#include <sstream>
#include <utility>

namespace vigine::config
{
namespace
{
class SchemaRegistry final : public ISchemaRegistry
{
  public:
    void declare(ConfigKey key, ConfigType type, ConfigValue defaultValue) override
    {
        for (SchemaEntry &entry : _entries)
        {
            if (entry.key() == key)
            {
                entry = SchemaEntry{std::move(key), type, std::move(defaultValue)};
                return;
            }
        }
        _entries.emplace_back(std::move(key), type, std::move(defaultValue));
    }

    [[nodiscard]] const std::vector<SchemaEntry> &entries() const noexcept override
    {
        return _entries;
    }

    [[nodiscard]] ValidationResult validate(const ConfigValue &tree) const override
    {
        bool ok = true;
        std::vector<std::string> errors;
        ConfigValue resolved = tree.isTable() ? tree : ConfigValue{ConfigTable{}};

        for (const SchemaEntry &entry : _entries)
        {
            const ConfigValue *found = findValue(tree, entry.key());
            if (found == nullptr || found->isNone())
            {
                setValue(resolved, entry.key(), entry.defaultValue());
                continue;
            }
            if (found->type() != entry.type())
            {
                std::ostringstream stream;
                stream << "key \"" << entry.key().toString() << "\" has the wrong type";
                errors.push_back(stream.str());
                ok = false;
            }
        }
        return ValidationResult{ok, std::move(errors), std::move(resolved)};
    }

  private:
    std::vector<SchemaEntry> _entries;
};
} // namespace

std::unique_ptr<ISchemaRegistry> createSchemaRegistry()
{
    return std::make_unique<SchemaRegistry>();
}
} // namespace vigine::config
