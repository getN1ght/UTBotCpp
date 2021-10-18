/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2021. All rights reserved.
 */

#include "gtest/gtest.h"

#include "BaseTest.h"
#include "KleeGenerator.h"
#include "Server.h"
#include "TestUtils.h"

#include "utils/path/FileSystemPath.h"
#include <functional>

namespace {
    using grpc::Channel;
    using grpc::ClientContext;
    using testsgen::TestsGenService;
    using testsgen::TestsResponse;
    using testUtils::checkTestCasePredicates;
    using testUtils::createLineRequest;

    class Regression_Test : public BaseTest {
    protected:
        Regression_Test() : BaseTest("regression") {
        }

        void SetUp() override {
            clearEnv();
        }

        std::pair<FunctionTestGen, Status>
        createTestForFunction(const fs::path &pathToFile, int lineNum, bool verbose = true) {
            auto lineRequest = createLineRequest(projectName, suitePath, buildDirRelativePath,
                                                 srcPaths, pathToFile, lineNum, verbose);
            auto request = GrpcUtils::createFunctionRequest(std::move(lineRequest));
            auto testGen = FunctionTestGen(*request, writer.get(), TESTMODE);
            testGen.setTargetForSource(pathToFile);

            Status status = Server::TestsGenServiceImpl::ProcessBaseTestRequest(testGen, writer.get());
            return { testGen, status };
        }
    };

    // http://jira-msc.rnd.huawei.com/browse/SAT-372
    TEST_F(Regression_Test, SAT_372_Printf_Symbolic_Parameter) {
        fs::path helloworld_c = getTestFilePath("helloworld.c");

        auto [testGen, status] = createTestForFunction(helloworld_c, 14);

        ASSERT_TRUE(status.ok()) << status.error_message();

        checkTestCasePredicates(
            testGen.tests.at(helloworld_c).methods.begin().value().testCases,
            vector<TestCasePredicate>({ [](const tests::Tests::MethodTestCase &testCase) {
                int ret = stoi(testCase.returnValueView->getEntryValue());
                int param = stoi(testCase.paramValues[0].view->getEntryValue());
                return ret == param + 1;
            } }),
            "helloworld");
    }

    // http://jira-msc.rnd.huawei.com/browse/SAT-752
    TEST_F(Regression_Test, Null_Return) {
        fs::path source = getTestFilePath("SAT-752.c");

        for (bool verbose : { false, true }) {
            auto [testGen, status] = createTestForFunction(source, 11, verbose);

            ASSERT_TRUE(status.ok()) << status.error_message();

            checkTestCasePredicates(
                testGen.tests.at(source).methods.begin().value().testCases,
                vector<TestCasePredicate>({ [](const tests::Tests::MethodTestCase &testCase) {
                    return testCase.returnValueView->getEntryValue() == PrinterUtils::C_NULL;
                } }),
                "byword");
        }
    }

    // http://jira-msc.rnd.huawei.com/browse/SAT-760
    TEST_F(Regression_Test, Incomplete_Array_Type) {
        fs::path folderPath = suitePath / "SAT-760";
        auto projectRequest = testUtils::createProjectRequest(
            projectName, suitePath, buildDirRelativePath, { suitePath, folderPath });
        auto request = GrpcUtils::createFolderRequest(std::move(projectRequest), folderPath);
        auto testGen = FolderTestGen(*request, writer.get(), TESTMODE);
        testGen.setTargetForSource(testGen.testingMethodsSourcePaths[0]);

        fs::path source1 = folderPath / "SAT-760_1.c";
        fs::path source2 = folderPath / "SAT-760_2.c";
        Tests tests1 = testGen.tests.at(source1);
        Tests tests2 = testGen.tests.at(source2);
        testGen.tests.clear();
        testGen.tests[source1] = tests1;
        testGen.tests[source2] = tests2;
        // Reorder files in order to parse them in the fixed manner.

        Status status = Server::TestsGenServiceImpl::ProcessBaseTestRequest(testGen, writer.get());
        ASSERT_TRUE(status.ok()) << status.error_message();
        {
            checkTestCasePredicates(
                testGen.tests.at(source1).methods.begin().value().testCases,
                vector<TestCasePredicate>({ [](const tests::Tests::MethodTestCase &testCase) {
                  EXPECT_EQ(1, testCase.globalPreValues.size());
                  EXPECT_EQ(1, testCase.globalPostValues.size());
                  return true;
                } }),
                "write");
        }
        {
            checkTestCasePredicates(
                testGen.tests.at(source2).methods.begin().value().testCases,
                vector<TestCasePredicate>({ [](const tests::Tests::MethodTestCase &testCase) {
                    EXPECT_EQ(0, testCase.globalPreValues.size());
                    EXPECT_EQ(0, testCase.globalPostValues.size());
                    return true;
                } }),
                "write");
        }
    }

    // http://jira-msc.rnd.huawei.com/browse/SAT-766
    TEST_F(Regression_Test, Global_Char_Array) {
        fs::path source = getTestFilePath("SAT-766.c");

        auto [testGen, status] = createTestForFunction(source, 8);

        ASSERT_TRUE(status.ok()) << status.error_message();

        checkTestCasePredicates(
            testGen.tests.at(source).methods.begin().value().testCases,
            vector<TestCasePredicate>({ [](const tests::Tests::MethodTestCase &testCase) {
              EXPECT_EQ(1, testCase.globalPreValues.size());
              EXPECT_EQ(1, testCase.globalPostValues.size());
              return !testCase.isError();
            } }),
            "first");
    }

    // http://jira-msc.rnd.huawei.com/browse/SAT-767
    TEST_F(Regression_Test, Index_Out_Of_Bounds) {
        fs::path source = getTestFilePath("SAT-767.c");
        auto [testGen, status] = createTestForFunction(source, 12);

        ASSERT_TRUE(status.ok()) << status.error_message();


        checkTestCasePredicates(
            testGen.tests.at(source).methods.begin().value().testCases,
            vector<TestCasePredicate>(
                { [](const tests::Tests::MethodTestCase &testCase) {
                  EXPECT_EQ(1, testCase.globalPreValues.size());
                  EXPECT_EQ(1, testCase.globalPostValues.size());
                  return !testCase.isError();
                } }),
            "first");
    }

    // http://jira-msc.rnd.huawei.com/browse/SAT-777
    TEST_F(Regression_Test, Global_Array_Of_Pointers) {
        fs::path source = getTestFilePath("SAT-777.c");
        auto [testGen, status] = createTestForFunction(source, 9);

        ASSERT_TRUE(status.ok()) << status.error_message();


        checkTestCasePredicates(
            testGen.tests.at(source).methods.begin().value().testCases,
            vector<TestCasePredicate>(
                { [](const tests::Tests::MethodTestCase &testCase) {
                  EXPECT_EQ(1, testCase.globalPreValues.size());
                  EXPECT_EQ(1, testCase.globalPostValues.size());
                  return !testCase.isError();
                } }),
            "set_file_list");
    }
}