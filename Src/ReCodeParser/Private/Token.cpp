#include "Token.h"

void FCppToken::InitToken()
{
	TokenType = ECppTokenType::None;
	StartPos = 0;
	StartLine = 0;
	*Identifier = 0;
	std::memset(String, 0, sizeof(String));
}

Re::String FCppToken::GetTokenName() const
{
	if(TokenType == ECppTokenType::Const)
	{
		return GetConstantValue();		
	}else
	{
		return Re::String(Identifier);
	}
}

const char* FCppToken::GetRawTokenName() const
{
	return Identifier;
}

Re::String FCppToken::GetConstantValue() const
{
	if(TokenType == ECppTokenType::Const)
	{
		switch (ConstType)
		{
		case ECppTokenConstType::Byte: 
			return FString::Printf(TEXT("%u"), Byte);
		case ECppTokenConstType::Int: 
			return FString::Printf(TEXT("%d"), Int);
		case ECppTokenConstType::Int64:
			return FString::Printf(TEXT("%" INT64_FMT), Int64);
		case ECppTokenConstType::Bool: 
			return FString::Printf(TEXT("%s"), NativeBool ? TEXT("true") : TEXT("false"));
		case ECppTokenConstType::Float: 
			return FString::Printf(TEXT("%f"), Float);
		case ECppTokenConstType::Double: 
			return FString::Printf(TEXT("%f"), Double);
		case ECppTokenConstType::String: 
			return FString::Printf(TEXT("\"%s\""), String);
		case ECppTokenConstType::Nullptr:
			return "nullptr";
		default: 
			return "InvalidTypeForToken";
		}
	}
	return "NotConstant";
}
