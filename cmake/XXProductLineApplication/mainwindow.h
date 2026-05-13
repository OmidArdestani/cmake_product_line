#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <sharedassets.h>
#include <ProductBuilder/IProductBuilder.h>
#include "ProductLineAPI/productlineapi.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow, public IPLAsset
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionRun_API_triggered();

private:
    void buildProduct();

    // API Functions
private:
    QJsonObject getFeature1(QJsonObject params);
    QJsonObject getInstance(QJsonObject params);

private:
    Ui::MainWindow *ui;
    IProductBuilder* productBuilder{ nullptr };
    ProductLineAPI   productLineAPI;
    QMap<QString, IPLAsset*> productAssetMap{};

    // IPLAsset interface
public:
    void enableAsset(bool value) override{}
    bool isEnable() const override {return true;}
    APIFunctionMap getAPIFunctionMap() override;
};
#endif // MAINWINDOW_H
