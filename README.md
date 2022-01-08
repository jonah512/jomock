# why jomock?
The most difficult thing about unit testing in C++ is that only methods defined as virtual are mocked.
Because of this, it is often impossible to use mocks properly without modifying the implemented source code.
Also, static methods or non-virtual methods cannot be mocked.
Because of this cause, the class and object, which are the subject of unit test, cannot isolate from other surrounding objects.
There is a practical difficulty that only limited scenarios are unit tested.

This code provides a way to test all possible scenarios without modifying the source code by solving this problem.

## 1. global function
```c++
TEST(JoMock, GlobalFunction) 
{
    EXPECT_CALL(JOMOCK(func), JOMOCK_FUNC())
        .Times(Exactly(1))
        .WillOnce(Return("Hello world."));

    EXPECT_EQ("Hello world.", func());    


    EXPECT_CALL(JOMOCK(ClassTest::staticFunc), JOMOCK_FUNC())
        .Times(Exactly(1))
        .WillOnce(Return(1));

    EXPECT_EQ(ClassTest::staticFunc(), 1);
}
```

## 2. static function in class
```c++
TEST(JoMock, StaticFunctionClass) 
{

    EXPECT_CALL(JOMOCK(ClassTest::staticFunc), JOMOCK_FUNC())
        .Times(Exactly(1))
        .WillOnce(Return(3));

    EXPECT_EQ(ClassTest::staticFunc(), 3);
}
```

## 3. non virtual function in class
```c++
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
```

## 4. method with multiple parameters
```c++
TEST(JoMock, ParameterFunctionTest)
{
    auto funcBinded = bind(ClassTest::parameterFunc, true, 'c', "test", "");
    auto mocker = &JOMOCK(ClassTest::parameterFunc);
    EXPECT_CALL(*mocker, JOMOCK_FUNC(_, _, _, _))
        .Times(Exactly(1))
        .WillOnce(Return("mocked func"));
    EXPECT_EQ(funcBinded(), "mocked func");
}

TEST(JoMock, ReferenceParameterFunctionTest)
{
    bool b;
    char c;
    string s;
    const string cs;

    EXPECT_CALL(JOMOCK(ClassTest::referenceParameterFunc), JOMOCK_FUNC(_, _, _, _))
        .Times(Exactly(2))
        .WillRepeatedly(Return("mocked func"));

    EXPECT_EQ(ClassTest::referenceParameterFunc(ref(b), ref(c), ref(s), ref(cs)), "mocked func");
}
```

## 5. overload functions 
```c++

TEST(JoMock, NonStaticPolyFunctionClass) 
{
    ClassTest classTest;
    JOMOCK_POLY(mocker1/*mocker name*/, 
				ClassTest/*class name*/, 
				fn1/*unique name for function pointer*/, 
				nonStaticFuncOverload/*target function*/, 
				int/*return type of the target function*/, 
				(int)/*argument : *important to specify target function* */)
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

```

## 6. legacy library 
```c++
TEST(JoMock, LogacyLibraryTest)
{
    EXPECT_CALL(JOMOCK(atoi), JOMOCK_FUNC(_))
        .Times(Exactly(1))
        .WillOnce(Return(1));
    EXPECT_EQ(atoi("TEN"), 1);
}
```

## 7. clean up mocks
```c++
CLEAR_JOMOCK();
// or 
::jomock::MockerCreator::restoreAll();
```
# environment
## windows case
1. Windows SDK 10 + Platform SDK : Visual Studio 2019 v142
2. 32bit/64bit Release(without optimization option) work
3. 64bit Debug works
4. gtest and gmock are installed by NuGet
## cygwin case 
1. cygwin latest
2. install gtest
3. add '#define NON_WIN32'
## linux case
1. add '#define NON_WIN32'
# the original ref : 
CppFreeMock: https://github.com/gzc9047/CppFreeMock
