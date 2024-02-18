#include "BaseParser.h"

#include "Token.h"
#include "ErrorException.h"

void FBaseParser::InitParserSource(const Re::String& InFileName, const char* SourceBuffer)
{
	Input = SourceBuffer;
	InputLen = FCString::Strlen(SourceBuffer);
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
				FParserError::Throwf("Unexpected '*/' outside of comment");
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
			FParserError::Throwf("End of class header encountered inside comment");
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

Re::SharedPtr<FCppToken> FBaseParser::GetToken(bool bNoConsts)
{
	char c = GetLeadingChar();
	if (c == 0)
	{
		UngetChar();
		return nullptr;
	}

	auto Token = RE_MAKE_SHARED(FCppToken)();
	Token->StartPos = PrevPos;
	Token->StartLine = PrevLine;

	char p = PeekChar();
	if((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c == '_'))
	{
		int32 Length = 0;
		do
		{
			Token->Identifier[Length++] = c;
			if(Length > FCppToken::NameSize)
			{
				FParserError::Throwf("Identifer length exceeds maximum of %d", (int32)FCppToken::NameSize);
				Length = ((int32)FCppToken::NameSize) - 1;
				break;
			}
			c = GetChar();
		} while ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || (c == '_'));
		UngetChar();
		Token->Identifier[Length] = 0;
		// Assume this is an identifier unless we find otherwise.
		Token->TokenType = ECppTokenType::Identifier;

		if(!bNoConsts)
		{
			if(Token->Matches("true"))
			{
				Token->SetConstBool(true);
				return Token;
			}
			else if(Token->Matches("false"))
			{
				Token->SetConstBool(false);
				return Token;
			}
			else if(Token->Matches("nullptr"))
			{
				Token->SetNullptr();
			}
		}
		return Token;
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

			Token->Identifier[Length++] = c;
			if (Length >= FCppToken::NameSize)
			{
				FParserError::Throwf("Number length exceeds maximum of %d ", (int32)FCppToken::NameSize);
				Length = ((int32)FCppToken::NameSize) - 1;
				break;
			}
			
			c = FChar::ToUpper(GetChar());
		} while ((c >= '0' && c <= '9') || (!bIsFloat && c == '.') || (!bIsHex && c == 'X') || (bIsHex && c >= 'A' && c <= 'F'));

		Token->Identifier[Length] = 0;
		if (!bIsFloat || c != 'F')
		{
			UngetChar();
		}

		if (bIsFloat)
		{
			Token->SetConstFloat(FCString::Atof(Token->Identifier));
		}
		else if (bIsHex)
		{
			char* End = Token->Identifier + FCString::Strlen(Token->Identifier);
			Token->SetConstInt64(FCString::Strtoi64(Token->Identifier, &End, 0));
		}
		else
		{
			Token->SetConstInt64(FCString::Atoi64(Token->Identifier));
		}
		return Token;
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
			WS.AppendChar(FrontChar);
			for(int i = 0; i < 4; i ++)
			{
				auto SubChar = GetChar(/*bLiteral=*/ true);
				WS.AppendChar(SubChar);
			}

			c = GetChar(/*bLiteral=*/ true);
			if (c != '\'')
			{
				FParserError::Throwf(TEXT("%s %s %s"), TEXT("Unterminated character constant"), *FileName, *GetLocation());
				UngetChar();
			}
			
			// TODO parse unicode char
			UE_LOG(LogTemp, Warning, TEXT("UnHandled Unicode Char : \\%s"), *WS);
			Token->SetConstChar(FrontChar);
		}
		else
		{
			c = GetChar(/*bLiteral=*/ true);
			if (c != '\'')
			{
				FParserError::Throwf(TEXT("%s %s %s"), TEXT("Unterminated character constant"), *FileName, *GetLocation());
				UngetChar();
			}

		}
		Token->SetConstChar(ActualCharLiteral);

		return Token;
	}
	else if (c == '"')
	{
		// String constant.
		char Temp[FCppToken::MaxStringConstSize];
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
			if (Length >= FCppToken::MaxStringConstSize)
			{
				FParserError::Throwf("String constant exceeds maximum of %d characters", (int32)FCppToken::MaxStringConstSize);
				c = '\"';
				Length = ((int32)FCppToken::MaxStringConstSize) - 1;
				break;
			}
			c = GetChar(/*bLiteral=*/ true);
		}
		Temp[Length] = 0;

		if (c != '"')
		{
			FParserError::Throwf("Unterminated string constant: %s %s %s", Temp, *FileName, *GetLocation());
			UngetChar();
		}

		Token->SetConstString(Temp);
		return Token;
	}
	else
	{
		// Symbol.
		int32 Length = 0;
		Token->Identifier[Length++] = c;

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
			Token->Identifier[Length++] = d;
			if (c == '>' && d == '>')
			{
				if (GetChar() == '>')
				{
					Token->Identifier[Length++] = '>';
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

		Token->Identifier[Length] = 0;
		Token->TokenType = ECppTokenType::Symbol;

		return Token;
	}
}

Re::Vector<Re::SharedPtr<FCppToken>> FBaseParser::GetTokensUntil(Re::Func<bool(FCppToken&)> Condition, bool bNoConst, const Re::String& DebugMessage)
{
	Re::Vector<Re::SharedPtr<FCppToken>> Tokens;
	while(true)
	{
		auto CurrentToken = GetToken(bNoConst);

		if(CurrentToken == nullptr)
		{
			FParserError::Throwf("Exit Early !! %s at %s %s", *DebugMessage, *FileName, *GetLocation());
		}

		Tokens.push_back(CurrentToken);

		if(Condition(*CurrentToken))
		{
			break;
		}
	}

	return Tokens;
}

Re::Vector<Re::SharedPtr<FCppToken>> FBaseParser::GetTokenUntilMatch(const char Match, bool bNoConst, const Re::String& DebugMessage)
{
	Re::Vector<Re::SharedPtr<FCppToken>> Tokens;
	while (true)
	{
		auto CurrentToken = GetToken(bNoConst);
		
		if(CurrentToken == nullptr)
		{
			FParserError::Throwf(TEXT("Exit Early !! %s at %s %s"), *DebugMessage, *FileName, *GetLocation());
		}
		
		Tokens.push_back(CurrentToken);

		if (CurrentToken->Matches(Match))
		{
			break;
		}
	}

	return Tokens;
}

Re::Vector<Re::SharedPtr<FCppToken>> FBaseParser::GetTokenUntilMatch(const char* Match, bool bNoConst, const Re::String& DebugMessage)
{
	Re::Vector<Re::SharedPtr<FCppToken>> Tokens;
	while (true)
	{
		auto CurrentToken = GetToken(bNoConst);
		if(CurrentToken == nullptr)
		{
			FParserError::Throwf(TEXT("Exit Early !! %s at %s %s"), *DebugMessage, *FileName, *GetLocation());
		}

		Tokens.push_back(CurrentToken);

		if (CurrentToken->Matches(Match))
		{
			break;
		}
	}

	return Tokens;
}
Re::Vector<Re::SharedPtr<FCppToken>> FBaseParser::GetTokensUntilPairMatch(const char Left, const char Right, const Re::String& DebugMessage)
{
	int MatchCount = 1;
	Re::Vector<Re::SharedPtr<FCppToken>> Tokens;
	while(true)
	{
		auto CurrentToken = GetToken(false);
		if(CurrentToken == nullptr)
		{
			FParserError::Throwf(TEXT("Exit Early !! %s at %s %s"), *DebugMessage, *FileName, *GetLocation());
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

void FBaseParser::UngetToken(const Re::SharedPtr<FCppToken>& Token)
{
	InputPos = Token->StartPos;
	InputLine = Token->StartLine;
}

Re::SharedPtr<FCppToken> FBaseParser::GetIdentifier(bool bNoConsts)
{
	auto Token = GetToken(bNoConsts);
	if (!Token)
	{
		return nullptr;
	}

	if (Token->TokenType == ECppTokenType::Identifier)
	{
		return Token;
	}

	UngetToken(Token);
	return nullptr;
}

Re::SharedPtr<FCppToken> FBaseParser::GetSymbol()
{
	auto Token = GetToken();
	if (!Token)
	{
		return nullptr;
	}

	if (Token->TokenType == ECppTokenType::Symbol)
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
		FParserError::Throwf("%s : Missing constant integer", Tag);
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
		FParserError::Throwf("%s : Missing constant integer", Tag);
	}

	return false;
}

bool FBaseParser::MatchIdentifier(const char* Match)
{
	auto Token = GetToken();
	if(Token != nullptr)
	{
		if(Token->GetTokenType() == ECppTokenType::Identifier &&
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
		if(Token->GetTokenType() == ECppTokenType::Const 
			&& (Token->GetConstType() == ECppTokenConstType::Int || Token->GetConstType() == ECppTokenConstType::Int64)
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
		if (Token->GetTokenType() == ECppTokenType::Const
			&& (Token->GetConstType() == ECppTokenConstType::Int 
				|| Token->GetConstType() == ECppTokenConstType::Int64))
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
	return Token->GetTokenType() == ECppTokenType::Identifier
		&& Token->GetTokenName() == Match;
}

bool FBaseParser::MatchSymbol(const char Match)
{
	auto Token = GetToken(true);
	if(Token)
	{
		if(Token->GetTokenType() == ECppTokenType::Symbol 
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
		if (Token->GetTokenType() == ECppTokenType::Symbol
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

bool FBaseParser::MatchToken(Re::Func<bool(const FCppToken&)> Condition)
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
			FParserError::Throwf("Missing ';' before %s", Token->GetRawTokenName());
		}
		else
		{
			FParserError::Throwf("Missing ';'");
		}
	}
}

FString FBaseParser::GetLocation() const
{
	return FString::Printf("%d:%d", InputLine, InputPos);
}

bool FBaseParser::PeekSymbol(const char Match)
{
	auto Token = GetToken(true);
	if(!Token)
	{
		return false;
	}
	UngetToken(Token);
	return Token->GetTokenType() == ECppTokenType::Symbol
		&& Token->GetRawTokenName()[0] == Match
		&& Token->GetRawTokenName()[1] == 0;
}

void FBaseParser::RequireIdentifier(const char* Match, const char* Tag)
{
	if(!MatchIdentifier(Match))
	{
		FParserError::Throwf(TEXT("Missing '%s' in %s"), Match, Tag);
	}
}

void FBaseParser::RequireSymbol(const char Match, const char* Tag)
{
	if(!MatchSymbol(Match))
	{
		FParserError::Throwf(TEXT("Missing %c in %s"), Match, Tag);
	}
}

void FBaseParser::RequireSymbol(const char Match, Re::Func<Re::String()> TagGetter)
{
	if(!MatchSymbol(Match))
	{
		FParserError::Throwf(TEXT("Missing %c in %s"), Match, *TagGetter());
	}
}

void FBaseParser::RequireConstInt(const char* Match, const char* Tag)
{
	if(!MatchConstInt(Match))
	{
		FParserError::Throwf(TEXT("Missing integer '%s' in %s"), Match, Tag);
	}
}

void FBaseParser::RequireAnyConstInt(const char* Tag)
{
	if(!MatchAnyConstInt())
	{
		FParserError::Throwf("Missing integer in %s'", Tag);
	}
}
