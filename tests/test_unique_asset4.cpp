#include <gtest/gtest.h>
#include <uniqueasset4.h>

TEST(UniqueAsset4Test, InitiallyDisabled) {
    UniqueAsset4 asset;
    EXPECT_FALSE(asset.isEnable());
}

TEST(UniqueAsset4Test, EnableDisable) {
    UniqueAsset4 asset;
    asset.enableAsset(true);
    EXPECT_TRUE(asset.isEnable());
    asset.enableAsset(false);
    EXPECT_FALSE(asset.isEnable());
}

TEST(UniqueAsset4Test, ExceptionFunctionThrows) {
    UniqueAsset4 asset;
    EXPECT_THROW(asset.exceptionFunction(), std::runtime_error);
}
