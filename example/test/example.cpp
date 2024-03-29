#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../../jomock/jomock.h"

#include <iostream>
using namespace ::std;
using namespace ::testing;
using testing::InitGoogleTest;


string func() {
    return "no mock";
}

int funcInt(int)
{
    return 100;
}

class ClassTest {
public:
    static int staticFunc()
    {
        return 1;
    }

    static std::shared_ptr<ClassTest> staticFuncSharedPtr()
    {
        return std::make_shared<ClassTest>();
    }

    int nonStaticFunc(int i)
    {
        return 2;
    }

    int nonStaticFuncOverload(char* c)
    {
        return 3;
    }

    int nonStaticFuncOverload(int j)
    {
        return 3;
    }

    static string parameterFunc(bool, char, string, const string&)
    {
        return "no mock";
    }

    static string referenceParameterFunc(bool& result, char&, string&, const string&)
    {
        return "no mock";
    }

    static void outputArgumentFunc(bool* result, int input)
    {
        *result = false;
    }
};

class JoMock : public ::testing::Test
{
public:


    virtual void SetUp()
    {
        CLEAR_JOMOCK();
    }
    virtual void TearDown()
    {
        CLEAR_JOMOCK();
    }
};

TEST_F(JoMock, GlobalFunction)
{

    EXPECT_CALL(JOMOCK(func), JOMOCK_FUNC())
        .Times(Exactly(1))
        .WillOnce(Return("global mock"));

    EXPECT_EQ("global mock", func());


    EXPECT_CALL(JOMOCK(ClassTest::staticFunc), JOMOCK_FUNC())
        .Times(Exactly(1))
        .WillOnce(Return(1));

    EXPECT_EQ(ClassTest::staticFunc(), 1);
}
/**/
TEST_F(JoMock, StaticFunctionClass)
{

    EXPECT_CALL(JOMOCK(ClassTest::staticFunc), JOMOCK_FUNC())
        .Times(Exactly(1))
        .WillOnce(Return(3));

    EXPECT_EQ(ClassTest::staticFunc(), 3);
}

TEST_F(JoMock, StaticFunctionClassSharedPointer)
{
    shared_ptr<ClassTest> test = std::make_shared<ClassTest>();

    EXPECT_CALL(JOMOCK(ClassTest::staticFuncSharedPtr), JOMOCK_FUNC())
        .WillRepeatedly(Return(test));

    EXPECT_EQ(ClassTest::staticFuncSharedPtr(), static_cast<shared_ptr<ClassTest>>(test));
}


TEST_F(JoMock, NonStaticPolyFunctionClass)
{
    ClassTest classTest;
    JOMOCK_POLY(mocker1, ClassTest, fn1, nonStaticFuncOverload, int, (int))
        EXPECT_CALL(*mocker1, JOMOCK_FUNC(&classTest, _))
        .Times(Exactly(1))
        .WillOnce(Return(4));
    EXPECT_EQ(classTest.nonStaticFuncOverload(2), 4);

    char* c = nullptr;
    JOMOCK_POLY(mocker2, ClassTest, fn2, nonStaticFuncOverload, int, (char*))
        EXPECT_CALL(*mocker2, JOMOCK_FUNC(&classTest, _))
        .Times(Exactly(1))
        .WillOnce(Return(5));
    EXPECT_EQ(classTest.nonStaticFuncOverload(c), 5);
}

TEST_F(JoMock, ParameterFunctionTest)
{
    auto funcBinded = bind(ClassTest::parameterFunc, true, 'c', "test", "");
    auto mocker = &JOMOCK(ClassTest::parameterFunc);
    EXPECT_CALL(*mocker, JOMOCK_FUNC(_, _, _, _))
        .Times(Exactly(1))
        .WillOnce(Return("mocked func"));
    EXPECT_EQ(funcBinded(), "mocked func");
}

TEST_F(JoMock, ReferenceParameterFunctionTest)
{
    bool b;
    char c;
    string s;
    const string cs;

    EXPECT_CALL(JOMOCK(ClassTest::referenceParameterFunc), JOMOCK_ARG_4)
        .Times(Exactly(1))
        .WillRepeatedly(Return("mocked func"));

    EXPECT_EQ(ClassTest::referenceParameterFunc(ref(b), ref(c), ref(s), ref(cs)), "mocked func");
}

TEST_F(JoMock, OutputArgumentFunctionTest)
{
    bool result = false;
    bool tmp = true;

    ClassTest::outputArgumentFunc(&result, 10);
    EXPECT_EQ(result, false);

    EXPECT_CALL(JOMOCK(ClassTest::outputArgumentFunc), JOMOCK_FUNC(_, _))
        .Times(Exactly(1))
        .WillOnce(SetArgPointee<0>(tmp));

    ClassTest::outputArgumentFunc(&result, 10);
    EXPECT_EQ(result, true);
}

TEST_F(JoMock, LogacyLibraryTest)
{
    EXPECT_CALL(JOMOCK(atoi), JOMOCK_FUNC(_))
        .Times(Exactly(1))
        .WillOnce(Return(1));
    EXPECT_EQ(atoi("TEN"), 1);

}

int main(int argc, char* argv[])
{
    std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << endl;

    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;

    return 0;
}
