////////////////////////////////////////////////////////////////
//		File name:  EnumClass.h
//		Author:		sidpage
//		Date:		October 2008
//
//		Source:		http://www.codeproject.com/KB/cpp/EnumIterator.aspx
//
//		License:	The Code Project Open License (CPOL) 1.02
//					http://www.codeproject.com/info/cpol10.aspx
//
////////////////////////////////////////////////////////////////
//		
//
//	File:	EnumClass.h
//
//  Description:
//		Class providing enum iteration and string conversion.
//
//		To declare and use the enum & EnumClass<>
//
//		// Declare in the header file for public access (if public access is required)
//		#define MYENUM {enum1 = 0,firstEnum = enum1,enum2 = 1,enum3 = 2,enum4 = 4,enum5 = 8,totalEnums,errorEnum = 0xFF}
//		DECLARE_ENUM(MyEnumClass, MyEnum, MYENUM);
//
//		// Declare in source file for implementation
//		IMPLEMENT_ENUM(MyEnumClass);
//
//
//		This will generate an enum the following:
//
//			enum MyEnum {enum1 = 0,firstEnum = enum1,enum2 = 1,enum3 = 2,enum4 = 4,enum5 = 8,totalEnums,errorEnum = 0xFF};
//			MyEnumClass__EnumClass MyEnumClass;
//
//
//		USAGE:
//			enum can be used normally
//
//			The EnumClass provides conversion and iterator functionality
//
//				std::cout << MyEnumClass(totalEnums) << std::endl;			// outputs "totalEnums"
//				std::cout << MyEnumClass("totalEnums") << std::endl;		// outputs 9
//
//			Iteration is also available:
//
//				for(MyEnum eMyEnum = firstEnum; eMyEnum != totalEnums; eMyEnum++)
//					std::cout << MyEnumClass(eMyEnum) << " : " << eMyEnum << std::endl;
//
//				EnumClass<MyEnum>::const_iterator iter = MyEnumClass.begin();
//				EnumClass<MyEnum>::const_iterator end = MyEnumClass.end()-2;	// Note: -2, to ensure totalEnums & errorEnum values are not iterated over
//				while(iter != end)
//				{
//					std::cout << MyEnumClass(*iter) << " : " << *iter << std::endl;
//					iter++;
//				}
//
//				EnumClass<MyEnum>::const_reverse_iterator rIter = MyEnumClass.rbegin();	// Note: iteration includes the totalEnums & errorEnum
//				EnumClass<MyEnum>::const_reverse_iterator rEnd = MyEnumClass.rend();
//				while(rIter != rEnd)
//				{
//					std::cout << MyEnumClass(*rIter) << " : " << *rIter << std::endl;
//					iter++;
//				}
//
//				Care must be used when the enum operator++ is used, especially when using enum values that are not sequential.
//				Standard arithmetic operations will work as expected, therefore the following will fail to
//				iterate over the complete sequence, since AltEnum4 is less than totalAltEnums.
//
//				#define MYALTENUM	{ AltEnum1=3, firstAltEnum=AltEnum1, AltEnum2, AltEnum3, AltEnum4=1, AltEnum5, totalAltEnums = 5, errorAltEnum = 0xFF }
//				DECLARE_ENUM(MyAltEnumClass, MyAltEnum, MYALTENUM);
//				IMPLEMENT_ENUM(MyAltEnumClass, MyAltEnum);
//
//				for(MyAltEnum eAltEnum = firstAltEnum; eAltEnum < totalAltEnums; eAltEnum++)
//					std::cout << MyAltEnumClass(eAltEnum) << " : " << eAltEnum << std::endl;
//
//
//		CAVETS:
//			The enum value parsing is relatively simple, however it should cope with most enum definitions:
//			It handles braces and the following arithmetic operators:
//				OR, AND, XOR, ADD, SUB, MUL, DIV, MOD
//			No precedences is is applied, evaluation is left to right.
//				A+B*C will not evaluate correctly, use braces to ensure correct evaluation, i.e. (A+B)*C
//
//
//
//
//  Copyright:      Jask Consultants © 2008, All rights reserved.
//
//                  This software is provided "as is", with no warranty.
//                  This software can be used in any way (but not sold)
//                  provided that this entire header is retained.
//
//  Web:	        www.jask.co.uk
//  Email:          info@jask.co.uk
//
//	History:
//		30/09/08	Simon Page		Created.
//
///////////////////////////////////////////////////////////////////////////////


#ifndef _ENUM_CLASS_SENTRY_H
#define _ENUM_CLASS_SENTRY_H


#include <string>
#include <vector>



template <typename T>
class EnumClass
{
	enum EAction {eUnkown, eOR, eAND, eXOR, eADD, eSUB, eMUL, eDIV, eMOD};

	// helper methods for the parser
	inline static void SkipWhitespace(const std::string& sStr, size_t& pos)		{ while((pos < sStr.size()) && isspace(sStr[pos])) pos++; }
	inline static void SkipValue(const std::string& sStr, size_t& pos)			{ while((pos < sStr.size()) && isalnum(sStr[pos])) pos++; }
	inline static void SkipAction(const std::string& sStr, size_t& pos)			{ pos++; }
	inline static bool IsValue(const std::string& sStr, size_t& pos)			{ return (isalnum(sStr[pos]) != 0); }
	inline static bool IsOpenBrace(const std::string& sStr, size_t& pos)		{ return (sStr[pos] == '('); }
	inline static void SkipToClosingBrace(const std::string& sStr, size_t& pos)
	{
		int nested = 1;
		pos++;
		while((pos < sStr.size()) && (nested != 0))
		{
			if(sStr[pos] == '(')		nested++;
			else if(sStr[pos] == ')')	nested--;
			pos++;
		}
	}

	// Enum Info object, contains an enum value and a list of associated string

	class EnumInfo
	{
	public:
		T eValue;
		std::vector<std::string> names;
		EnumInfo(const std::string& sName)
		{
			AddName(sName);
		}
		const std::string& GetPrimaryName() const			{ return names.front(); }
		void AddName(const std::string& sName)				{ names.push_back(sName); }

		bool operator == (const std::string& sName) const	{ return Contains(sName); }
		bool operator == (T eValue) const					{ return (this->eValue = eValue); }

		bool Contains(const std::string& sName) const
		{
			bool found = false;
			std::vector<std::string>::const_iterator iter = names.begin();
			std::vector<std::string>::const_iterator end = names.end();
			while((iter != end) && !found)
			{
				found = (*iter == sName);
				iter++;
			}
			return found;
		}
	};


	// Enum list container
	class EnumList : public std::vector<EnumInfo>
	{

	public:
		// return a const EnumInfo iterator for the enum value provided
		typename EnumList::const_iterator FindEnumValue(T eValue) const
		{

			bool found = false;
			typename EnumList::const_iterator iter = EnumList::begin();

			while((iter != EnumList::end()) && !found)
			{
				found = (iter->eValue == eValue);
				if(!found)
					iter++;
			}
			return iter;

		}

		// return an EnumInfo iterator for the enum value provided
		typename EnumList::iterator FindEnumValue(T eValue)
		{
			bool found = false;
			typename EnumList::iterator iter = EnumList::begin();
			while((iter != EnumList::end()) && !found)
			{
				found = (iter->eValue == eValue);
				if(!found)
					iter++;
			}

			return iter;
		}

		// return an EnumInfo iterator for the enum string provided
		typename EnumList::const_iterator FindEnumValue(const std::string& sValue) const
		{
			bool found = false;
			typename EnumList::const_iterator iter = EnumList::begin();
			while((iter != EnumList::end()) && !found)
			{
				found = (*iter == sValue);
				if(!found)
					iter++;
			}

			return iter;
		}

		// Add the EnumInfo object if enum value is not in the list
		// Add the name to the existing EnumInfo object, if the enum value is in the list
		void push_back(const EnumInfo& eNumInfo)
		{
			typename EnumList::iterator iter = FindEnumValue(eNumInfo.eValue);
			if(iter == EnumList::end())
				std::vector<EnumInfo>::push_back(eNumInfo);
			else
			{
				iter->AddName(eNumInfo.GetPrimaryName());
			}
		}
	};

public:
	// removed the { } placed at around the incoming enum string 
	std::string RemoveBraces(const std::string eString)
	{
		size_t start = eString.find('{');
		size_t end = eString.find('}');
		return eString.substr(start+1, end-1-start);
	}

	// convert the enum string into a list of EnumInfo objects
	void ParseEnumString(const std::string& _eString)
	{
		std::string eString = RemoveBraces(_eString);

		int index = 0;
		size_t start = 0;
		size_t pos = eString.find_first_of(',',start);
		while(pos != std::string::npos)
		{
			AddEnum(eString.substr(start,pos-start));
			start = pos+1;
			pos = eString.find_first_of(',',start);
			index++;
		}

		AddEnum(eString.substr(start,pos-start));
	}

	// Add an enum string to the enum list provided (only if the enum string is not empty) 
	void AddEnum(const std::string& sStr)
	{

		size_t pos = 0;
		SkipWhitespace(sStr, pos);
		std::string sInfo = sStr.substr(pos);
		if(sInfo.size() > 0)
		{
			std::string sName;
			std::string sValue;
			SplitEnumInfo(sInfo, sName, sValue);

			EnumInfo eNumInfo(sName);
			eNumInfo.eValue = DetermineEnumValue(sValue);
			enumList.push_back(eNumInfo);
		}
	}

	// return a name and value strings, from the given string
	void SplitEnumInfo(const std::string& sStr, std::string& sName, std::string& sValue)
	{
		size_t pos(0);
		SkipWhitespace(sStr, pos);

		size_t start = pos;
		pos = sStr.find('=');
		if(pos != std::string::npos)
		{
			pos = start;
			SkipValue(sStr, pos);
			sName = sStr.substr(start, pos-start);

			SkipWhitespace(sStr, pos);
			pos++;	// skip '='
			SkipWhitespace(sStr, pos);

			start = pos;
			sValue = sStr.substr(start);
		}
		else
		{
			SkipWhitespace(sStr, pos);
			SkipValue(sStr, pos);
			sName = sStr.substr(start, pos-1-start);
		}
	}

	// return the numeric value of the string provided
	T DetermineEnumValue(const std::string& sValue)
	{
		int iValue = 0;

		size_t pos(0);
		SkipWhitespace(sValue, pos);

		if(pos < sValue.size())
		{
			iValue =  Evaluate(sValue);
		}
		else if(!enumList.empty())
		{
			// no value provided, therefore use the previous value incremented by 1
			iValue = enumList.back().eValue + 1;
		}

		return static_cast<T>(iValue);
	}

	// return the numeric value of the arithmetic string provided
	int Evaluate(const std::string& sStr)
	{
		size_t pos = 0;
		int lValue = GetValue(sStr, pos);

		while(pos < sStr.size())
		{
			EAction eAction = GetAction(sStr, pos);
			int rValue = GetValue(sStr, pos);

			switch(eAction)
			{
			case eUnkown:	break;
			case eOR:		lValue |= rValue;	break;
			case eAND:		lValue &= rValue;	break;
			case eXOR:		lValue ^= rValue;	break;
			case eADD:		lValue += rValue;	break;
			case eSUB:		lValue -= rValue;	break;
			case eMUL:		lValue *= rValue;	break;
			case eDIV:		lValue /= rValue;	break;
			case eMOD:		lValue %= rValue;	break;
			}
		}
		return lValue;
	}

	// returns an arithmetic operation enum
	EAction GetAction(const std::string& sStr, size_t& pos)
	{
		EAction eAction(eUnkown);
		SkipWhitespace(sStr, pos);

		if(pos < sStr.size())
		{
			switch(sStr[pos])
			{
			case '|':	eAction = eOR;	break;
			case '&':	eAction = eAND;	break;
			case '^':	eAction = eXOR;	break;
			case '+':	eAction = eADD;	break;
			case '-':	eAction = eSUB;	break;
			case '*':	eAction = eMUL;	break;
			case '/':	eAction = eDIV;	break;
			case '%':	eAction = eMOD;	break;
			}
			SkipAction(sStr, pos);
		}
		return eAction;
	}

	// return the numeric value of the string provided
	// handles braces, recursively
	int GetValue(const std::string& sStr, size_t& pos)
	{
		int iValue = 0;

		SkipWhitespace(sStr, pos);
		size_t start = pos;

		if(pos < sStr.size())
		{
			if(IsOpenBrace(sStr, pos))
			{
				SkipToClosingBrace(sStr, pos);
				iValue = Evaluate(sStr.substr(start + 1, pos - 2 - start));
			}
			else if(IsValue(sStr, pos))
			{
				SkipValue(sStr, pos);
				iValue = ConvertValue(sStr.substr(start, pos-start));
			}
		}
		return iValue;
	}

	// return the numeric value of the string provided
	// will convert both integer values & enum values
	int ConvertValue(const std::string& sStr)
	{
		int iValue = 0;
		if(sStr.size() > 0)
		{
			if(isdigit(sStr[0]))
				iValue = GetIntegerValue(sStr);
			else
				iValue = GetEnumValue(sStr);
		}
		return iValue;
	}

	// return the numeric value of the string provided
	// will convert decimal, hex and octal values
	int GetIntegerValue(const std::string& sStr)
	{
		int eValue(0);
		size_t pos = 0;
		SkipWhitespace(sStr, pos);
		size_t start = pos;

		std::string sValue = sStr.substr(start);

		if(pos < sStr.size())
		{
			int base = 10;
			if(sValue[0] == '0')
			{
				if(sValue.size() > 1)
				{
					if((sValue[1] == 'x')|| (sValue[1] == 'X'))
					{
						sValue = sValue.substr(2);
						base = 16;
					}
					else
					{
						sValue = sValue.substr(1);
						base = 8;
					}
				}
			}
			char* endChar;
			eValue =  strtol(sValue.c_str(), &endChar, base);
		}
		return eValue;
	}

	// return the numeric value of the enum string provided
	const T GetEnumValue(const std::string& sStr) const
	{
		T eValue = static_cast<T>(0);
		size_t pos = 0;
		SkipWhitespace(sStr, pos);
		size_t start = pos;
		SkipValue(sStr, pos);
		std::string sValue = sStr.substr(start, pos-start);

		// Need only search those EnumInfo's that have been defined so far
		typename EnumList::const_iterator iter = enumList.FindEnumValue(sValue);
		if(iter != enumList.end())
			eValue = iter->eValue;

		return eValue;
	}

	// return the next enum in the sequence
	const T GetNext(T eValue) const
	{
		typename EnumList::const_iterator iter = enumList.FindEnumValue(eValue);
		++iter;
		return (*iter).eValue;
	}

	// return enum name for the enum provided
	const std::string& GetEnumString(T eValue) const
	{
		typename EnumList::const_iterator iter = enumList.FindEnumValue(eValue);
		if(iter != enumList.end())
			return iter->GetPrimaryName();
		else
			return undefined;
	}

	// enum const iterator
	class const_iterator
	{
		typename EnumList::const_iterator iter;
		const_iterator(const typename EnumList::const_iterator& _iter):iter(_iter){}
	public:
		const_iterator()							{}
		const T operator*() const					{ return iter->eValue; }
		const_iterator operator ++()				{ return ++iter; }
		const_iterator operator ++(int)				{ iter++; return iter; }
		const_iterator operator +(int offset)		{ return iter + offset; }
		const_iterator operator -(int offset)		{ return iter - offset; }

		bool operator == (const const_iterator& src) const	{ return src.iter == iter; }
		bool operator != (const const_iterator& src) const	{ return src.iter != iter; }

		friend class EnumClass;
	};

	// reverse const iterator
	class const_reverse_iterator
	{
		typename EnumList::const_reverse_iterator iter;
		const_reverse_iterator(const typename EnumList::const_reverse_iterator& _iter):iter(_iter){}
	public:
		const_reverse_iterator()						{}
		const T operator*() const						{ return iter->eValue; }
		const_reverse_iterator operator ++()			{ return ++iter; }
		const_reverse_iterator operator ++(int)			{ iter++; return iter; }
		const_reverse_iterator operator +(int offset)	{ return iter + offset; }
		const_reverse_iterator operator -(int offset)	{ return iter - offset; }

		bool operator == (const const_reverse_iterator& src) const	{ return src.iter == iter; }
		bool operator != (const const_reverse_iterator& src) const	{ return src.iter != iter; }

		friend class EnumClass;
	};

	// class friends
	friend const T& operator++(T& eValue);
	friend const T& operator++(T& eValue, int);


private:
	// Class data
	EnumList enumList;
	const std::string undefined;

public:
	// Public interface
	EnumClass(const std::string& eString)
		:undefined("undefined")							{ ParseEnumString(eString); }

	const std::string operator()(T eValue) const		{ return GetEnumString(eValue); }
	const T operator()(const std::string& sStr) const	{ return GetEnumValue(sStr); }

	const_iterator begin() const						{ return const_iterator(enumList.begin()); }
	const_iterator end() const							{ return const_iterator(enumList.end()); }
	const_reverse_iterator rbegin() const				{ return const_reverse_iterator(enumList.rbegin()); }
	const_reverse_iterator rend() const					{ return const_reverse_iterator(enumList.rend()); }

	T front() const										{ return enumList.front().eValue; }
	T back() const										{ return enumList.back().eValue; }
	size_t size() const									{ return enumList.size(); }

};



// Helper Macros

// Allows correct stringization of the enum definition
#define DEFINE_AS_STR(text)		DEFINE_AS_STR_I(text)
#define DEFINE_AS_STR_I(ENUM)	#ENUM


// Use to declare the enum, and the helper class (EnumClassName)
#define DECLARE_ENUM(EnumClassName, EnumName, ENUM) \
	enum EnumName ENUM; \
	class EnumClassName##__EnumClass: public EnumClass<EnumName> \
	{ \
		public: \
			EnumClassName##__EnumClass(): EnumClass<EnumName>(DEFINE_AS_STR(ENUM)) {}; \
	}; \
	extern EnumClassName##__EnumClass EnumClassName; \
	inline const EnumName& operator++(EnumName& eValue)			{ return (eValue = EnumClassName.GetNext(eValue)); } \
	inline const EnumName& operator++(EnumName& eValue, int)	{ return (eValue = EnumClassName.GetNext(eValue)); }



// Use to implement the helper class object (EnumClassName)
#define IMPLEMENT_ENUM(EnumClassName) \
	EnumClassName##__EnumClass EnumClassName;






#endif // ENUM_CLASS_SENTRY_H
