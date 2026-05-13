#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ProductBuilder/ProductBuilderFactory.h"

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
    productBuilder = ProductBuilderFactory::create();

    if (!productBuilder)
        return;

    // Transfer ownership to Qt's object tree to prevent a memory leak.
    productBuilder->setParent(this);

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

QJsonObject MainWindow::getInstance(QJsonObject params)
{
    return productLineAPI.getInstance(params);
}

APIFunctionMap MainWindow::getAPIFunctionMap()
{
    APIFunctionMap apiFunctionMap;

    apiFunctionMap.insert("getFeature1", BIND_API_FUNC(&MainWindow::getFeature1));
    apiFunctionMap.insert("getInstance", BIND_API_FUNC(&MainWindow::getInstance));

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
