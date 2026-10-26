#include <QApplication>
#include <gtest/gtest.h>

int main(int argc, char** argv)
{
    // Also applies during CTest's PRE_TEST discovery.
    if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORM"))
        qputenv("QT_QPA_PLATFORM", "offscreen");
    QApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
