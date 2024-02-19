#include "Token.h"

namespace ReParser
{
	void Token::InitToken()
	{
		TokenType = ETokenType::None;
		StartPos = 0;
		StartLine = 0;
		*Identifier = 0;
		std::memset(String, 0, sizeof(String));
	}

	Re::String Token::GetTokenName() const
	{
		if(TokenType == ETokenType::Const)
		{
			return GetConstantValue();
		}else
		{
			return Re::String{Identifier};
		}
	}

	const char* Token::GetRawTokenName() const
	{
		return Identifier;
	}

	Re::String Token::GetConstantValue() const
	{
		if(TokenType == ETokenType::Const)
		{
			switch (ConstType)
			{
			case ETokenConstType::Byte:
				return std::to_string(Byte);
			case ETokenConstType::Int:
				return std::to_string(Int);
			case ETokenConstType::Int64:
				return std::to_string(Int64);
			case ETokenConstType::Bool:
				return NativeBool ? "true" : "false";
			case ETokenConstType::Float:
				return std::to_string(Float);
			case ETokenConstType::Double:
				return std::to_string(Double);
			case ETokenConstType::String:
				return Re::String("\"") + String + "\"";
			case ETokenConstType::Nullptr:
				return "nullptr";
			default:
				return "InvalidTypeForToken";
			}
		}
		return "NotConstant";
	}
}