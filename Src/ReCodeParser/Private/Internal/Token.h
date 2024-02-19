#pragma once
#include "ReCppCommon.h"

namespace ReParser
{
	enum class ETokenType
	{
		None = 0,
		Identifier,
		Symbol,
		Const,
		Max
	};

	enum class ETokenConstType
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

	class Token final
	{
		friend class BaseParser;

	public:
		constexpr static int32 NameSize = 1024;
		constexpr static int32 MaxStringConstSize = 1024;

	private:
		// Token Type
		ETokenType TokenType = ETokenType::None;

		// Token Position
		int32 StartPos = 0;
		int32 StartLine = 1;
		// Token Identifier String
		char Identifier[NameSize]{};

	public:
		// Storage for Const
		ETokenConstType ConstType = ETokenConstType::None;
		union
		{
			// TOKEN_Const values.
			uint8 Byte{};							// If CPT_Byte.
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

		ETokenType GetTokenType() const
		{
			return TokenType;
		}

		ETokenConstType GetConstType() const
		{
			return ConstType;
		}

		// match
		bool Matches(const char Ch) const
		{
			return TokenType == ETokenType::Symbol && Identifier[0] == Ch && Identifier[1] == 0;
		}

		bool Matches(const char* Str) const
		{
			return (TokenType == ETokenType::Identifier || TokenType == ETokenType::Symbol)
				&& std::strcmp(Identifier, Str) == 0;
		}

		bool IsBool() const
		{
			return ConstType == ETokenConstType::Bool;
		}

	#pragma region setters

		// setter
		void SetIdentifier(const char* InString)
		{
			InitToken();
			TokenType = ETokenType::Identifier;
			std::strncpy(Identifier, InString, NameSize);
		}

		void SetNullptr()
		{
			ConstType = ETokenConstType::Nullptr;
			TokenType = ETokenType::Const;
		}

		void SetConstInt64(int64 InInt64)
		{
			ConstType = ETokenConstType::Int64;
			TokenType = ETokenType::Const;
			Int64 = InInt64;
		}

		void SetConstInt(int32 InInt)
		{
			ConstType = ETokenConstType::Int;
			TokenType = ETokenType::Const;
			Int = InInt;
		}

		void SetConstBool(bool InBool)
		{
			ConstType = ETokenConstType::Bool;
			TokenType = ETokenType::Const;
			NativeBool = InBool;
		}

		void SetConstFloat(float InFloat)
		{
			ConstType = ETokenConstType::Float;
			TokenType = ETokenType::Const;
			Float = InFloat;
		}

		void SetConstDouble(double InDouble)
		{
			ConstType = ETokenConstType::Double;
			TokenType = ETokenType::Const;
			Double = InDouble;
		}

		void SetConstString(const char* InString, int32 MaxLength = MaxStringConstSize)
		{
			RE_ASSERT(MaxLength > 0);
			ConstType = ETokenConstType::String;
			TokenType = ETokenType::Const;
			if(InString != String)
			{
				std::strncpy(String, InString, MaxLength);
			}
		}

		void SetConstChar(char InChar)
		{
			String[0] = InChar;
			String[1] = 0;
			TokenType = ETokenType::Const;
		}

	#pragma endregion

	#pragma region getter

		bool GetConstInt(int32& OutInt) const
		{
			if(TokenType == ETokenType::Const)
			{
				if(ConstType == ETokenConstType::Int64)
				{
					OutInt = static_cast<int32>(Int64);
					return true;
				}
				else if(ConstType == ETokenConstType::Int)
				{
					OutInt = Int;
					return true;
				}
				else if(ConstType == ETokenConstType::Byte)
				{
					OutInt = Byte;
					return true;
				}
				else if(ConstType == ETokenConstType::Float)
				{
					OutInt = static_cast<int32>(Float);
					return true;
				}
				else if(ConstType == ETokenConstType::Double)
				{
					OutInt = static_cast<int32>(Double);
					return true;
				}
				else if (ConstType == ETokenConstType::Bool)
				{
					OutInt = NativeBool ? 1 : 0;
					return true;
				}
				else if(ConstType == ETokenConstType::Nullptr)
				{
					OutInt = 0;
					return true;
				}
			}
			return false;
		}

		bool GetConstInt64(int64& OutInt) const
		{
			if (TokenType == ETokenType::Const)
			{
				if (ConstType == ETokenConstType::Int64)
				{
					OutInt = Int64;
					return true;
				}
				else if (ConstType == ETokenConstType::Int)
				{
					OutInt = Int;
					return true;
				}
				else if (ConstType == ETokenConstType::Byte)
				{
					OutInt = Byte;
					return true;
				}
				else if (ConstType == ETokenConstType::Float)
				{
					OutInt = static_cast<int64>(Float);
					return true;
				}
				else if (ConstType == ETokenConstType::Double)
				{
					OutInt = static_cast<int64>(Double);
					return true;
				}
				else if (ConstType == ETokenConstType::Bool)
				{
					OutInt = NativeBool ? 1 : 0;
					return true;
				}
				else if (ConstType == ETokenConstType::Nullptr)
				{
					OutInt = 0;
					return true;
				}
			}
			return false;
		}

		bool GetBool(bool& OutBool) const
		{
			if (TokenType == ETokenType::Const)
			{
				if (ConstType == ETokenConstType::Int64)
				{
					OutBool = Int64 != 0;
					return true;
				}
				else if (ConstType == ETokenConstType::Int)
				{
					OutBool = Int != 0;
					return true;
				}
				else if (ConstType == ETokenConstType::Byte)
				{
					OutBool = Byte != 0;
					return true;
				}
				else if (ConstType == ETokenConstType::Float)
				{
					OutBool = Float != 0;
					return true;
				}
				else if (ConstType == ETokenConstType::Double)
				{
					OutBool = Double != 0;
					return true;
				}
				else if (ConstType == ETokenConstType::Bool)
				{
					OutBool = NativeBool;
					return true;
				}
				else if (ConstType == ETokenConstType::Nullptr)
				{
					OutBool = false;
					return true;
				}
			}
			return false;
		}

	#pragma endregion
	};
}

