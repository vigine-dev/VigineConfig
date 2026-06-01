#include "vigine/config/configkey.h"

#include <stdexcept>

namespace vigine::config
{
ConfigKey::ConfigKey(std::string_view dotted)
{
    if (dotted.empty())
    {
        return;
    }

    std::size_t start = 0;
    while (true)
    {
        const std::size_t dot = dotted.find('.', start);
        const std::size_t end = (dot == std::string_view::npos) ? dotted.size() : dot;
        if (end == start)
        {
            throw std::invalid_argument("ConfigKey: empty segment in \"" + std::string(dotted) + "\"");
        }
        _segments.emplace_back(dotted.substr(start, end - start));
        if (dot == std::string_view::npos)
        {
            break;
        }
        start = dot + 1;
    }
}

std::string ConfigKey::toString() const
{
    std::string out;
    for (std::size_t index = 0; index < _segments.size(); ++index)
    {
        if (index != 0)
        {
            out.push_back('.');
        }
        out += _segments[index];
    }
    return out;
}

ConfigKey ConfigKey::parent() const
{
    ConfigKey result;
    if (!_segments.empty())
    {
        result._segments.assign(_segments.begin(), _segments.end() - 1);
    }
    return result;
}

std::string_view ConfigKey::leaf() const noexcept
{
    if (_segments.empty())
    {
        return {};
    }
    return _segments.back();
}

ConfigKey &ConfigKey::append(std::string_view segment)
{
    _segments.emplace_back(segment);
    return *this;
}
}
