/*
* @file      jomock.h
* @brief     This file defines functions and classes supporting mock for static/non-virtual mehtod of c++ class.
*            This is an open source with MIT license deployed in https://github.com/jonah512/jomock
* @author    Josh Cho(hyugrae.cho@gmail.com, hyugrae.cho@samsung.com)
* @date      06/Jan/2022
*
*/
#pragma once

#include <unordered_map>
#include <vector>
#include <functional>

#ifndef NON_WIN32_SUPPORT
#include <Windows.h>
#include <memoryapi.h>
#else 
#include <list>
#include <cerrno>
#include <unistd.h>
#include <sys/mman.h>
#include <memory>
#endif


using namespace std;

#define JOMOCK(function) *::jomock::MockerCreator::getJoMock<::jomock::TypeForUniqMocker<__COUNTER__>>(function, #function)
#define JOMOCK_POLY(mocker, className, functionPoint, functionName, ret, args) ret(className::*functionPoint)args = &className::functionName;\
                                            auto mocker = &JOMOCK(functionPoint);
#define JOMOCK_POLY_S(mocker, functionPoint, functionName, ret, args) ret(*functionPoint)args = &functionName;\
                                            auto mocker = &JOMOCK(functionPoint);
#define CLEAR_JOMOCK ::jomock::MockerCreator::restoreAll
#define JOMOCK_FUNC stubFunc
#define JOMOCK_FUNC_1(function) stubFunc(function, ::testing::_)
#define JOMOCK_FUNC_2(function) stubFunc(function, ::testing::_, ::testing::_)
#define JOMOCK_FUNC_3(function) stubFunc(function, ::testing::_, ::testing::_, ::testing::_)
#define JOMOCK_FUNC_4(function) stubFunc(function, ::testing::_, ::testing::_, ::testing::_, ::testing::_)
#define JOMOCK_FUNC_5(function) stubFunc(function, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)
#define JOMOCK_FUNC_6(function) stubFunc(function, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)
#define JOMOCK_FUNC_7(function) stubFunc(function, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)
#define JOMOCK_FUNC_8(function) stubFunc(function, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)

#define JOMOCK_ARG_1 stubFunc( ::testing::_)
#define JOMOCK_ARG_2 stubFunc( ::testing::_, ::testing::_)
#define JOMOCK_ARG_3 stubFunc( ::testing::_, ::testing::_, ::testing::_)
#define JOMOCK_ARG_4 stubFunc( ::testing::_, ::testing::_, ::testing::_, ::testing::_)
#define JOMOCK_ARG_5 stubFunc( ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)
#define JOMOCK_ARG_6 stubFunc( ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)
#define JOMOCK_ARG_7 stubFunc( ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)
#define JOMOCK_ARG_8 stubFunc( ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)
#define JOMOCK_ARG_9 stubFunc( ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)
#define JOMOCK_ARG_10 stubFunc( ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)
#define JOMOCK_ARG_11 stubFunc( ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)
#define JOMOCK_ARG_12 stubFunc( ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)

namespace jomock {

    template < int uniq >
    struct TypeForUniqMocker { };

    template < typename T >
    struct JoMockBase { };

    template < typename T >
    struct JoMock : public JoMockBase<T> { };

    template < typename T >
    struct MockerEntryPoint { };

    template < typename T >
    struct SingletonBase {
        static T& getInstance() {
            static T value;
            return value;
        }
    };

    struct JoMockPatch {
        template < typename F1, typename F2 >
        static void graftFunction(F1 address, F2 destination, std::vector<char>& binary_backup) {
            void* function = reinterpret_cast<void*>((std::size_t&)address);
            if (!unprotectMemoryForOnePage(function)) {
                std::abort();
            }
            else {
                setJump(function, reinterpret_cast<void*>((std::size_t&)destination), binary_backup);
            }
        }

        template < typename F >
        static void _restore(F address, const std::vector<char>& binary_backup) {
            revertJump(reinterpret_cast<void*>((std::size_t&)address), binary_backup);
        }

        static std::size_t alignAddress(const std::size_t address, const std::size_t page_size) {
            return address & (~(page_size - 1));
        }

        static void backupBinary(const char* const function, std::vector<char>& binary_backup, const std::size_t size) {
            binary_backup = std::vector<char>(function, function + size);
        }

        static bool isDistanceOverflow(const std::size_t distance) {
            if(distance > INT32_MAX) return true;
            if(distance < ((long long)INT32_MIN)) return true;
            return false;
        }

        static std::size_t calculateDistance(const void* const address, const void* const destination) {
        	#ifndef ARM64_SUPPORT
            std::size_t distance = reinterpret_cast<std::size_t>(destination)
                - reinterpret_cast<std::size_t>(address) - 5; // For jmp instruction;
            return distance;
            #else
            std::size_t distance = reinterpret_cast<std::size_t>(destination)- reinterpret_cast<std::size_t>(address);
            return ((distance>>2) & 0x03FFFFFF);
            #endif

        }

        static void patchFunctionShortDistance(char* const function, const std::size_t distance) {
            const char* const distance_bytes = reinterpret_cast<const char*>(&distance);
            function[0] = (char)0xE9; // jmp
            std::copy(distance_bytes, distance_bytes + 4, function + 1);
        }

        static void patchFunctionLongAddress(char* const function, const void* const destination) {
            const char* const distance_bytes = reinterpret_cast<const char*>(&destination);
            function[0] = 0x68; // push
            std::copy(distance_bytes, distance_bytes + 4, function + 1);
            function[5] = (char)0xC7;
            function[6] = (char)0x44;
            function[7] = (char)0x24;
            function[8] = (char)0x04;
            std::copy(distance_bytes + 4, distance_bytes + 8, function + 9);
            function[13] = (char)0xC3; // ret
        }
		#ifdef ARM64_SUPPORT
        // Function to flush the instruction cache
        static void flushInstructionCache(void* start, void* end) {
            __builtin___clear_cache(start, end);
        }
        static std::size_t calculateDistanceArm64(const void* const address, const void* const destination) {
            std::size_t distance = reinterpret_cast<std::size_t>(destination)
                - reinterpret_cast<std::size_t>(address);
            return ((distance>>2) & 0x03FFFFFF);
        }
        static void patchFunctionArm64(uint * function, const std::size_t distance) {
            const uint32_t kBInstruction = 0x14000000;
            uint32_t instruction =  kBInstruction | distance;
            function[0] = instruction;
            flushInstructionCache((void*)function, (void*)(function + sizeof(uint32_t)));
        }
        #endif

        static void setJump(void* const address, const void* const destination, std::vector<char>& binary_backup) {
            char* const function = reinterpret_cast<char*>(address);
            #ifdef ARM64_SUPPORT
            std::size_t distance = calculateDistanceArm64(address, destination);
            backupBinary(function, binary_backup, 4); // 4 bytes : 1 instruction 3 data
            patchFunctionArm64((uint*)address, distance);
            #else
            std::size_t distance = calculateDistance(address, destination);
            if (isDistanceOverflow(distance)) {
                backupBinary(function, binary_backup, 14); // long jmp.
                patchFunctionLongAddress(function, destination);
            }
            else {
                backupBinary(function, binary_backup, 5); // short jmp.
                patchFunctionShortDistance(function, distance);
            }
            #endif
        }

        static void revertJump(void* address, const std::vector<char>& binary_backup) {
            std::copy(binary_backup.begin(), binary_backup.end(), reinterpret_cast<char*>(address));
        }

        static int unprotectMemory(const void* const address, const size_t length) {
            void* const page = reinterpret_cast<void*>(alignAddress(reinterpret_cast<std::size_t>(address), length));
#ifndef NON_WIN32_SUPPORT
            DWORD oldProtect;
            return VirtualProtect(page, length, PAGE_EXECUTE_READWRITE, &oldProtect);
#else
            int ret = mprotect(page, length, PROT_READ | PROT_WRITE | PROT_EXEC);
            if (ret != 0) return 0;
            else return 1;
#endif
        }

        static int unprotectMemoryForOnePage(void* const address) {
            static std::size_t pageSize = 0;
            if (pageSize == 0)
            {
#ifndef NON_WIN32_SUPPORT
                SYSTEM_INFO info;
                ::GetSystemInfo(&info);
                pageSize = info.dwPageSize;
#else
                pageSize = getpagesize();
#endif
            }
            return unprotectMemory(address, pageSize);
        }
    };

    template < typename I, typename R, typename ... P >
    struct MockerEntryPoint<I(R(P ...))> {
        typedef I IntegrateType(R(P ...));
        friend struct JoMock<IntegrateType>;
        static R EntryPoint(P... p) {
            return SingletonBase<JoMock<IntegrateType>*>::getInstance()->stubFunc(p ...);
        }
    };

    template < typename I, typename C, typename R, typename ... P >
    struct MockerEntryPoint<I(R(C::*)(P ...) const)> {
        typedef I IntegrateType(R(C::*)(const void*, P ...) const);
        friend struct JoMock<IntegrateType>; \
            R EntryPoint(P... p) {
            return SingletonBase<JoMock<IntegrateType>*>::getInstance()->stubFunc(this, p ...);
        }
    };

    template < typename I, typename C, typename R, typename ... P >
    struct MockerEntryPoint<I(R(C::*)(P ...))> {
        typedef I IntegrateType(R(C::*)(const void*, P ...));
        friend struct JoMock<IntegrateType>; \
            R EntryPoint(P... p) {
            return SingletonBase<JoMock<IntegrateType>*>::getInstance()->stubFunc(this, p ...);
        }
    };

    template < typename R, typename ... P >
    struct JoMockBase<R(P ...)> {
        JoMockBase(const string& _functionName) : functionName(_functionName) {}
        virtual ~JoMockBase() {}

        R stubFunc(P... p) {
            gmocker.SetOwnerAndName(this, functionName.c_str());
            return gmocker.Invoke(p ...);
        }

        ::testing::MockSpec<R(P...)> gmock_stubFunc(const ::testing::Matcher<P>&... p) {
            gmocker.RegisterOwner(this);
            return gmocker.With(p ...);
        }

        virtual void restore() = 0;

        mutable ::testing::FunctionMocker<R(P...)> gmocker;
        vector<char> binaryBackup; // Backup the mockee's binary code changed in RuntimePatcher.
        const string functionName;
    };

    template < typename I, typename R, typename ... P>
    struct JoMock<I(R(P ...))> : JoMockBase<R(P ...)> {
        typedef I IntegrateType(R(P ...));
        typedef R FunctionType(P ...);
        JoMock(FunctionType function, const string& functionName) :
            JoMockBase<FunctionType>(functionName),
            originFunction(function) {
            SingletonBase<decltype(this)>::getInstance() = this;
            JoMockPatch::graftFunction(originFunction,
                MockerEntryPoint<IntegrateType>::EntryPoint,
                JoMockBase<FunctionType>::binaryBackup);
        }

        virtual ~JoMock() {
            restore();
        }

        void restore() {
            JoMockPatch::_restore(originFunction, JoMockBase<FunctionType>::binaryBackup);
            SingletonBase<decltype(this)>::getInstance() = nullptr;
        }

        FunctionType* originFunction;
    };

    template < typename I, typename C, typename R, typename ... P>
    struct JoMock<I(R(C::*)(const void*, P ...) const)> : JoMockBase<R(const void*, P ...)> {
        typedef I IntegrateType(R(C::*)(const void*, P ...) const);
        typedef I EntryPointType(R(C::*)(P ...) const);
        typedef R(C::* FunctionType)(P ...) const;
        typedef R StubFunctionType(const void*, P ...);
        JoMock(FunctionType function, const string& functionName) :
            JoMockBase<StubFunctionType>(functionName),
            originFunction(function) {
            SingletonBase<decltype(this)>::getInstance() = this;
            JoMockPatch::graftFunction(originFunction,
                &MockerEntryPoint<EntryPointType>::EntryPoint,
                JoMockBase<StubFunctionType>::binaryBackup);
        }
        virtual ~JoMock() {
            restore();
        }
        virtual void restore() {
            JoMockPatch::_restore(originFunction, JoMockBase<StubFunctionType>::binaryBackup);
            SingletonBase<decltype(this)>::getInstance() = nullptr;
        }
        FunctionType originFunction;
    };

    template < typename I, typename C, typename R, typename ... P>
    struct JoMock<I(R(C::*)(const void*, P ...))> : JoMockBase<R(const void*, P ...)> {
        typedef I IntegrateType(R(C::*)(const void*, P ...));
        typedef I EntryPointType(R(C::*)(P ...));
        typedef R(C::* FunctionType)(P ...);
        typedef R StubFunctionType(const void*, P ...);
        JoMock(FunctionType function, const string& functionName) :
            JoMockBase<StubFunctionType>(functionName),
            originFunction(function) {
            SingletonBase<decltype(this)>::getInstance() = this;
            JoMockPatch::graftFunction(originFunction,
                &MockerEntryPoint<EntryPointType>::EntryPoint,
                JoMockBase<StubFunctionType>::binaryBackup);
        }
        virtual ~JoMock() {
            restore();
        }
        virtual void restore() {
            JoMockPatch::_restore(originFunction, JoMockBase<StubFunctionType>::binaryBackup);
            SingletonBase<decltype(this)>::getInstance() = nullptr;
        }
        FunctionType originFunction;
    };

    template < typename T >
    struct JoMockCache {
    private:
        friend struct MockerCreator;
        typedef unordered_map<const void*, const shared_ptr<T>> HashMap;

        static HashMap& getInstance() {
            return SingletonBase<HashMap>::getInstance();
        }

        static void restoreCachedMockFunction() {
            for (auto& mocker : getInstance()) {
                mocker.second->restore();
            }
            getInstance().clear();
        }
    };

    struct MockerCreator {
    private:
        typedef list<function<void()>> mockFunctions;

        template < typename I, typename R, typename ... P >
        static const shared_ptr<JoMockBase<R(P ...)>> createJoMock(R function(P ...), const string& functionName) {
            return shared_ptr<JoMockBase<R(P ...)>>(new JoMock<I(R(P ...))>(function, functionName));
        }

        template < typename I, typename C, typename R, typename ... P >
        static const shared_ptr<JoMockBase<R(const void*, P ...)>> createJoMock(R(C::* function)(P ...) const, const string& functionName) {
            typedef I IntegrateType(R(C::*)(const void*, P ...) const);
            return shared_ptr<JoMockBase<R(const void*, P ...)>>(new JoMock<IntegrateType>(function, functionName));
        };

        template < typename I, typename C, typename R, typename ... P >
        static const shared_ptr<JoMockBase<R(const void*, P ...)>> createJoMock(R(C::* function)(P ...), const string& functionName) {
            typedef I IntegrateType(R(C::*)(const void*, P ...));
            return shared_ptr<JoMockBase<R(const void*, P ...)>>(new JoMock<IntegrateType>(function, functionName)); \
        };

        template < typename I, typename M, typename F >
        static const shared_ptr<M> getJoMocker(F function, const string& functionName) {
            typedef JoMockCache<M> JoMockCacheType;
            const void* address = reinterpret_cast<const void*>((size_t&)function);
            auto got = JoMockCacheType::getInstance().find(address);
            if (got != JoMockCacheType::getInstance().end()) {
                return got->second;
            }
            else {
                SingletonBase<mockFunctions>::getInstance().push_back(bind(JoMockCacheType::restoreCachedMockFunction));
                JoMockCacheType::getInstance().insert({ {address, createJoMock<I>(function, functionName)} });
                return getJoMocker<I, M>(function, functionName);
            }
        }

    public:
        template < typename I, typename R, typename ... P >
        static const shared_ptr<JoMockBase<R(P ...)>> getJoMock(R function(P ...), const string& functionName) {
            return getJoMocker<I, JoMockBase<R(P ...)>>(function, functionName);
        }

        template < typename I, typename C, typename R, typename ... P >
        static shared_ptr<JoMockBase<R(const void*, P ...)>> getJoMock(R(C::* function)(P ...) const, const string& functionName) {
            return getJoMocker<I, JoMockBase<R(const void*, P ...)>>(function, functionName);
        };

        template < typename I, typename C, typename R, typename ... P >
        static shared_ptr<JoMockBase<R(const void*, P ...)>> getJoMock(R(C::* function)(P ...), const string& functionName) {
            return getJoMocker<I, JoMockBase<R(const void*, P ...)>>(function, functionName);
        };

        static void restoreAll() {
            for (auto& restorer : SingletonBase<mockFunctions>::getInstance()) {
                restorer();
            }
            SingletonBase<mockFunctions>::getInstance().clear();
        }
    };
}

