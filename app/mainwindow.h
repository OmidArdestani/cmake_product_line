#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <pl_core/sharedassets.h>
#include <ProductBuilder/IProductBuilder.h>
#include "ProductLineAPI/grpcapiserver.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionRun_API_triggered();

private:
    void buildProduct();

private:
    Ui::MainWindow *ui;
    IProductBuilder* productBuilder{ nullptr };
    GrpcApiServer    apiServer;
    QMap<QString, IPLAsset*> productAssetMap{};
};
#endif // MAINWINDOW_H
