// End-to-end test of the per-asset gRPC services over a real localhost
// channel (ephemeral port). Covers: enabled call, disabled call
// (FAILED_PRECONDITION), and loopback payload integrity.
#include <gtest/gtest.h>

#include <grpcpp/grpcpp.h>

#include <uniqueasset1.h>
#include <uniqueasset2.h>

#include <unique_asset1.grpc.pb.h>
#include <unique_asset2.grpc.pb.h>

namespace {

class GrpcApiTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        int port = 0;
        grpc::ServerBuilder builder;
        builder.AddListeningPort("127.0.0.1:0", grpc::InsecureServerCredentials(), &port);
        builder.RegisterService(asset1.grpcService());
        builder.RegisterService(asset2.grpcService());
        server = builder.BuildAndStart();
        ASSERT_NE(server, nullptr);
        ASSERT_GT(port, 0);

        channel = grpc::CreateChannel("127.0.0.1:" + std::to_string(port),
                                      grpc::InsecureChannelCredentials());
    }

    void TearDown() override
    {
        server->Shutdown();
    }

    UniqueAsset1 asset1;
    UniqueAsset2 asset2;
    std::unique_ptr<grpc::Server> server;
    std::shared_ptr<grpc::Channel> channel;
};

TEST_F(GrpcApiTest, RunFeatureEchoesParamsWhenEnabled)
{
    asset1.enableAsset(true);

    auto stub = plapi::UniqueAsset1Service::NewStub(channel);

    plapi::Asset1RunFeatureRequest request;
    auto* param = request.add_params();
    param->set_key("input");
    param->set_value("42");

    plapi::Asset1RunFeatureReply reply;
    grpc::ClientContext context;
    grpc::Status status = stub->RunFeature(&context, request, &reply);

    ASSERT_TRUE(status.ok()) << status.error_message();
    EXPECT_EQ(reply.status().code(), 0);
    EXPECT_FALSE(reply.date_time().empty());
    ASSERT_EQ(reply.loopback_size(), 1);
    EXPECT_EQ(reply.loopback(0).key(), "input");
    EXPECT_EQ(reply.loopback(0).value(), "42");
}

TEST_F(GrpcApiTest, RunFeatureRejectedWhenDisabled)
{
    // asset1 stays disabled (default)
    auto stub = plapi::UniqueAsset1Service::NewStub(channel);

    plapi::Asset1RunFeatureRequest request;
    plapi::Asset1RunFeatureReply reply;
    grpc::ClientContext context;
    grpc::Status status = stub->RunFeature(&context, request, &reply);

    EXPECT_EQ(status.error_code(), grpc::StatusCode::FAILED_PRECONDITION);
}

TEST_F(GrpcApiTest, Asset2RunFeatureSucceedsWhenEnabled)
{
    asset2.enableAsset(true);

    auto stub = plapi::UniqueAsset2Service::NewStub(channel);

    plapi::Asset2RunFeatureRequest request;
    plapi::Asset2RunFeatureReply reply;
    grpc::ClientContext context;
    grpc::Status status = stub->RunFeature(&context, request, &reply);

    ASSERT_TRUE(status.ok()) << status.error_message();
    EXPECT_EQ(reply.status().code(), 0);
}

TEST_F(GrpcApiTest, AssetNamesAreStable)
{
    EXPECT_EQ(asset1.assetName(), QStringLiteral("UniqueAsset1"));
    EXPECT_EQ(asset2.assetName(), QStringLiteral("UniqueAsset2"));
}

} // namespace
