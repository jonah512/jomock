# why jomock?
The most difficult thing about unit testing in C++ is that only methods defined as virtual are mocked.
Because of this, it is often impossible to use mocks properly without modifying the implemented source code.
Also, static methods or non-virtual methods cannot be mocked.
Because of this cause, the class and object, which are the subject of unit test, cannot isolate from other surrounding objects.
There is a practical difficulty that only limited scenarios are unit tested.

This code provides a way to test all possible scenarios without modifying the source code by solving this problem.

This is the most powerful c++ mocking tool on windows VC.
You can easily isolate your module from all other classes or functions for an unit test.

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

## 1. global function
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

## 2. static function in class
TEST(JoMock, StaticFunctionClass) {

    EXPECT_CALL(JOMOCK(ClassTest::staticFunc), JOMOCK_FUNC())
        .Times(Exactly(1))
        .WillOnce(Return(3));

    EXPECT_EQ(ClassTest::staticFunc(), 3);
}

## 3. non virtual function in class
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
1. Windows SDK 10 + Platform SDK : Visual Studio 2019 v142
2. 32bit/64bit Release(without optimization option) work
3. 64bit Debug works

# original ref : CppFreeMock
https://github.com/gzc9047/CppFreeMock
