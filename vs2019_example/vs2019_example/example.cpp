#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "../../jomock/jomock.h"

#include <iostream>

using namespace ::std;
using namespace ::testing;

string func() {
    return "Non mocked.";
}

class ClassTest {
public:
    static int staticFunc() {
        return 1;
    }

    int nonStaticFunc() {
        return 2;
    }
};

TEST(JoMock, GlobalFunction) {

    EXPECT_CALL(JOMOCK(func), JOMOCK_FUNC())
        .Times(Exactly(1))
        .WillOnce(Return("Hello world."));

    EXPECT_EQ("Hello world.", func());


    EXPECT_CALL(JOMOCK(ClassTest::staticFunc), JOMOCK_FUNC())
        .Times(Exactly(1))
        .WillOnce(Return(1));

    EXPECT_EQ(ClassTest::staticFunc(), 1);
}

TEST(JoMock, StaticFunctionClass) {

    EXPECT_CALL(JOMOCK(ClassTest::staticFunc), JOMOCK_FUNC())
        .Times(Exactly(1))
        .WillOnce(Return(3));

    EXPECT_EQ(ClassTest::staticFunc(), 3);
}

TEST(JoMock, NonStaticFunctionClass) {

    ClassTest classTest;
    auto mocker = &JOMOCK(&ClassTest::nonStaticFunc);
    EXPECT_CALL(*mocker, JOMOCK_FUNC(&classTest))
        .Times(Exactly(1))
        .WillOnce(Return(4));
    EXPECT_EQ(classTest.nonStaticFunc(), 4);

    mocker->restore();
    EXPECT_EQ(classTest.nonStaticFunc(), 2);
}



int main(int argc, char* argv[])
{
    std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << endl;

    testing::InitGoogleTest(&argc, argv);

    RUN_ALL_TESTS();

    std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;

    return 0;
}

