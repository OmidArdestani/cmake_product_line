#include "ToolController.h"
#include "ProductLineAPI/grpcapiserver.h"
#include <QCoreApplication>
#include <stdexcept>

#ifdef UAsset1
#include <uniqueasset1.h>
#include <unique_asset1.pb.h>
#endif
#ifdef UAsset2
#include <uniqueasset2.h>
#include <unique_asset2.pb.h>
#endif
#ifdef UAsset3
#include <uniqueasset3.h>
#include <unique_asset3.pb.h>
#endif
#ifdef UAsset4
#include <uniqueasset4.h>
#include <unique_asset4.pb.h>
#endif

namespace app {
namespace {
QString text(const char* source) { return QCoreApplication::translate("ToolController", source); }

template<class Asset, class Request, class Reply>
FacilityTool facility(const QMap<QString, IPLAsset*>& assets, const QString& name, MenuId menu, ToolId enabled, ToolId run)
{
    auto* asset = dynamic_cast<Asset*>(assets.value(name));
    if (!asset)
        throw std::logic_error((QStringLiteral("Missing facility: ") + name).toStdString());
    return {name, menu, enabled, run,
        asset, [asset] {
            // These sample facilities accept an empty request. Future commands
            // collect their input here, without coupling the asset to widgets.
            Request request;
            Reply reply;
            asset->runFeature(request, reply);
            if (reply.status().code() != 0)
                throw std::runtime_error("Facility returned an unsuccessful status");
        }};
}
} // namespace

ToolController::ToolController(ApplicationMenuSystem& menus,
                               GrpcApiServer& api, const QMap<QString, IPLAsset*>& assets,
                               Reporter reporter, QObject* parent)
    : QObject(parent), menus_(menus), api_(api), report_(std::move(reporter))
{
#ifdef UAsset1
    facilities_.append(facility<UniqueAsset1, plapi::Asset1RunFeatureRequest, plapi::Asset1RunFeatureReply>(
        assets, QStringLiteral("UniqueAsset1"), MenuId::Asset1, ToolId::EnableAsset1, ToolId::RunAsset1));
#endif
#ifdef UAsset2
    facilities_.append(facility<UniqueAsset2, plapi::Asset2RunFeatureRequest, plapi::Asset2RunFeatureReply>(
        assets, QStringLiteral("UniqueAsset2"), MenuId::Asset2, ToolId::EnableAsset2, ToolId::RunAsset2));
#endif
#ifdef UAsset3
    facilities_.append(facility<UniqueAsset3, plapi::Asset3RunFeatureRequest, plapi::Asset3RunFeatureReply>(
        assets, QStringLiteral("UniqueAsset3"), MenuId::Asset3, ToolId::EnableAsset3, ToolId::RunAsset3));
#endif
#ifdef UAsset4
    facilities_.append(facility<UniqueAsset4, plapi::Asset4RunFeatureRequest, plapi::Asset4RunFeatureReply>(
        assets, QStringLiteral("UniqueAsset4"), MenuId::Asset4, ToolId::EnableAsset4, ToolId::RunAsset4));
#endif
}

void ToolController::toggleApi()
{
    if (api_.isRunning()) {
        api_.stop();
        report_(text(QT_TRANSLATE_NOOP("ToolController", "API stopped.")), false);
    } else if (api_.start()) {
        report_(text(QT_TRANSLATE_NOOP("ToolController", "API started.")), false);
    } else {
        report_(text(QT_TRANSLATE_NOOP("ToolController", "Could not start the API. Check whether its port is already in use.")), true);
    }
    syncApi();
}

void ToolController::syncApi()
{
    if (auto* action = menus_.action(ToolId::ToggleApi)) {
        const bool running = api_.isRunning();
        action->setText(running ? text(QT_TRANSLATE_NOOP("ToolController", "Stop API"))
                                : text(QT_TRANSLATE_NOOP("ToolController", "Run API")));
        action->setIcon(menus_.icon(running ? IconId::Stop : IconId::Run));
    }
}

void ToolController::setFacilityEnabled(const FacilityTool& facility, bool enabled)
{
    facility.asset->enableAsset(enabled);
    syncFacility(facility);
}

void ToolController::syncFacility(const FacilityTool& facility)
{
    const bool enabled = facility.asset->isEnable();
    if (auto* action = menus_.action(facility.enabledAction))
        action->setChecked(enabled);
    if (auto* action = menus_.action(facility.runAction))
        action->setEnabled(enabled);
}

void ToolController::runFacility(const FacilityTool& facility)
{
    // State may have changed through gRPC since the menu was opened.
    syncFacility(facility);
    if (!facility.asset->isEnable()) {
        report_(text(QT_TRANSLATE_NOOP("ToolController", "%1 is disabled. Enable it before running.")).arg(facility.name), true);
        return;
    }
    try {
        facility.run();
        report_(text(QT_TRANSLATE_NOOP("ToolController", "%1 completed successfully.")).arg(facility.name), false);
    } catch (const std::exception& error) {
        report_(text(QT_TRANSLATE_NOOP("ToolController", "%1 failed: %2")).arg(facility.name, QString::fromUtf8(error.what())), true);
    }
}

} // namespace app
