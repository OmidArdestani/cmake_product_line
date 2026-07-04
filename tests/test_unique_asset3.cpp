#include <gtest/gtest.h>
#include <uniqueasset3.h>

TEST(UniqueAsset3Test, InitiallyDisabled) {
    UniqueAsset3 asset;
    EXPECT_FALSE(asset.isEnable());
}

TEST(UniqueAsset3Test, EnableDisable) {
    UniqueAsset3 asset;
    asset.enableAsset(true);
    EXPECT_TRUE(asset.isEnable());
    asset.enableAsset(false);
    EXPECT_FALSE(asset.isEnable());
}

TEST(UniqueAsset3Test, ExceptionFunctionThrows) {
    UniqueAsset3 asset;
    EXPECT_THROW(asset.exceptionFunction(), std::runtime_error);
}
