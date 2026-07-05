#include <gtest/gtest.h>
#include <uniqueasset2.h>

TEST(UniqueAsset2Test, InitiallyDisabled) {
    UniqueAsset2 asset;
    EXPECT_FALSE(asset.isEnable());
}

TEST(UniqueAsset2Test, EnableDisable) {
    UniqueAsset2 asset;
    asset.enableAsset(true);
    EXPECT_TRUE(asset.isEnable());
    asset.enableAsset(false);
    EXPECT_FALSE(asset.isEnable());
}

TEST(UniqueAsset2Test, ExceptionFunctionThrows) {
    UniqueAsset2 asset;
    EXPECT_THROW(asset.exceptionFunction(), std::runtime_error);
}
