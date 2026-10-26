#pragma once

#include "Menus/MenuIds.h"
#include <pl_core/sharedassets.h>
#include <functional>

class GrpcApiServer;

namespace app {

struct FacilityTool
{
    QString name;
    MenuId menu;
    ToolId enabledAction;
    ToolId runAction;
    IPLAsset* asset = nullptr; // application-owned; outlives this controller
    std::function<void()> run;
};

class ToolController : public QObject
{
public:
    using Reporter = std::function<void(const QString&, bool error)>;
    ToolController(ApplicationMenuSystem& menus,
                   GrpcApiServer& api, const QMap<QString, IPLAsset*>& assets,
                   Reporter reporter, QObject* parent = nullptr);

    const QVector<FacilityTool>& facilities() const { return facilities_; }
    void toggleApi();
    void syncApi();
    void setFacilityEnabled(const FacilityTool& facility, bool enabled);
    void syncFacility(const FacilityTool& facility);
    void runFacility(const FacilityTool& facility);

private:
    ApplicationMenuSystem& menus_;
    GrpcApiServer& api_;
    Reporter report_;
    QVector<FacilityTool> facilities_;
};

} // namespace app
