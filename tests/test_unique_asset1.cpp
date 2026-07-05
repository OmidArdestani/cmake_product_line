#include <gtest/gtest.h>
#include <uniqueasset1.h>

TEST(UniqueAsset1Test, InitiallyDisabled) {
    UniqueAsset1 asset;
    EXPECT_FALSE(asset.isEnable());
}

TEST(UniqueAsset1Test, EnableDisable) {
    UniqueAsset1 asset;
    asset.enableAsset(true);
    EXPECT_TRUE(asset.isEnable());
    asset.enableAsset(false);
    EXPECT_FALSE(asset.isEnable());
}

TEST(UniqueAsset1Test, ExceptionFunctionThrows) {
    UniqueAsset1 asset;
    EXPECT_THROW(asset.exceptionFunction(), std::runtime_error);
}
