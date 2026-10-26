#pragma once

#include "Menus/MenuIds.h"
#include <MenuSystem/MenuRegistry.h>
#include <MenuSystem/Icons/IconProvider.h>
#include <pl_core/sharedassets.h>
#include <functional>

class GrpcApiServer;

namespace app {

struct FacilityTool
{
    QString name;
    pl::gui::MenuKey menu;
    pl::gui::ActionKey enabledAction;
    pl::gui::ActionKey runAction;
    IPLAsset* asset = nullptr; // application-owned; outlives this controller
    std::function<void()> run;
};

class ToolController : public QObject
{
public:
    using Reporter = std::function<void(const QString&, bool error)>;
    ToolController(pl::gui::MenuRegistry& menus, pl::gui::IconProvider& icons,
                   GrpcApiServer& api, const QMap<QString, IPLAsset*>& assets,
                   Reporter reporter, QObject* parent = nullptr);

    const QVector<FacilityTool>& facilities() const { return facilities_; }
    void toggleApi();
    void syncApi();
    void setFacilityEnabled(const FacilityTool& facility, bool enabled);
    void syncFacility(const FacilityTool& facility);
    void runFacility(const FacilityTool& facility);

private:
    pl::gui::MenuRegistry& menus_;
    pl::gui::IconProvider& icons_;
    GrpcApiServer& api_;
    Reporter report_;
    QVector<FacilityTool> facilities_;
};

} // namespace app
