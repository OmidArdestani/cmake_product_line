#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Menus/ApplicationMenus.h"
#include "Menus/ApplicationIcons.h"
#include "Tools/ToolController.h"
#include <QDebug>
#include <QEvent>
#include <QStatusBar>
#include <stdexcept>

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

MainWindow::MainWindow(QWidget *parent, quint16 apiPort)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , apiServer(apiPort)
{
    ui->setupUi(this);

    buildProduct();
    buildMenus();
}

MainWindow::~MainWindow()
{
    // Stop workers before releasing their services and facilities.
    apiServer.stop();
    tools.reset();
    menus.reset();
    qDeleteAll(productAssetMap);
    delete ui;
}

void MainWindow::buildProduct()
{
#ifdef Product_1
    productBuilder = std::make_unique<Product1Builder>();
#endif

#ifdef Product_2
    productBuilder = std::make_unique<Product2Builder>();
#endif

#ifdef Product_3
    productBuilder = std::make_unique<Product3Builder>();
#endif

#ifdef Product_4
    productBuilder = std::make_unique<Product4Builder>();
#endif

#ifdef Product_5
    productBuilder = std::make_unique<Product5Builder>();
#endif

    productBuilder->registerFeatures(productAssetMap);

    // Expose every asset of the active product over gRPC.
    for (auto* asset : productAssetMap)
        apiServer.registerAsset(asset);
}

void MainWindow::buildMenus()
{
    menus = std::make_unique<app::ApplicationMenuSystem>(*this);
    tools = std::make_unique<app::ToolController>(*menus, apiServer, productAssetMap,
        [this](const QString& message, bool error) {
            statusBar()->showMessage(message, error ? 15000 : 5000);
            if (error)
                qWarning().noquote() << message;
            else
                qInfo().noquote() << message;
        });
    // Icons once, one ordered definition, one build for both GUI surfaces.
    if (!app::registerApplicationIcons(*menus)
        || !menus->build(app::applicationMenus(*this, *menus, *tools)))
        throw std::logic_error("Application menu registration failed");
    tools->syncApi();
}

void MainWindow::changeEvent(QEvent* event)
{
    QMainWindow::changeEvent(event);
    if (event->type() == QEvent::LanguageChange && menus && tools) {
        ui->retranslateUi(this);
        menus->retranslate();
        tools->syncApi();
    }
}
