#pragma once
#include "ReCppCommon.h"

enum class ECppTokenType
{
	None = 0,
	Identifier,
	Symbol,
	Const,
	Max
};

enum class ECppTokenConstType
{
	None,
	Byte,
	Int,
	Int64,
	Bool,
	Float,
	Double,
	String,
	Nullptr
};

class FCppToken final
{
	using namespace Re;
	friend class FBaseParser;

public:
	constexpr static int32 NameSize = 1024;
	constexpr static int32 MaxStringConstSize = 1024;

private:
	// Token Type
	ECppTokenType TokenType = ECppTokenType::None;

	// Token Position
	int32 StartPos = 0;
	int32 StartLine = 1;
	// Token Identifier String
	char Identifier[NameSize];

public:
	// Storage for Const
	ECppTokenConstType ConstType = ECppTokenConstType::None;
	union
	{
		// TOKEN_Const values.
		uint8 Byte;								// If CPT_Byte.
		int64 Int64;							// If CPT_Int64.
		int32 Int;								// If CPT_Int.
		bool NativeBool;						// if CPT_Bool
		float Float;							// If CPT_Float.
		double Double;							// If CPT_Double.
		char String[MaxStringConstSize];		// If CPT_String
	};

public:

	void InitToken();

	// GetName
	Re::String GetTokenName() const;

	const char* GetRawTokenName() const;

	Re::String GetConstantValue() const;

	ECppTokenType GetTokenType() const
	{
		return TokenType;
	}

	ECppTokenConstType GetConstType() const
	{
		return ConstType;
	}

	// match
	bool Matches(const char Ch) const
	{
		return TokenType == ECppTokenType::Symbol && Identifier[0] == Ch && Identifier[1] == 0;
	}

	bool Matches(const char* Str) const
	{
		return (TokenType == ECppTokenType::Identifier || TokenType == ECppTokenType::Symbol)
			&& std::strcmp(Identifier, Str) == 0;
	}

	bool IsBool() const
	{
		return ConstType == ECppTokenConstType::Bool;
	}

#pragma region setters

	// setter
	void SetIdentifier(const char* InString)
	{
		InitToken();
		TokenType = ECppTokenType::Identifier;
		std::strncpy(Identifier, InString, NameSize);
	}

	void SetNullptr()
	{
		ConstType = ECppTokenConstType::Nullptr;
		TokenType = ECppTokenType::Const;
	}

	void SetConstInt64(int64 InInt64)
	{
		ConstType = ECppTokenConstType::Int64;
		TokenType = ECppTokenType::Const;
		Int64 = InInt64;
	}

	void SetConstInt(int32 InInt)
	{
		ConstType = ECppTokenConstType::Int;
		TokenType = ECppTokenType::Const;
		Int = InInt;
	}

	void SetConstBool(bool InBool)
	{
		ConstType = ECppTokenConstType::Bool;
		TokenType = ECppTokenType::Const;
		NativeBool = InBool;
	}

	void SetConstFloat(float InFloat)
	{
		ConstType = ECppTokenConstType::Float;
		TokenType = ECppTokenType::Const;
		Float = InFloat;
	}

	void SetConstDouble(double InDouble)
	{
		ConstType = ECppTokenConstType::Double;
		TokenType = ECppTokenType::Const;
		Double = InDouble;
	}

	void SetConstString(const char* InString, int32 MaxLength = MaxStringConstSize)
	{
		RE_ASSERT(MaxLength > 0);
		ConstType = ECppTokenConstType::String;
		TokenType = ECppTokenType::Const;
		if(InString != String)
		{
			std::strncpy(String, InString, MaxLength);
		}
	}

	void SetConstChar(char InChar)
	{
		String[0] = InChar;
		String[1] = 0;
		TokenType = ECppTokenType::Const;
	}

#pragma endregion

#pragma region getter

	bool GetConstInt(int32& OutInt) const
	{
		if(TokenType == ECppTokenType::Const)
		{
			if(ConstType == ECppTokenConstType::Int64)
			{
				OutInt = static_cast<int32>(Int64);
				return true;
			}
			else if(ConstType == ECppTokenConstType::Int)
			{
				OutInt = Int;
				return true;
			}
			else if(ConstType == ECppTokenConstType::Byte)
			{
				OutInt = Byte;
				return true;
			}
			else if(ConstType == ECppTokenConstType::Float)
			{
				OutInt = static_cast<int32>(Float);
				return true;
			}
			else if(ConstType == ECppTokenConstType::Double)
			{
				OutInt = static_cast<int32>(Double);
				return true;
			}
			else if (ConstType == ECppTokenConstType::Bool)
			{
				OutInt = NativeBool ? 1 : 0;
				return true;
			}
			else if(ConstType == ECppTokenConstType::Nullptr)
			{
				OutInt = 0;
				return true;
			}
		}
		return false;
	}

	bool GetConstInt64(int64& OutInt) const
	{
		if (TokenType == ECppTokenType::Const)
		{
			if (ConstType == ECppTokenConstType::Int64)
			{
				OutInt = Int64;
				return true;
			}
			else if (ConstType == ECppTokenConstType::Int)
			{
				OutInt = Int;
				return true;
			}
			else if (ConstType == ECppTokenConstType::Byte)
			{
				OutInt = Byte;
				return true;
			}
			else if (ConstType == ECppTokenConstType::Float)
			{
				OutInt = static_cast<int64>(Float);
				return true;
			}
			else if (ConstType == ECppTokenConstType::Double)
			{
				OutInt = static_cast<int64>(Double);
				return true;
			}
			else if (ConstType == ECppTokenConstType::Bool)
			{
				OutInt = NativeBool ? 1 : 0;
				return true;
			}
			else if (ConstType == ECppTokenConstType::Nullptr)
			{
				OutInt = 0;
				return true;
			}
		}
		return false;
	}

	bool GetBool(bool& OutBool) const
	{
		if (TokenType == ECppTokenType::Const)
		{
			if (ConstType == ECppTokenConstType::Int64)
			{
				OutBool = Int64 != 0;
				return true;
			}
			else if (ConstType == ECppTokenConstType::Int)
			{
				OutBool = Int != 0;
				return true;
			}
			else if (ConstType == ECppTokenConstType::Byte)
			{
				OutBool = Byte != 0;
				return true;
			}
			else if (ConstType == ECppTokenConstType::Float)
			{
				OutBool = Float != 0;
				return true;
			}
			else if (ConstType == ECppTokenConstType::Double)
			{
				OutBool = Double != 0;
				return true;
			}
			else if (ConstType == ECppTokenConstType::Bool)
			{
				OutBool = NativeBool;
				return true;
			}
			else if (ConstType == ECppTokenConstType::Nullptr)
			{
				OutBool = false;
				return true;
			}
		}
		return false;
	}

#pragma endregion
};
