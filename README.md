# jomock
This is the most powerful c++ mocking tool on windows VC.
You can easily isolate your module from all other classes or functions for an unit test.

1. global function
2. static function in class
3. non virtual function in class

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

# environment
Windows SDK 10 + Platform SDK : Visual Studio 2019 v142 + 64bit
