//===========================================================================
// Trustwave ltd. @{SRCH}
//								Enum.h
//
//---------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
// An easy way to define an enum with a set of utilities to be converted
// into a string. Now, every time we want to define an enum type use the
// following macros. Please do not use the macros that start with _.
/*
 example:
 #define Food_LIST(m)   \
     m(Food, Apple, 1)  \
     m(Food, Banana, 2) \
     m(Food, Pear, 4)	  \
     m(Food, Grapes)
SmartEnum(Food_LIST)
*/
// This example creates the following
//    1) enum Food {Apple=1, Banana=2, Pear=4, Grapes};
//    2) static std::array<const char *,number_of_Food> {"", "Apple", "Banana","",  "Pear", "Grapes"};
//---------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Avi Lachmish
// Date    : 4/12/2017 12:30:40 PM
// Comments: First Issue
//===========================================================================
#ifndef COMMON_ENUM_H
#define COMMON_ENUM_H
//===========================================================================
//								Include Files.
//===========================================================================
#include <array>

// microsoft bug - That is, __VA_ARGS__ is expanded as a single argument,
// instead of being broken down to multiple ones.
#define EXPAND(x) x

// Define a macro that uses the "paired, sliding arg list"
// technique to select the appropriate override.
#define _GET_OVERRIDE(_1, _2, _3, NAME, ...) NAME

#define _ENUM_VALUE2(typeName, value) value,
#define _ENUM_VALUE3(typeName, value, val) value = val,
#define EnumValue(...) EXPAND(_GET_OVERRIDE(__VA_ARGS__, _ENUM_VALUE3, _ENUM_VALUE2)(__VA_ARGS__))

#define _SET_Array2(typeName, value) typeName##Array[value] = #value;
#define _SET_Array3(typeName, value, val) typeName##Array[val] = #value;
#define SET_Array(...) EXPAND(_GET_OVERRIDE(__VA_ARGS__, _SET_Array3, _SET_Array2)(__VA_ARGS__))

#define EnumDefineEnum(typeName, values) enum typeName { values(EnumValue) number_of_##typeName };
#define EnumDefineNames(typeName, values) \
    static std::array<const char*, number_of_##typeName> typeName##Array = {""}; \
    namespace { \
        struct Init##typeName { \
            Init##typeName() \
            { \
                typeName##Array.fill(""); \
                values(SET_Array) \
            } \
        }; \
        Init##typeName __##typeName##__; \
    }
#define EnumToString(typeName, value) typeName##Array[value]

#define SmartEnum(typeName) EnumDefineEnum(typeName, typeName##_LIST) EnumDefineNames(typeName, typeName##_LIST)

#endif // COMMON_ENUM_H