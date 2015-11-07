#ifndef __MDB_UTILS_H__
#define __MDB_UTILS_H__

//数据库的一些辅助工具
//需要C++14

#include <string>
#include <sstream>

//支持的比较操作
enum MCompare
{
	Cequal, Cgreater, Clesser, Cinequal,
	Cgreater_equal, Clesser_equal,
	Clike
};

//数据库中支持的数据类型
enum MType
{
	Tnull,
	Tbool,
	Tchar, Tint, Tint64, Tfloat, Tdouble,
	Tuchar, Tuint, Tuint64,
	Tstring
};

//可变数据类型
class MVariant
{
protected:
	MType type;
	union
	{
		bool boolD;
		char charD;
		int intD;
		long long int64D;
		unsigned char ucharD;
		unsigned int uintD;
		unsigned long long uint64D;
		float floatD;
		double doubleD;
	}data;
	std::string strData;

	//自动取出union中对应的变量
#define AUTO_CALL \
	switch (sType) \
	{ \
		case Tbool: \
			return func(data.boolD); \
		case Tchar: \
			return func(data.charD); \
		case Tint: \
			return func(data.intD); \
		case Tint64: \
			return func(data.int64D); \
		case Tuchar: \
			return func(data.ucharD); \
		case Tuint: \
			return func(data.uintD); \
		case Tuint64: \
			return func(data.uint64D); \
		case Tfloat: \
			return func(data.floatD); \
		case Tdouble: \
			return func(data.doubleD); \
		default: \
			return func(zeroInt); \
	}

	template<typename ret, typename F>
	ret autoCall(const MType sType, F func) const
	{
		static const int zeroInt = 0;
		AUTO_CALL
	}
	template<typename ret, typename F>
	ret autoCall(const MType sType, F func)
	{
		static int zeroInt = 0;
		zeroInt = 0;
		AUTO_CALL
	}
#undef AUTO_CALL
	
	template<typename _t>
	_t convertTo() const
	{
		if (type == Tstring)
		{
			_t ret;
			std::istringstream stream(strData);
			stream >> ret;
			return ret;
		}
		return autoCall<_t>(type, [](auto var)->_t { return (_t)var; });
	}

	template<MType _tE, typename _t>
	MVariant& saveAs(_t x)
	{
		type = _tE;
		autoCall<void>(_tE, [x](auto& var) { var = x; });
		return *this;
	}
public:
	operator bool() const { return convertTo<bool>(); }
	operator char() const { return convertTo<char>(); }
	operator int() const { return convertTo<int>(); }
	operator long long() const { return convertTo<long long>(); }
	operator float() const { return convertTo<float>(); }
	operator double() const { return convertTo<double>(); }
	operator unsigned char() const { return convertTo<unsigned char>(); }
	operator unsigned int() const { return convertTo<unsigned int>(); }
	operator unsigned long long() const { return convertTo<unsigned long long>(); }
	operator std::string() const
	{
		if (type == Tstring)
			return strData;
		std::ostringstream stream;
		autoCall<void>(type, [&stream](auto var) { stream << var; });
		return stream.str();
	}

	operator short() const { return convertTo<short>(); }
	operator long() const { return convertTo<long>(); }
	operator unsigned short() const { return convertTo<unsigned short>(); }
	operator unsigned long() const { return convertTo<unsigned long>(); }
	operator wchar_t() const { return (wchar_t)convertTo<unsigned int>(); }

	MVariant& operator=(bool x) { return saveAs<Tbool>(x); }
	MVariant& operator=(char x) { return saveAs<Tchar>(x); }
	
	MVariant& operator=(int x) { return saveAs<Tint>(x); }
	MVariant& operator=(long long x) { return saveAs<Tint64>(x); }
	MVariant& operator=(float x) { return saveAs<Tfloat>(x); }
	MVariant& operator=(double x) { return saveAs<Tdouble>(x); }
	MVariant& operator=(unsigned char x) { return saveAs<Tuchar>(x); }
	MVariant& operator=(unsigned int x) { return saveAs<Tuint>(x); }
	MVariant& operator=(unsigned long long x) { return saveAs<Tuint64>(x); }
	MVariant& operator=(std::string x)
	{
		type = Tstring;
		strData = x;
		return *this;
	}
	MVariant& operator=(const char * x)
	{
		type = Tstring;
		strData = x;
		return *this;
	}

	MVariant& operator=(short x) { return saveAs<Tint>(int(x)); }
	MVariant& operator=(long x) { return saveAs<Tint>(int(x)); }
	MVariant& operator=(unsigned short x) { return saveAs<Tuint>((unsigned int)x); }
	MVariant& operator=(unsigned long x) { return saveAs<Tuint>((unsigned int)x); }
	MVariant& operator=(wchar_t x) { return saveAs<Tuint>((unsigned int)x); }

	MVariant() : type(Tnull) { data.intD = 0; }
	MVariant(bool x) { saveAs<Tbool>(x); }
	MVariant(char x) { saveAs<Tchar>(x); }
	MVariant(int x) { saveAs<Tint>(x); }
	MVariant(long long x) { saveAs<Tint64>(x); }
	MVariant(float x) { saveAs<Tfloat>(x); }
	MVariant(double x) { saveAs<Tdouble>(x); }
	MVariant(unsigned char x) { saveAs<Tuchar>(x); }
	MVariant(unsigned int x) { saveAs<Tuint>(x); }
	MVariant(unsigned long long x) { saveAs<Tuint64>(x); }
	MVariant(std::string x) : type(Tstring), strData(x) {}
	MVariant(const char * x) : type(Tstring) { strData = x; }

	MVariant(short x) { saveAs<Tint>(int(x)); }
	MVariant(long x) { saveAs<Tint>(int(x)); }
	MVariant(unsigned short x) { saveAs<Tuint>((unsigned int)x); }
	MVariant(unsigned long x) { saveAs<Tuint>((unsigned int)x); }
	MVariant(wchar_t x) { saveAs<Tuint>((unsigned int)x); }

	//转换数据类型
	void convertType(MType targetType)
	{
		if (targetType == type)
			return;
		if (targetType == Tnull)
			data.intD = 0;
		else if (targetType == Tstring)
		{
			std::ostringstream stream;
			autoCall<void>(type, [&stream](auto var) { stream << var; });
			strData = stream.str();
		}
		else
		{
			autoCall<void>(targetType,
				[this](auto& var)
				{
					auto type_var = var;
					var = convertTo<decltype(type_var)>();
				});
		}
		type = targetType;
	}

	//获取数据类型
	MType getType() const
	{
		return type;
	}

	//判断是否为空
	bool isNull() const
	{
		return type == Tnull;
	}

	bool operator==(const MVariant& var) const
	{
		//空变量不等于任何变量
		if (type == Tnull || var.type == Tnull)
			return false;

		if (type == Tstring && var.type == Tstring)
			return strData == var.strData;
		if (type == Tstring || var.type == Tstring)
			return false;

		//当两种数据类型互相转换都相等时 才认为两个变量相等
		return 
			autoCall<bool>(type,
				[&var](auto myvar)->bool
				{
					auto cvar = var.convertTo<decltype(myvar)>();
					return myvar == cvar;
				}) &&
			var.autoCall<bool>(var.type,
				[&var, this](auto cvar)->bool
				{
					auto myvar = this->convertTo<decltype(cvar)>();
					return myvar == cvar;
				});
		
	}
	bool operator!=(const MVariant& var) const
	{
		return !((*this) == var);
	}
};
#endif