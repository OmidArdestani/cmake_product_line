#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <pl_core/sharedassets.h>
#include <ProductBuilder/IProductBuilder.h>
#include "ProductLineAPI/grpcapiserver.h"
#include "Menus/MenuIds.h"
#include <memory>

namespace app { class ToolController; }

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr, quint16 apiPort = 50051);
    ~MainWindow();

protected:
    void changeEvent(QEvent* event) override;

private:
    void buildProduct();
    void buildMenus();

private:
    Ui::MainWindow *ui;
    std::unique_ptr<IProductBuilder> productBuilder;
    GrpcApiServer    apiServer;
    QMap<QString, IPLAsset*> productAssetMap{};
    std::unique_ptr<app::ApplicationMenuSystem> menus;
    std::unique_ptr<app::ToolController> tools;
};
#endif // MAINWINDOW_H
