#include "mainwindow.h"
#include "ui_mainwindow.h"

#ifdef Product_1
#include "ProductBuilder/Product1Builder.h"
#endif

#ifdef Product_2
#include "ProductBuilder/Product2Builder.h"
#endif

#ifdef Product_3
#include "ProductBuilder/Product3Builder.h"
#endif

#ifdef Product_4
#include "ProductBuilder/Product4Builder.h"
#endif

#ifdef Product_5
#include "ProductBuilder/Product5Builder.h"
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , productLineAPI(this)
{
    ui->setupUi(this);

    buildProduct();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::buildProduct()
{
#ifdef Product_1
    productBuilder = new Product1Builder();
#endif

#ifdef Product_2
    productBuilder = new Product2Builder();
#endif

#ifdef Product_3
    productBuilder = new Product3Builder();
#endif

#ifdef Product_4
    productBuilder = new Product4Builder();
#endif

#ifdef Product_5
    productBuilder = new Product5Builder();
#endif

    productBuilder->registerFeatures(productAssetMap);
    productBuilder->buildMenu(menuBar());
    productBuilder->decorate(this->centralWidget());
}

QJsonObject MainWindow::getFeature1(QJsonObject)
{
    auto feature = productAssetMap.value("feature1");
    if(feature)
    {
        quint64 instanceId = reinterpret_cast<quintptr>(feature);
        productLineAPI.insertInstance(instanceId, feature);
        return QJsonObject{
            {"instanceId", QString::number(instanceId)},
            {"error", ""}
        };
    }

    return QJsonObject{
        {"error", "This feature is not exist!"}
    };
}

APIFunctionMap MainWindow::getAPIFunctionMap()
{
    APIFunctionMap apiFunctionMap;

    apiFunctionMap.insert("getFeature1", BIND_API_FUNC(&MainWindow::getFeature1));

    return apiFunctionMap;
}

void MainWindow::on_actionRun_API_triggered()
{
    if(ui->actionRun_API->text().toLower() == "run")
    {
        productLineAPI.start();
        ui->actionRun_API->setText("Stop");
    }
    else
    {
        productLineAPI.stop();
        ui->actionRun_API->setText("Run");
    }
}

