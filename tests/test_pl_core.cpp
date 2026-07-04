#include <gtest/gtest.h>
#include <pl_core/sharedassets.h>

TEST(SharedAssetsTest, Initialization) {
    SharedAssets assets;
    // SharedAssets is a simple value type — construction must not throw
    SUCCEED();
}
