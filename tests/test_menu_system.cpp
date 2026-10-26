#include <MenuSystem/MenuRegistry.h>
#include <MenuSystem/Icons/IconProvider.h>
#include <QApplication>
#include <QFile>
#include <QPainter>
#include <QTemporaryDir>
#include <QTranslator>
#include <gtest/gtest.h>
#include <memory>

using namespace pl::gui;

namespace {
enum class Menu { Root = 1, Nested = 2, Other = 3 };
enum class Command { First = 1, Second = 2, Third = 3 };
enum class Symbol { First = 1, Second = 2, Bad = 3 };
MenuKey key(Menu value) { return {QStringLiteral("tests"), value}; }
ActionKey key(Command value) { return {QStringLiteral("tests"), value}; }
IconKey key(Symbol value) { return {QStringLiteral("tests"), value}; }

class MenuTest : public ::testing::Test
{
protected:
    QMenuBar bar;
    MenuRegistry menus{&bar};
    QObject receiver;

    void SetUp() override
    {
        ASSERT_TRUE(menus.addMenu({key(Menu::Root), {}, {"Test", "Tools"}}));
    }

    ActionDefinition command(Command id, int order = 0)
    {
        ActionDefinition def;
        def.id = key(id);
        def.text = {"Test", "Execute"};
        def.order = order;
        return def;
    }
};

TEST_F(MenuTest, NestedMenusHideWhenTheirLastCommandIsHidden)
{
    ASSERT_TRUE(menus.addMenu({key(Menu::Nested), key(Menu::Root), {"Test", "Nested"}}));
    EXPECT_FALSE(menus.menu(key(Menu::Root))->menuAction()->isVisible());
    ASSERT_TRUE(menus.addSeparator(key(Menu::Nested)));
    EXPECT_FALSE(menus.menu(key(Menu::Nested))->menuAction()->isVisible());
    ASSERT_TRUE(menus.addAction(key(Menu::Nested), command(Command::First), &receiver, [](bool) {}));
    EXPECT_TRUE(menus.menu(key(Menu::Root))->menuAction()->isVisible());
    auto* action = menus.action(key(Command::First));
    action->setEnabled(false); // disabled commands still count as content
    EXPECT_TRUE(menus.menu(key(Menu::Root))->menuAction()->isVisible());
    action->setVisible(false);
    EXPECT_FALSE(menus.menu(key(Menu::Root))->menuAction()->isVisible());
    action->setVisible(true);
    EXPECT_TRUE(menus.setMenuVisible(key(Menu::Nested), false));
    EXPECT_FALSE(menus.menu(key(Menu::Root))->menuAction()->isVisible());
    EXPECT_TRUE(menus.setMenuVisible(key(Menu::Nested), true));
    EXPECT_TRUE(menus.menu(key(Menu::Root))->menuAction()->isVisible());
}

TEST_F(MenuTest, OrdersActionsAndSeparatorsStably)
{
    ASSERT_TRUE(menus.addAction(key(Menu::Root), command(Command::Third, 30), &receiver, [](bool) {}));
    ASSERT_TRUE(menus.addAction(key(Menu::Root), command(Command::First, 10), &receiver, [](bool) {}));
    ASSERT_TRUE(menus.addSeparator(key(Menu::Root), 20));
    ASSERT_TRUE(menus.addAction(key(Menu::Root), command(Command::Second, 10), &receiver, [](bool) {}));
    const auto entries = menus.menu(key(Menu::Root))->actions();
    ASSERT_EQ(entries.size(), 4);
    EXPECT_EQ(entries[0], menus.action(key(Command::First)));
    EXPECT_EQ(entries[1], menus.action(key(Command::Second)));
    EXPECT_TRUE(entries[2]->isSeparator());
    EXPECT_EQ(entries[3], menus.action(key(Command::Third)));
}

TEST_F(MenuTest, RejectsDuplicatesAndMissingParentsWithoutPartialRegistration)
{
    EXPECT_FALSE(menus.addMenu({key(Menu::Root), {}, {{}, "Duplicate"}}));
    EXPECT_FALSE(menus.addMenu({key(Menu::Nested), key(Menu::Other), {{}, "Orphan"}}));
    EXPECT_EQ(menus.menu(key(Menu::Nested)), nullptr);
    EXPECT_FALSE(menus.addAction(key(Menu::Other), command(Command::First), &receiver, [](bool) {}));
    EXPECT_EQ(menus.action(key(Command::First)), nullptr);
    EXPECT_FALSE(menus.addAction(key(Menu::Root), command(Command::First), nullptr, [](bool) {}));
    ASSERT_TRUE(menus.addAction(key(Menu::Root), command(Command::First), &receiver, [](bool) {}));
    EXPECT_FALSE(menus.addAction(key(Menu::Root), command(Command::First), &receiver, [](bool) {}));
    EXPECT_FALSE(menus.addSeparator(key(Menu::Other)));
    EXPECT_FALSE(menus.attachAction(key(Menu::Root), key(Command::First)));
    EXPECT_FALSE(menus.attachAction(key(Menu::Other), key(Command::First)));
}

TEST_F(MenuTest, ModulesCanReuseEnumValues)
{
    const MenuKey otherModule{QStringLiteral("another.product"), Menu::Root};
    ASSERT_TRUE(menus.addMenu({otherModule, {}, {{}, "Another product"}}));
    EXPECT_NE(menus.menu(otherModule), menus.menu(key(Menu::Root)));
    auto def = command(Command::First);
    ASSERT_TRUE(menus.addAction(key(Menu::Root), def, &receiver, [](bool) {}));
    def.id = {QStringLiteral("another.product"), Command::First};
    EXPECT_TRUE(menus.addAction(otherModule, def, &receiver, [](bool) {}));
    static_assert(!std::is_convertible_v<ActionKey, MenuKey>);
    static_assert(!std::is_constructible_v<ActionKey, QString, int>);
}

TEST_F(MenuTest, DispatchDoesNotDependOnLabelsAndSharesActionState)
{
    int invocations = 0;
    auto def = command(Command::First);
    def.checkable = true;
    def.shortcut = QKeySequence(Qt::CTRL | Qt::Key_R);
    ASSERT_TRUE(menus.addAction(key(Menu::Root), def, &receiver, [&](bool checked) {
        EXPECT_TRUE(checked);
        ++invocations;
    }));
    ASSERT_TRUE(menus.addMenu({key(Menu::Other), {}, {{}, "Other"}}));
    ASSERT_TRUE(menus.attachAction(key(Menu::Other), def.id));
    auto* action = menus.action(def.id);
    action->setText(QStringLiteral("Completely different label"));
    action->trigger();
    EXPECT_EQ(invocations, 1);
    EXPECT_TRUE(menus.menu(key(Menu::Other))->actions().first()->isChecked());
    EXPECT_EQ(action->shortcut(), def.shortcut);
    action->setEnabled(false);
    action->trigger();
    EXPECT_EQ(invocations, 1);
}

TEST_F(MenuTest, DestroyedHandlerContextDisablesAction)
{
    auto context = std::make_unique<QObject>();
    int calls = 0;
    ASSERT_TRUE(menus.addAction(key(Menu::Root), command(Command::First), context.get(), [&](bool) { ++calls; }));
    context.reset();
    auto* action = menus.action(key(Command::First));
    EXPECT_FALSE(action->isEnabled());
    action->trigger();
    EXPECT_EQ(calls, 0);
}

TEST_F(MenuTest, CanRetranslateWithoutChangingIdentityOrConnections)
{
    class Translator : public QTranslator {
    public:
        bool isEmpty() const override { return false; }
        QString translate(const char* context, const char* source, const char*, int) const override
        { return QByteArray(context) == "Test" ? QStringLiteral("Translated ") + QString::fromUtf8(source) : QString(); }
    } translator;
    int calls = 0;
    auto translatedCommand = command(Command::First);
    translatedCommand.toolTip = {"Test", "Use %1", {QStringLiteral("asset")}};
    ASSERT_TRUE(menus.addAction(key(Menu::Root), translatedCommand, &receiver, [&](bool) { ++calls; }));
    QCoreApplication::installTranslator(&translator);
    menus.retranslate();
    QCoreApplication::removeTranslator(&translator);
    EXPECT_EQ(menus.menu(key(Menu::Root))->title(), QStringLiteral("Translated Tools"));
    auto* action = menus.action(key(Command::First));
    EXPECT_EQ(action->text(), QStringLiteral("Translated Execute"));
    EXPECT_EQ(action->toolTip(), QStringLiteral("Translated Use asset"));
    action->trigger();
    EXPECT_EQ(calls, 1);
}

TEST(MenuOwnership, RegistryCleansMenusAndActionsAndToleratesWindowDestruction)
{
    QObject owner;
    auto bar = std::make_unique<QMenuBar>();
    auto registry = std::make_unique<MenuRegistry>(bar.get(), &owner);
    ASSERT_TRUE(registry->addMenu({key(Menu::Root), {}, {{}, "Tools"}}));
    QPointer<QMenu> menu = registry->menu(key(Menu::Root));
    registry.reset();
    EXPECT_TRUE(menu.isNull());
    EXPECT_TRUE(bar->actions().isEmpty());

    registry = std::make_unique<MenuRegistry>(bar.get(), &owner);
    ASSERT_TRUE(registry->addMenu({key(Menu::Root), {}, {{}, "Tools"}}));
    bar.reset();
    EXPECT_EQ(registry->menu(key(Menu::Root)), nullptr);
    EXPECT_FALSE(registry->addMenu({key(Menu::Other), {}, {{}, "Other"}}));
    registry.reset();
}

class IconTest : public ::testing::Test
{
protected:
    QTemporaryDir directory;
    IconProvider icons;
    QString path(const char* name) { return directory.filePath(QString::fromUtf8(name)); }
    void save(const QString& filename, QSize size, QColor color)
    {
        QImage img(size, QImage::Format_ARGB32);
        img.fill(color);
        ASSERT_TRUE(img.save(filename));
    }
};

TEST_F(IconTest, CropsZeroBasedRowsAndColumnsWithMarginsSpacingAndAlpha)
{
    IconSheet geometry;
    geometry.cellSize = QSize(4, 4);
    geometry.margins = QMargins(1, 2, 3, 4);
    geometry.spacing = QSize(2, 3);
    QImage sheet(14, 17, QImage::Format_ARGB32);
    sheet.fill(Qt::transparent);
    QPainter painter(&sheet);
    painter.fillRect(QRect(7, 9, 4, 4), QColor(255, 0, 0, 128)); // row 1, column 1
    painter.end();
    ASSERT_TRUE(sheet.save(path("sheet.png")));
    ASSERT_TRUE(icons.registerSheet(path("sheet.png"), geometry));
    ASSERT_TRUE(icons.registerIcon(key(Symbol::First), IconSource::sprite(path("sheet.png"), 1, 1)));
    const auto icon = icons.icon(key(Symbol::First));
    ASSERT_FALSE(icon.isNull());
    const auto crop = icon.pixmap(4, 4).toImage();
    EXPECT_EQ(crop.pixelColor(0, 0), QColor(255, 0, 0, 128));
    EXPECT_EQ(crop.pixelColor(3, 3), QColor(255, 0, 0, 128));
    EXPECT_EQ(icons.icon(key(Symbol::First)).cacheKey(), icon.cacheKey());
    EXPECT_FALSE(icons.fromSource(IconSource::sprite(path("sheet.png"), 0, 0)).isNull());
    EXPECT_EQ(icons.fromSource(IconSource::sprite(path("sheet.png"), 0, 0)).pixmap(4, 4).toImage().pixelColor(0, 0).alpha(), 0);
}

TEST_F(IconTest, RejectsInvalidSheetsAndCoordinates)
{
    EXPECT_FALSE(icons.registerSheet(path("sheet.png"), {}));
    IconSheet sheet;
    sheet.cellSize = QSize(4, 4);
    save(path("sheet.png"), QSize(8, 8), Qt::red);
    ASSERT_TRUE(icons.registerSheet(path("sheet.png"), sheet));
    EXPECT_FALSE(icons.registerSheet(path("sheet.png"), sheet));
    EXPECT_TRUE(icons.fromSource(IconSource::sprite(path("sheet.png"), -1, 0)).isNull());
    EXPECT_TRUE(icons.fromSource(IconSource::sprite(path("sheet.png"), 0, 2)).isNull());
    EXPECT_TRUE(icons.fromSource(IconSource::sprite(path("sheet.png"), 2, 0)).isNull());
    EXPECT_TRUE(icons.fromSource(IconSource::sprite(path("unknown.png"), 0, 0)).isNull());
    save(path("bad-grid.png"), QSize(9, 8), Qt::red);
    ASSERT_TRUE(icons.registerSheet(path("bad-grid.png"), sheet));
    EXPECT_TRUE(icons.fromSource(IconSource::sprite(path("bad-grid.png"), 0, 0)).isNull());
    EXPECT_TRUE(icons.fromSource(IconSource::file(path("missing.svg"))).isNull());
    EXPECT_TRUE(icons.icon(key(Symbol::Bad)).isNull());
}

TEST_F(IconTest, SupportsHighDpiSheetsAndExplicitModesAndStates)
{
    save(path("sheet.png"), QSize(4, 4), Qt::red);
    save(path("sheet@2x.png"), QSize(8, 8), Qt::blue);
    save(path("disabled.png"), QSize(4, 4), Qt::green);
    save(path("on.png"), QSize(4, 4), Qt::yellow);
    IconSheet sheet;
    sheet.cellSize = QSize(4, 4);
    sheet.highDpiFiles.insert(2, path("sheet@2x.png"));
    ASSERT_TRUE(icons.registerSheet(path("sheet.png"), sheet));
    ASSERT_TRUE(icons.registerIcon(key(Symbol::First), QVector<IconVariant>{
        {IconSource::sprite(path("sheet.png"), 0, 0)},
        {IconSource::file(path("disabled.png")), QIcon::Disabled},
        {IconSource::file(path("on.png")), QIcon::Normal, QIcon::On}
    }));
    const auto icon = icons.icon(key(Symbol::First));
    EXPECT_EQ(icon.pixmap(4, 4, QIcon::Disabled).toImage().pixelColor(0, 0), QColor(Qt::green));
    EXPECT_EQ(icon.pixmap(4, 4, QIcon::Normal, QIcon::On).toImage().pixelColor(0, 0), QColor(Qt::yellow));
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const auto highDpi = icon.pixmap(QSize(4, 4), 2.0);
    EXPECT_EQ(highDpi.size(), QSize(8, 8));
    EXPECT_EQ(highDpi.devicePixelRatio(), 2.0);
    EXPECT_EQ(highDpi.toImage().pixelColor(0, 0), QColor(Qt::blue));
#else
    EXPECT_TRUE(icon.availableSizes().contains(QSize(8, 8)));
#endif
}

TEST_F(IconTest, InvalidHighDpiVariantKeepsBaseAndCacheCanBeReloaded)
{
    save(path("sheet.png"), QSize(4, 4), Qt::red);
    save(path("bad@2x.png"), QSize(7, 8), Qt::blue);
    IconSheet sheet;
    sheet.cellSize = QSize(4, 4);
    sheet.highDpiFiles.insert(2, path("bad@2x.png"));
    ASSERT_TRUE(icons.registerSheet(path("sheet.png"), sheet));
    ASSERT_TRUE(icons.registerIcon(key(Symbol::First), IconSource::sprite(path("sheet.png"), 0, 0)));
    EXPECT_EQ(icons.icon(key(Symbol::First)).pixmap(4, 4).toImage().pixelColor(0, 0), QColor(Qt::red));
    save(path("sheet.png"), QSize(4, 4), Qt::green);
    EXPECT_EQ(icons.icon(key(Symbol::First)).pixmap(4, 4).toImage().pixelColor(0, 0), QColor(Qt::red));
    icons.clearCache();
    EXPECT_EQ(icons.icon(key(Symbol::First)).pixmap(4, 4).toImage().pixelColor(0, 0), QColor(Qt::green));
}

TEST_F(IconTest, SvgRemainsScalableAndExistingQIconsRetainIdentity)
{
    QFile svg(path("icon.svg"));
    ASSERT_TRUE(svg.open(QIODevice::WriteOnly));
    svg.write("<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 16 16\"><rect width=\"16\" height=\"16\" fill=\"red\"/></svg>");
    svg.close();
    ASSERT_TRUE(icons.registerIcon(key(Symbol::First), IconSource::file(svg.fileName())));
    auto icon = icons.icon(key(Symbol::First));
    ASSERT_FALSE(icon.isNull());
    EXPECT_EQ(icon.pixmap(64, 64).size(), QSize(64, 64));
    ASSERT_TRUE(icons.registerIcon(key(Symbol::Second), IconSource::fromIcon(icon)));
    EXPECT_EQ(icons.icon(key(Symbol::Second)).cacheKey(), icon.cacheKey());
    EXPECT_FALSE(icons.registerIcon(key(Symbol::First), IconSource::file(svg.fileName())));
    EXPECT_FALSE(icons.registerIcon(key(Symbol::Bad), QVector<IconVariant>{
        {IconSource::file(svg.fileName())}, {IconSource::file(svg.fileName())}}));
}
} // namespace
