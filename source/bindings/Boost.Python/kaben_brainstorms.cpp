
// Boost Includes ==============================================================
#include <boost/python.hpp>
#include <boost/cstdint.hpp>

// Includes ====================================================================
#include <string.hh>

// Using =======================================================================
using namespace boost::python;

// Declarations ================================================================
namespace  {

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(cString_Insert_overloads_1_3, Insert, 1, 3)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(cString_Clip_overloads_1_2, Clip, 1, 2)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(cString_Replace_overloads_2_3, Replace, 2, 3)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(cString_CountWhitespace_overloads_0_1, CountWhitespace, 0, 1)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(cString_CountWordsize_overloads_0_1, CountWordsize, 0, 1)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(cString_CountLinesize_overloads_0_1, CountLinesize, 0, 1)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(cString_GetWord_overloads_0_1, GetWord, 0, 1)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(cString_GetWordAt_overloads_0_1, GetWordAt, 0, 1)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(cString_Find_overloads_1_2, Find, 1, 2)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(cString_FindWord_overloads_1_2, FindWord, 1, 2)


}// namespace 


// Module ======================================================================
BOOST_PYTHON_MODULE(kaben_brainstorms)
{
    class_< cString >("cString", init< optional< const char* > >())
        .def(init< const int >())
        .def(init< const cString& >())
        .def_readonly("MAX_LENGTH", &cString::MAX_LENGTH)
        .def("GetData", &cString::GetData)
        .def("GetSize", &cString::GetSize)
        .def("Compare", &cString::Compare)
        .def("Set", &cString::Set)
        .def("Insert", (cString& (cString::*)(const char, int, int) )&cString::Insert, cString_Insert_overloads_1_3())
        .def("Insert", (cString& (cString::*)(const char*, int, int) )&cString::Insert, cString_Insert_overloads_1_3())
        .def("Insert", (cString& (cString::*)(const cString&, int, int) )&cString::Insert, cString_Insert_overloads_1_3())
        .def("Clip", &cString::Clip, cString_Clip_overloads_1_2())
        .def("ClipFront", &cString::ClipFront)
        .def("ClipEnd", &cString::ClipEnd)
        .def("Replace", &cString::Replace, cString_Replace_overloads_2_3())
        .def("Pop", &cString::Pop)
        .def("PopWord", &cString::PopWord)
        .def("PopLine", &cString::PopLine)
        .def("LeftJustify", &cString::LeftJustify)
        .def("RightJustify", &cString::RightJustify)
        .def("Reverse", &cString::Reverse)
        .def("ToLower", &cString::ToLower)
        .def("ToUpper", &cString::ToUpper)
        .def("CompressWhitespace", &cString::CompressWhitespace)
        .def("RemoveWhitespace", &cString::RemoveWhitespace)
        .def("RemoveChar", &cString::RemoveChar)
        .def("RemovePos", &cString::RemovePos)
        .def("AsInt", &cString::AsInt)
        .def("AsDouble", &cString::AsDouble)
        .def("IsEmpty", &cString::IsEmpty)
        .def("IsContinueLine", &cString::IsContinueLine)
        .def("CountWhitespace", &cString::CountWhitespace, cString_CountWhitespace_overloads_0_1())
        .def("CountWordsize", &cString::CountWordsize, cString_CountWordsize_overloads_0_1())
        .def("CountLinesize", &cString::CountLinesize, cString_CountLinesize_overloads_0_1())
        .def("CountNumLines", &cString::CountNumLines)
        .def("CountNumWords", &cString::CountNumWords)
        .def("GetWord", &cString::GetWord, cString_GetWord_overloads_0_1())
        .def("GetWordAt", &cString::GetWordAt, cString_GetWordAt_overloads_0_1())
        .def("IsWhitespace", (bool (cString::*)(int) const)&cString::IsWhitespace)
        .def("IsUpperLetter", (bool (cString::*)(int) const)&cString::IsUpperLetter)
        .def("IsLowerLetter", (bool (cString::*)(int) const)&cString::IsLowerLetter)
        .def("IsLetter", (bool (cString::*)(int) const)&cString::IsLetter)
        .def("IsNumber", (bool (cString::*)(int) const)&cString::IsNumber)
        .def("IsNumeric", (bool (cString::*)(int) const)&cString::IsNumeric)
        .def("IsAlphaNumeric", (bool (cString::*)(int) const)&cString::IsAlphaNumeric)
        .def("IsWhitespace", (bool (cString::*)() const)&cString::IsWhitespace)
        .def("IsUpperLetter", (bool (cString::*)() const)&cString::IsUpperLetter)
        .def("IsLowerLetter", (bool (cString::*)() const)&cString::IsLowerLetter)
        .def("IsLetter", (bool (cString::*)() const)&cString::IsLetter)
        .def("IsNumber", (bool (cString::*)() const)&cString::IsNumber)
        .def("IsNumeric", (bool (cString::*)() const)&cString::IsNumeric)
        .def("IsAlphaNumeric", (bool (cString::*)() const)&cString::IsAlphaNumeric)
        .def("Find", (int (cString::*)(char, int) const)&cString::Find, cString_Find_overloads_1_2())
        .def("Find", (int (cString::*)(const char*, int) const)&cString::Find, cString_Find_overloads_1_2())
        .def("Find", (int (cString::*)(const cString&, int) const)&cString::Find, cString_Find_overloads_1_2())
        .def("FindWord", &cString::FindWord, cString_FindWord_overloads_1_2())
        .def("Substring", &cString::Substring)
        .def("IsSubstring", &cString::IsSubstring)
        .def( other< std::istream >() >> self )
        .def( other< std::ostream >() << self )
        .def("__call__", &cString::operator ())
        .def( self == other< char >() )
        .def( self == self )
        .def( self != other< char >() )
        .def( self < other< char >() )
        .def( self > other< char >() )
        .def( self <= other< char >() )
        .def( self >= other< char >() )
        .def( self += other< char >() )
        .def( self += other< char >() )
        .def( self += self )
        .def( self + other< char >() )
        .def( self + other< char >() )
        .def( self + self )
        .def("__str__", &cString::operator const char*)
    ;

}

