// Tencent is pleased to support the open source community by making RapidJSON available.
//
// Copyright (C) 2015 THL A29 Limited, a Tencent company, and Milo Yip. All rights reserved.
//
// Licensed under the MIT License (the "License"); you may not use this file except
// in compliance with the License. You may obtain a copy of the License at
//
// http://opensource.org/licenses/MIT
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.

#ifndef RAPIDJSON_GETTER_H_
#define RAPIDJSON_GETTER_H_

#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <stdexcept>
#include <limits>
#include <rapidjson/document.h>
#include <rapidjson/pointer.h>

namespace rapidjson {

class ValueError : public std::exception {
};
namespace internal {
//供转换用

template<typename Ch, typename Number>
struct ChToNumberHelper {
};

template<>
struct ChToNumberHelper<char, long> {

    static long
    To(const char* str, char** endptr)
    {
        return strtol(str, endptr, 0);
    }
};

template<>
struct ChToNumberHelper<char, long long> {

    static long long
    To(const char* str, char** endptr)
    {
        return strtoll(str, endptr, 0);
    }
};

template<>
struct ChToNumberHelper<char, unsigned long> {

    static unsigned long
    To(const char* str, char** endptr)
    {
        return strtoul(str, endptr, 0);
    }
};

template<>
struct ChToNumberHelper<char, unsigned long long> {

    static unsigned long long
    To(const char* str, char** endptr)
    {
        return strtoull(str, endptr, 0);
    }
};

template<>
struct ChToNumberHelper<char, float> {

    static float
    To(const char* str, char** endptr)
    {
        return strtof(str, endptr);
    }
};

template<>
struct ChToNumberHelper<char, double> {

    static double
    To(const char* str, char** endptr)
    {
        return strtod(str, endptr);
    }
};

template<>
struct ChToNumberHelper<wchar_t, long> {

    static long
    To(const wchar_t* str, wchar_t** endptr)
    {
        return wcstol(str, endptr, 0);
    }
};

template<>
struct ChToNumberHelper<wchar_t, long long> {

    static long long
    To(const wchar_t* str, wchar_t** endptr)
    {
        return wcstoll(str, endptr, 0);
    }
};

template<>
struct ChToNumberHelper<wchar_t, unsigned long> {

    static unsigned long
    To(const wchar_t* str, wchar_t** endptr)
    {
        return wcstoul(str, endptr, 0);
    }
};

template<>
struct ChToNumberHelper<wchar_t, unsigned long long> {

    static unsigned long long
    To(const wchar_t* str, wchar_t** endptr)
    {
        return wcstoull(str, endptr, 0);
    }
};

template<>
struct ChToNumberHelper<wchar_t, float> {

    static float
    To(const wchar_t* str, wchar_t** endptr)
    {
        return wcstof(str, endptr);
    }
};

template<>
struct ChToNumberHelper<wchar_t, double> {

    static double
    To(const wchar_t* str, wchar_t** endptr)
    {
        return wcstod(str, endptr);
    }
};

template<typename Number>
struct NumberHelper {
};

template<>
struct NumberHelper<int> {
    typedef long ResultType;
};

template<>
struct NumberHelper<int64_t> {
    typedef long long ResultType;
};

template<>
struct NumberHelper<unsigned> {
    typedef unsigned long ResultType;
};

template<>
struct NumberHelper<uint64_t> {
    typedef unsigned long long ResultType;
};

template<>
struct NumberHelper<float> {
    typedef float ResultType;
};

template<>
struct NumberHelper<double> {
    typedef double ResultType;
};

template<typename Number, typename Exception, typename Ch>
Number
StrTo(const Ch* s)
{
    Ch* end = NULL;
    const typename NumberHelper<Number>::ResultType
        v = ChToNumberHelper<Ch, typename NumberHelper<Number>::ResultType>::To(
        s, &end);
    if (end == s || *end != 0)
        throw Exception();
    if (std::numeric_limits<typename NumberHelper<Number>::ResultType>::has_infinity) {
        if (errno == ERANGE &&
            (v == std::numeric_limits<typename NumberHelper<Number>::ResultType>::infinity() ||
                v == -std::numeric_limits<typename NumberHelper<Number>::ResultType>::infinity()))
            throw Exception();
    }
    else {
        if (errno == ERANGE && (v == std::numeric_limits<typename NumberHelper<Number>::ResultType>::max() ||
            v == std::numeric_limits<typename NumberHelper<Number>::ResultType>::min()))
            throw Exception();
        if (v < std::numeric_limits<Number>::min() ||
            v > std::numeric_limits<Number>::max())
            throw Exception();
    }
    return v;
}
}
#define RAPIDJSON_GETTER(type, typeName) \
template<typename T>\
type \
Get##typeName##ByPointer(const T& root,\
        const GenericPointer<typename T::ValueType>& pointer,\
        size_t* unresolvedTokenIndex = 0)\
{\
    const typename T::ValueType* v = pointer.Get(root, unresolvedTokenIndex);\
    if (v==NULL)\
        throw ValueError();\
    return Get##typeName(*v);\
}\
template<typename T, typename CharType, size_t N>\
type \
Get##typeName##ByPointer(const T& root,\
        const CharType(& source)[N],\
        size_t* unresolvedTokenIndex = 0)\
{\
    const typename T::ValueType* v = GenericPointer<typename T::ValueType>(source, N-1).Get(root, unresolvedTokenIndex);\
    if (v==NULL)\
        throw ValueError();\
    return Get##typeName(*v);\
}\
template<typename T>\
type \
Get##typeName##ByPointerWithDefault(const T& root,\
        const GenericPointer<typename T::ValueType>& pointer,\
        type defaultValue)\
{\
    try {\
        return Get##typeName##ByPointer(root, pointer);\
    }\
    catch (const ValueError& e) {\
        return defaultValue;\
    }\
}\
template<typename T, typename CharType, size_t N>\
type \
Get##typeName##ByPointerWithDefault(const T& root,\
        const CharType(& source)[N],\
        type defaultValue)\
{\
    try {\
        return Get##typeName##ByPointer(root, source);\
    }\
    catch (const ValueError& e) {\
        return defaultValue;\
    }\
}

RAPIDJSON_GETTER(bool, Bool)
RAPIDJSON_GETTER(int, Int)
RAPIDJSON_GETTER(unsigned, Uint)
RAPIDJSON_GETTER(int64_t, Int64)
RAPIDJSON_GETTER(uint64_t, Uint64)
RAPIDJSON_GETTER(float, Float)
RAPIDJSON_GETTER(double, Double)
RAPIDJSON_GETTER(std::basic_string<typename T::Ch>, String)
#undef RAPIDJSON_GETTER
#ifndef RAPIDJSON_GETTER_DEFAULT_FLAGS
#define RAPIDJSON_GETTER_DEFAULT_FLAGS kGetterNoFlags
#endif
enum GetterFlag {
    kGetterNoFlags = 0,
    kGetterNullAsZero = 1, //treat null value as zero and false
    kGetterNumberAsBool = 2, //number can be bool
    kGetterBoolStringAsBool = 4, //treat "true" "false" as bool
    kGetterStringAsBool = 8, //treat not empty string as true empty string as false
    kGetterDefaultFlags = RAPIDJSON_GETTER_DEFAULT_FLAGS
};

template<unsigned GetterFlags, typename ValueType>
bool
GetBool(const ValueType& v)
{
    switch (v.GetType()) {
        case kNullType:
            if (GetterFlags & kGetterNullAsZero)return 0;
        case kObjectType:
        case kArrayType:throw ValueError();
        case kNumberType:
            if (GetterFlags & kGetterNumberAsBool) {
                //对于bool支持整型转换bool
                if (v.IsInt64())
                    return v.GetInt64();
                if (v.IsUint64())
                    return v.GetUint64();
                if (v.IsDouble())
                    return v.GetDouble();
            }
            throw ValueError();
        case kStringType:
            if (GetterFlags & kGetterBoolStringAsBool) {
                if (strncmp(v.GetString(), "true", v.GetStringLength()) == 0 ||
                    strncmp(v.GetString(), "True", v.GetStringLength()) == 0)
                    return true;
                if (strncmp(v.GetString(), "false", v.GetStringLength()) == 0 ||
                    strncmp(v.GetString(), "False", v.GetStringLength()) == 0)
                    return false;
            }
            if (GetterFlags & kGetterStringAsBool) {
                return v.GetStringLength();
            }
            throw ValueError();
        case kTrueType:
        case kFalseType:return v.GetBool();
    }
}

template<unsigned GetterFlags, typename ValueType>
int
GetInt(const ValueType& v)
{
    switch (v.GetType()) {
        case kNullType:if (GetterFlags & kGetterNullAsZero)return 0;
        case kObjectType:
        case kArrayType:throw ValueError();
        case kNumberType:
            if (v.IsInt())
                return v.GetInt();
            throw ValueError();
        case kStringType:return internal::StrTo<int, ValueError>(v.GetString());
        case kTrueType:
        case kFalseType:return v.GetBool();
    }
}

template<unsigned GetterFlags, typename ValueType>
unsigned
GetUint(const ValueType& v)
{
    switch (v.GetType()) {
        case kNullType:if (GetterFlags & kGetterNullAsZero)return 0;
        case kObjectType:
        case kArrayType:throw ValueError();
        case kNumberType:
            if (v.IsUint())
                return v.GetUint();
            throw ValueError();
        case kStringType:return internal::StrTo<unsigned, ValueError>(v.GetString());
        case kTrueType:
        case kFalseType:return v.GetBool();
    }
}

template<unsigned GetterFlags, typename ValueType>
int64_t
GetInt64(const ValueType& v)
{
    switch (v.GetType()) {
        case kNullType:if (GetterFlags & kGetterNullAsZero)return 0;
        case kObjectType:
        case kArrayType:throw ValueError();
        case kNumberType:
            if (v.IsInt64())
                return v.GetInt64();
            throw ValueError();
        case kStringType:return internal::StrTo<int64_t, ValueError>(v.GetString());
        case kTrueType:
        case kFalseType:return v.GetBool();
    }
}

template<unsigned GetterFlags, typename ValueType>
uint64_t
GetUint64(const ValueType& v)
{
    switch (v.GetType()) {
        case kNullType:if (GetterFlags & kGetterNullAsZero)return 0;
        case kObjectType:
        case kArrayType:throw ValueError();
        case kNumberType:
            if (v.IsUint64())
                return v.GetUint64();
            throw ValueError();
        case kStringType:return internal::StrTo<uint64_t, ValueError>(v.GetString());
        case kTrueType:
        case kFalseType:return v.GetBool();
    }
}

template<unsigned GetterFlags, typename ValueType>
double
GetDouble(const ValueType& v)
{
    switch (v.GetType()) {
        case kNullType:if (GetterFlags & kGetterNullAsZero)return 0;
        case kObjectType:
        case kArrayType:
        case kTrueType:
        case kFalseType:throw ValueError();
        case kNumberType:
            if (v.IsLosslessDouble() || v.IsDouble())
                return v.GetDouble();
            throw ValueError();
        case kStringType:return internal::StrTo<double, ValueError>(v.GetString());
    }
}

template<unsigned GetterFlags, typename ValueType>
float
GetFloat(const ValueType& v)
{
    switch (v.GetType()) {
        case kNullType:if (GetterFlags & kGetterNullAsZero)return 0;
        case kObjectType:
        case kArrayType:
        case kTrueType:
        case kFalseType:throw ValueError();
        case kNumberType:
            if (v.IsLosslessFloat() || v.IsFloat())
                return v.GetFloat();
            throw ValueError();
        case kStringType:return internal::StrTo<float, ValueError>(v.GetString());
    }
}

template<unsigned GetterFlags, typename ValueType>
std::basic_string<typename ValueType::Ch>
GetString(const ValueType &v)
{
    switch (v.GetType()) {
        case kNullType:return "null";
        case kObjectType:
        case kArrayType:throw ValueError();
        case kTrueType:return "true";
        case kFalseType:return "false";
        case kNumberType:
        {
            char buf[1024];
            if (v.IsInt())
                snprintf(buf, sizeof (buf), "%d", v.GetInt());
            if (v.IsUint())
                snprintf(buf, sizeof (buf), "%u", v.GetUint());
            if (v.IsInt64())
                snprintf(buf, sizeof (buf), "%" PRIi64, v.GetInt64());
            else if (v.IsUint64())
                snprintf(buf, sizeof (buf), "%" PRIu64, v.GetUint64());
            else if (v.IsFloat())
                snprintf(buf, sizeof (buf), "%f", v.GetFloat());
            else if (v.IsDouble())
                snprintf(buf, sizeof (buf), "%f", v.GetDouble());
            else
                throw ValueError();
            return buf;
        }
        case kStringType:return std::basic_string<typename ValueType::Ch > (v.GetString(), v.GetStringLength());
    }
}
#define RAPIDJSON_GETTER(type, typeName) \
template<typename ValueType>\
type \
Get##typeName(const ValueType& v)\
{\
    return Get##typeName<kGetterDefaultFlags>(v);\
}
RAPIDJSON_GETTER(bool, Bool)
RAPIDJSON_GETTER(int, Int)
RAPIDJSON_GETTER(unsigned, Uint)
RAPIDJSON_GETTER(int64_t, Int64)
RAPIDJSON_GETTER(uint64_t, Uint64)
RAPIDJSON_GETTER(float, Float)
RAPIDJSON_GETTER(double, Double)
RAPIDJSON_GETTER(std::basic_string<typename ValueType::Ch>, String)
#undef RAPIDJSON_GETTER
}

#endif //RAPIDJSON_GETTER_H_
