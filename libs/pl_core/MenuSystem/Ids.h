#pragma once

#include <QString>
#include <type_traits>
#include <utility>

namespace pl::gui {

// One enum per module and ID category. Labels and translated text are never IDs.
template<class Tag>
class ScopedId
{
public:
    ScopedId() = default;

    template<class Enum, std::enable_if_t<std::is_enum_v<Enum>, int> = 0>
    ScopedId(QString module, Enum value)
        : module_(std::move(module)), value_(static_cast<qint64>(value)) {}

    bool isValid() const { return !module_.isEmpty(); }
    QString name() const { return module_ + QLatin1Char('/') + QString::number(value_); }

    friend bool operator==(const ScopedId& a, const ScopedId& b)
    { return a.module_ == b.module_ && a.value_ == b.value_; }
    friend bool operator<(const ScopedId& a, const ScopedId& b)
    { return a.module_ == b.module_ ? a.value_ < b.value_ : a.module_ < b.module_; }

private:
    QString module_;
    qint64 value_ = 0;
};

using MenuKey = ScopedId<struct MenuTag>;
using ActionKey = ScopedId<struct ActionTag>;
using IconKey = ScopedId<struct IconTag>;

} // namespace pl::gui
