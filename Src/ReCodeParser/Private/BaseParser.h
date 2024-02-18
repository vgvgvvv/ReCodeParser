#pragma once
#include "ReCppCommon.h"

class FCppToken;

class FBaseParser
{
public:
	using namespace Re;

	virtual ~FBaseParser() = default;


	void InitParserSource(const Re::String& InFileName, const char* SourceBuffer);

	// Basic Operations
	
	char GetChar(bool Literal = false);
	char PeekChar();
	char GetLeadingChar();
	void UngetChar();

	/**
	 * Tests if a character is an end-of-line character.
	 *
	 * @param	c	The character to test.
	 *
	 * @return	true if c is an end-of-line character, false otherwise.
	 */
	static bool IsEOL(char c);

	/**
	 * Tests if a character is a whitespace character.
	 *
	 * @param	c	The character to test.
	 *
	 * @return	true if c is an whitespace character, false otherwise.
	 */
	static bool IsWhitespace(char c);

	/** Clears out the stored comment. */
	void ClearComment();

public:

	/**
	 * \brief 
	 * \param bNoConsts ignore Const
	 * \return 
	 */
	Re::SharedPtr<FCppToken> GetToken(bool bNoConsts = false);

	Re::Vector<Re::SharedPtr<FCppToken>> GetTokensUntil(Re::Func<bool(FCppToken&)> Condition, bool bNoConst = false, const Re::String& DebugMessage = "");
	Re::Vector<Re::SharedPtr<FCppToken>> GetTokenUntilMatch(const char Match, bool bNoConst = false, const Re::String& DebugMessage = "");
	Re::Vector<Re::SharedPtr<FCppToken>> GetTokenUntilMatch(const char* Match, bool bNoConst = false, const Re::String& DebugMessage = "");
	Re::Vector<Re::SharedPtr<FCppToken>> GetTokensUntilPairMatch(const char Left, const char Right, const Re::String& DebugMessage = "");

	void UngetToken(const Re::SharedPtr<FCppToken>& Token);

	Re::SharedPtr<FCppToken>  GetIdentifier(bool bNoConsts = false);
	Re::SharedPtr<FCppToken> GetSymbol();

	// TODO
	bool GetConstInt(int32& Result, const char* Tag = NULL);
	bool GetConstInt64(int64& Result, const char* Tag = NULL);

	// Matching predefined text.
	bool MatchIdentifier(const char* Match);
	bool MatchConstInt(const char* Match);
	bool MatchAnyConstInt();
	bool PeekIdentifier(const char* Match);
	bool MatchSymbol(const char Match);
	bool MatchSymbol(const char* Match);
	bool MatchToken(Re::Func<bool(const FCppToken&)> Condition);
	bool MatchSemi();
	bool PeekSymbol(const char Match);

	// Requiring predefined text.
	void RequireIdentifier(const char* Match, const char* Tag);
	void RequireSymbol(const char Match, const char* Tag);
	void RequireSymbol(const char Match, Re::Func<Re::String()> TagGetter);
	void RequireConstInt(const char* Match, const char* Tag);
	void RequireAnyConstInt(const char* Tag);
	void RequireSemi();

public:

	Re::String GetLocation() const;


protected:

	// Input text
	const char* Input = nullptr;
	// Length of input text
	int32 InputLen = 0;
	// Current position in text
	int32 InputPos = 0;
	// Current line in text
	int32 InputLine = 0;
	// last GetChar line
	int32 PrevPos = 0;
	// last GetChar pos
	int32 PrevLine = 0;
	// Previous comment parsed by GetChar() call.
	Re::String PrevComment;
	// Number of statements parsed.
	int32 StatementsParsed = 0;
	// Total number of lines parsed.
	int32 LinesParsed = 0;

	Re::String FileName;
	
};
