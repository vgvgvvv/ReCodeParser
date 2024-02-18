#include "BaseParser.h"

#include "Token.h"

void FBaseParser::InitParserSource(const Re::String& InFileName, const char* SourceBuffer)
{
	Input = SourceBuffer;
	InputLen = static_cast<int32>(std::strlen(SourceBuffer));
	InputPos = 0;
	InputLine = 1;
	PrevPos = 0;
	PrevLine = 1;
	FileName = InFileName;
}

#pragma region basic operation

char FBaseParser::GetChar(bool bLiteral)
{
	bool bInsideComment = false;

	PrevPos = InputPos;
	PrevLine = InputLine;

Loop:
	const char c = Input[InputPos++];
	if (bInsideComment)
	{
		// Record the character as a comment.
		PrevComment += c;
	}

	if (c == '\n')
	{
		InputLine++;
	}
	else if (!bLiteral)
	{
		const char NextChar = PeekChar();
		if (c == '/' && NextChar == '*')
		{
			if (!bInsideComment)
			{
				ClearComment();
				// Record the slash and star.
				PrevComment += c;
				PrevComment += NextChar;
				bInsideComment = true;

				// Move past the star. Do it only when not in comment,
				// otherwise end of comment might be missed e.g.
				// /*/ Comment /*/
				// ~~~~~~~~~~~~~^ Will report second /* as beginning of comment
				// And throw error that end of file is found in comment.
				InputPos++;
			}

			goto Loop;
		}
		else if (c == '*' && NextChar == '/')
		{
			if (!bInsideComment)
			{
				ClearComment();
				SetError(Re::String{"Unexpected '*/' outside of comment : at "} + GetLocation());
			}

			/** Asterisk and slash always end comment. */
			bInsideComment = false;

			// Star already recorded; record the slash.
			PrevComment += Input[InputPos];

			InputPos++;
			goto Loop;
		}
	}

	if (bInsideComment)
	{
		if (c == 0)
		{
			ClearComment();
			SetError("End of class header encountered inside comment at : " + GetLocation());
		}
		goto Loop;
	}
	return c;
}

char FBaseParser::PeekChar()
{
	return (InputPos < InputLen) ? Input[InputPos] : 0;
}

char FBaseParser::GetLeadingChar()
{
	char TrailingCommentNewline = 0;

	for (;;)
	{
		bool MultipleNewlines = false;

		char c;

		// Skip blanks.
		do
		{
			c = GetChar();

			// Check if we've encountered another newline since the last one
			if (c == TrailingCommentNewline)
			{
				MultipleNewlines = true;
			}
		} while (IsWhitespace(c));

		if (c != '/' || PeekChar() != '/')
		{
			return c;
		}

		// Clear the comment if we've encountered newlines since the last comment
		if (MultipleNewlines)
		{
			ClearComment();
		}

		// Record the first slash.  The first iteration of the loop will get the second slash.
		PrevComment += c;

		do
		{
			c = GetChar(true);
			if (c == 0)
				return c;
			PrevComment += c;
		} while (!IsEOL(c));

		TrailingCommentNewline = c;

		for (;;)
		{
			c = GetChar();
			if (c == 0)
				return c;
			if (c == TrailingCommentNewline || !IsEOL(c))
			{
				UngetChar();
				break;
			}

			PrevComment += c;
		}
	}
}

void FBaseParser::UngetChar()
{
	InputPos = PrevPos;
	InputLine = PrevLine;
}

void FBaseParser::SetError(const Re::String& str)
{
	Errors.push(str);
}

bool FBaseParser::GetError(Re::String& str)
{
	if(Errors.empty())
	{
		return false;
	}
	str = Errors.top();
	return true;
}

bool FBaseParser::IsEOL(char c)
{
	return c == '\n' || c == '\r' || c == 0;
}

bool FBaseParser::IsWhitespace(char c)
{
	return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

#pragma endregion

void FBaseParser::ClearComment()
{
	PrevComment.clear();
}

Re::SharedPtr<Token> FBaseParser::GetToken(bool bNoConsts)
{
	char c = GetLeadingChar();
	if (c == 0)
	{
		UngetChar();
		return nullptr;
	}

	auto token = RE_MAKE_SHARED(Token)();
	token->StartPos = PrevPos;
	token->StartLine = PrevLine;

	char p = PeekChar();
	if((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c == '_'))
	{
		int32 Length = 0;
		do
		{
			token->Identifier[Length++] = c;
			if(Length > Token::NameSize)
			{
				SetError(Re::String{"Identifer length exceeds maximum of "} + std::to_string(Token::NameSize) + " : at " + GetLocation());
				Length = ((int32)Token::NameSize) - 1;
				break;
			}
			c = GetChar();
		} while ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || (c == '_'));
		UngetChar();
		token->Identifier[Length] = 0;
		// Assume this is an identifier unless we find otherwise.
		token->TokenType = ETokenType::Identifier;

		if(!bNoConsts)
		{
			if(token->Matches("true"))
			{
				token->SetConstBool(true);
				return token;
			}
			else if(token->Matches("false"))
			{
				token->SetConstBool(false);
				return token;
			}
			else if(token->Matches("nullptr"))
			{
				token->SetNullptr();
			}
		}
		return token;
	}
	// if const values are allowed, determine whether the non-identifier token represents a const
	else if (!bNoConsts && ((c >= '0' && c <= '9') || ((c == '+' || c == '-') && (p >= '0' && p <= '9'))))
	{
		// Integer or floating point constant.
		bool  bIsFloat = 0;
		int32 Length = 0;
		bool  bIsHex = 0;

		do
		{
			if (c == '.')
			{
				bIsFloat = true;
			}
			if (c == 'X' || c == 'x')
			{
				bIsHex = true;
			}

			token->Identifier[Length++] = c;
			if (Length >= Token::NameSize)
			{
				SetError(Re::String{"Number length exceeds maximum of "} + std::to_string(Token::NameSize) + ": at " + GetLocation() );
				Length = ((int32)Token::NameSize) - 1;
				break;
			}
			
			c = static_cast<char>(std::toupper(GetChar()));
		} while ((c >= '0' && c <= '9') || (!bIsFloat && c == '.') || (!bIsHex && c == 'X') || (bIsHex && c >= 'A' && c <= 'F'));

		token->Identifier[Length] = 0;
		if (!bIsFloat || c != 'F')
		{
			UngetChar();
		}

		if (bIsFloat)
		{
			token->SetConstFloat(std::atof(token->Identifier));
		}
		else if (bIsHex)
		{
			char* End = token->Identifier + std::strlen(token->Identifier);
			token->SetConstInt64(std::strtoll(token->Identifier, &End, 0));
		}
		else
		{
			token->SetConstInt64(std::atoll(token->Identifier));
		}
		return token;
	}
	else if (c == '\'')
	{
		char ActualCharLiteral = GetChar(/*bLiteral=*/ true);

		bool IsUnicode = false;
		char FrontChar = '?';
		if (ActualCharLiteral == '\\')
		{
			ActualCharLiteral = GetChar(/*bLiteral=*/ true);
			switch (ActualCharLiteral)
			{
			case static_cast<char>('t'):
				ActualCharLiteral = '\t';
				break;
			case static_cast<char>('n'):
				ActualCharLiteral = '\n';
				break;
			case static_cast<char>('r'):
				ActualCharLiteral = '\r';
				break;
			case static_cast<char>('u'):
			case static_cast<char>('x'):
			case static_cast<char>('U'):
				IsUnicode = true;
				FrontChar = ActualCharLiteral;
				break;
			}
		}

		if(IsUnicode)
		{
			Re::String WS;
			WS += FrontChar;
			for(int i = 0; i < 4; i ++)
			{
				auto SubChar = GetChar(/*bLiteral=*/ true);
				WS += SubChar;
			}

			c = GetChar(/*bLiteral=*/ true);
			if (c != '\'')
			{
				SetError(Re::String{"Unterminated character constant : at "} + FileName + " : " + GetLocation());
				UngetChar();
			}
			
			// TODO parse unicode char
			RE_LOG(Re::String{"UnHandled Unicode Char : "} + WS);
			token->SetConstChar(FrontChar);
		}
		else
		{
			c = GetChar(/*bLiteral=*/ true);
			if (c != '\'')
			{
				SetError(Re::String{"Unterminated character constant : at "} + FileName + " : " + GetLocation());
				UngetChar();
			}

		}
		token->SetConstChar(ActualCharLiteral);

		return token;
	}
	else if (c == '"')
	{
		// String constant.
		char Temp[Token::MaxStringConstSize];
		int32 Length = 0;
		c = GetChar(/*bLiteral=*/ true);
		while ((c != '"') && !IsEOL(c))
		{
			if (c == '\\')
			{
				c = GetChar(/*bLiteral=*/ true);
				if (IsEOL(c))
				{
					break;
				}
				else if (c == 'n')
				{
					// Newline escape sequence.
					c = '\n';
				}
			}
			Temp[Length++] = c;
			if (Length >= Token::MaxStringConstSize)
			{
				SetError(Re::String{"String constant exceeds maximum of "} + std::to_string(Token::MaxStringConstSize) +
					" characters : at " + FileName + " : " + GetLocation());
				c = '\"';
				Length = ((int32)Token::MaxStringConstSize) - 1;
				break;
			}
			c = GetChar(/*bLiteral=*/ true);
		}
		Temp[Length] = 0;

		if (c != '"')
		{
			SetError(Re::String{"Unterminated string constant: "} + Temp + " at " + FileName + " : " + GetLocation());
			UngetChar();
		}

		token->SetConstString(Temp);
		return token;
	}
	else
	{
		// Symbol.
		int32 Length = 0;
		token->Identifier[Length++] = c;

		// Handle special 2-character symbols.
		#define PAIR(cc,dd) ((c==cc)&&(d==dd)) /* Comparison macro for convenience */
		char d = GetChar();
		if
			(PAIR('<', '<')
				|| (PAIR('>', '>'))
				|| PAIR('!', '=')
				|| PAIR('<', '=')
				|| PAIR('>', '=')
				|| PAIR('+', '+')
				|| PAIR('-', '-')
				|| PAIR('+', '=')
				|| PAIR('-', '=')
				|| PAIR('*', '=')
				|| PAIR('/', '=')
				|| PAIR('&', '&')
				|| PAIR('|', '|')
				|| PAIR('^', '^')
				|| PAIR('=', '=')
				|| PAIR('*', '*')
				|| PAIR('~', '=')
				|| PAIR(':', ':')
				)
		{
			token->Identifier[Length++] = d;
			if (c == '>' && d == '>')
			{
				if (GetChar() == '>')
				{
					token->Identifier[Length++] = '>';
				}
				else
				{
					UngetChar();
				}
			}
		}
		else
		{
			UngetChar();
		}
		#undef PAIR

		token->Identifier[Length] = 0;
		token->TokenType = ETokenType::Symbol;

		return token;
	}
}

Re::Vector<Re::SharedPtr<Token>> FBaseParser::GetTokensUntil(Re::Func<bool(Token&)> Condition, bool bNoConst, const Re::String& DebugMessage)
{
	Re::Vector<Re::SharedPtr<Token>> Tokens;
	while(true)
	{
		auto CurrentToken = GetToken(bNoConst);

		if(CurrentToken == nullptr)
		{
			SetError(Re::String{"Exit Early !! "} + DebugMessage + " at " + FileName + " : " + GetLocation());
		}

		Tokens.push_back(CurrentToken);

		if(Condition(*CurrentToken))
		{
			break;
		}
	}

	return Tokens;
}

Re::Vector<Re::SharedPtr<Token>> FBaseParser::GetTokenUntilMatch(const char Match, bool bNoConst, const Re::String& DebugMessage)
{
	Re::Vector<Re::SharedPtr<Token>> Tokens;
	while (true)
	{
		auto CurrentToken = GetToken(bNoConst);
		
		if(CurrentToken == nullptr)
		{
			SetError(Re::String{"Exit Early !! "} + DebugMessage + " at " + FileName + " : " + GetLocation());
		}
		
		Tokens.push_back(CurrentToken);

		if (CurrentToken->Matches(Match))
		{
			break;
		}
	}

	return Tokens;
}

Re::Vector<Re::SharedPtr<Token>> FBaseParser::GetTokenUntilMatch(const char* Match, bool bNoConst, const Re::String& DebugMessage)
{
	Re::Vector<Re::SharedPtr<Token>> Tokens;
	while (true)
	{
		auto CurrentToken = GetToken(bNoConst);
		if(CurrentToken == nullptr)
		{
			SetError(Re::String{"Exit Early !! "} + DebugMessage + " : at " + FileName + " : " + GetLocation());
		}

		Tokens.push_back(CurrentToken);

		if (CurrentToken->Matches(Match))
		{
			break;
		}
	}

	return Tokens;
}
Re::Vector<Re::SharedPtr<Token>> FBaseParser::GetTokensUntilPairMatch(const char Left, const char Right, const Re::String& DebugMessage)
{
	int MatchCount = 1;
	Re::Vector<Re::SharedPtr<Token>> Tokens;
	while(true)
	{
		auto CurrentToken = GetToken(false);
		if(CurrentToken == nullptr)
		{
			SetError(Re::String{"Exit Early !! "} +  DebugMessage + " : at " + FileName + " : " + GetLocation());
		}

		Tokens.push_back(CurrentToken);

		if(CurrentToken->Matches(Left))
		{
			MatchCount++;
		}
		else if(CurrentToken->Matches(Right))
		{
			MatchCount--;
		}
		if(MatchCount == 0)
		{
			break;
		}
	}
	return Tokens;
}

void FBaseParser::UngetToken(const Re::SharedPtr<Token>& Token)
{
	InputPos = Token->StartPos;
	InputLine = Token->StartLine;
}

Re::SharedPtr<Token> FBaseParser::GetIdentifier(bool bNoConsts)
{
	auto Token = GetToken(bNoConsts);
	if (!Token)
	{
		return nullptr;
	}

	if (Token->TokenType == ETokenType::Identifier)
	{
		return Token;
	}

	UngetToken(Token);
	return nullptr;
}

Re::SharedPtr<Token> FBaseParser::GetSymbol()
{
	auto Token = GetToken();
	if (!Token)
	{
		return nullptr;
	}

	if (Token->TokenType == ETokenType::Symbol)
	{
		return Token;
	}

	UngetToken(Token);
	return nullptr;
}

bool FBaseParser::GetConstInt(int32& Result, const char* Tag)
{
	auto Token = GetToken();
	if(Token)
	{
		if(Token->GetConstInt(Result))
		{
			return true;
		}
		else
		{
			UngetToken(Token);
		}
	}

	if(Tag != nullptr)
	{
		SetError(Re::String{"Missing constant integer : "} + Tag + " : at " + GetLocation());
	}

	return false;
}

bool FBaseParser::GetConstInt64(int64& Result, const char* Tag)
{
	auto Token = GetToken();
	if (Token)
	{
		if (Token->GetConstInt64(Result))
		{
			return true;
		}
		else
		{
			UngetToken(Token);
		}
	}

	if (Tag != nullptr)
	{
		SetError(Re::String{"Missing constant integer : "} +  Tag + " : at " + GetLocation());
	}

	return false;
}

bool FBaseParser::MatchIdentifier(const char* Match)
{
	auto Token = GetToken();
	if(Token != nullptr)
	{
		if(Token->GetTokenType() == ETokenType::Identifier &&
			Token->Matches(Match))
		{
			return true;
		}
		else
		{
			UngetToken(Token);
		}
	}
	return false;
}

bool FBaseParser::MatchConstInt(const char* Match)
{
	auto Token = GetToken();
	if(Token)
	{
		if(Token->GetTokenType() == ETokenType::Const
			&& (Token->GetConstType() == ETokenConstType::Int || Token->GetConstType() == ETokenConstType::Int64)
			&& Token->GetTokenName() == Match)
		{
			return true;
		}
		else
		{
			UngetToken(Token);
		}
	}
	
	return false;
}

bool FBaseParser::MatchAnyConstInt()
{
	auto Token = GetToken();
	if (Token)
	{
		if (Token->GetTokenType() == ETokenType::Const
			&& (Token->GetConstType() == ETokenConstType::Int
				|| Token->GetConstType() == ETokenConstType::Int64))
		{
			return true;
		}
		else
		{
			UngetToken(Token);
		}
	}

	return false;
}

bool FBaseParser::PeekIdentifier(const char* Match)
{
	auto Token = GetToken(true);
	if(!Token)
	{
		return false;
	}
	UngetToken(Token);
	return Token->GetTokenType() == ETokenType::Identifier
		&& Token->GetTokenName() == Match;
}

bool FBaseParser::MatchSymbol(const char Match)
{
	auto Token = GetToken(true);
	if(Token)
	{
		if(Token->GetTokenType() == ETokenType::Symbol
			&& Token->GetRawTokenName()[0] == Match 
			&& Token->GetRawTokenName()[1] == 0)
		{
			return true;
		}else
		{
			UngetToken(Token);
		}
	}
	return false;
}

bool FBaseParser::MatchSymbol(const char* Match)
{
	auto Token = GetToken(true);
	if (Token)
	{
		if (Token->GetTokenType() == ETokenType::Symbol
			&& Token->GetTokenName() == Match)
		{
			return true;
		}
		else
		{
			UngetToken(Token);
		}
	}
	return false;
}

bool FBaseParser::MatchToken(Re::Func<bool(const Token&)> Condition)
{
	auto Token = GetToken(true);
	if (Token)
	{
		if (Condition(*Token))
		{
			return true;
		}
		else
		{
			UngetToken(Token);
		}
	}
	return false;
}

bool FBaseParser::MatchSemi()
{
	if (MatchSymbol(';'))
	{
		return true;
	}
	return false;
}

void FBaseParser::RequireSemi()
{
	if(!MatchSymbol(';'))
	{
		auto Token = GetToken();
		if(Token)
		{
			SetError(Re::String{"Missing ';' before "} + Token->GetRawTokenName() + " : at " + GetLocation());
		}
		else
		{
			SetError(Re::String{"Missing ';'"} + " : at " + GetLocation());
		}
	}
}

Re::String FBaseParser::GetLocation() const
{
	return std::to_string(InputLen) + ":" + std::to_string(InputPos);
}

bool FBaseParser::PeekSymbol(const char Match)
{
	auto Token = GetToken(true);
	if(!Token)
	{
		return false;
	}
	UngetToken(Token);
	return Token->GetTokenType() == ETokenType::Symbol
		&& Token->GetRawTokenName()[0] == Match
		&& Token->GetRawTokenName()[1] == 0;
}

void FBaseParser::RequireIdentifier(const char* Match, const char* Tag)
{
	if(!MatchIdentifier(Match))
	{
		SetError(Re::String{"Missing "} + Match + " in " + Tag + " : at " + GetLocation());
	}
}

void FBaseParser::RequireSymbol(char Match, const char* Tag)
{
	if(!MatchSymbol(Match))
	{
		SetError(Re::String{"Missing "} + Match + " in " + Tag + " : at " + GetLocation());
	}
}

void FBaseParser::RequireSymbol(char Match, const Re::Func<Re::String()>& TagGetter)
{
	if(!MatchSymbol(Match))
	{
		SetError(Re::String{"Missing "} +  Match + " in " + TagGetter() + " : at " + GetLocation());
	}
}

void FBaseParser::RequireConstInt(const char* Match, const char* Tag)
{
	if(!MatchConstInt(Match))
	{
		SetError(Re::String{"Missing integer '"} + Match + "' in " + Tag + " : at " + GetLocation());
	}
}

void FBaseParser::RequireAnyConstInt(const char* Tag)
{
	if(!MatchAnyConstInt())
	{

		SetError(Re::String{"Missing integer in "} + Tag + " : at " + GetLocation());
	}
}
