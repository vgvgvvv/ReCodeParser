#pragma once
#include "ReCodeParserDefine.h"
#include "ReClassInfo.h"
#include "ReCppCommon.h"
#include "Token.h"

namespace ReParser
{
	class Token;
	class BaseParser;

	class IParsableFile
	{
        DECLARE_CLASS(IParsableFile)
	public:
		virtual ~IParsableFile() = default;
		virtual const Re::String& GetFilePath() const = 0;

		bool IsValid() const { return std::filesystem::exists(GetFilePath()); }
	};

	class ICodeFile : public IParsableFile
	{
        DECLARE_DERIVED_CLASS(ICodeFile, IParsableFile)
	public:
		virtual const Re::String& GetContent() const = 0;
		virtual void OnNextToken(BaseParser& parser, const Token& token) { }
	};

	class RECODEPARSER_API BaseParser
	{
	public:

		virtual ~BaseParser() = default;

		void InitParserSource(const char* SourceBuffer);

		virtual void InitParserSource(const Re::String& InFileName, const char* SourceBuffer);

		virtual bool ParseWithoutFile();

		virtual bool CompileDeclaration(const Token& token);
		// Basic Operations

		char GetChar(bool Literal = false);
		char PeekChar();
		char GetLeadingChar();
		void UngetChar();

		void SetError(const Re::String& str);
		bool GetError(Re::String& str);

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

	protected:

		/** Clears out the stored comment. */
		void ClearComment();

	    virtual bool IsBeginComment(char currentChar)
	    {
	        auto nextChar = PeekChar();
	        if (currentChar == '/' && nextChar == '*') return true;

	        return false;
	    }

	    virtual bool IsEndComment(char currentChar)
	    {
	        auto nextChar = PeekChar();
	        if (currentChar == '*' && nextChar == '/') return true;

	        return false;
	    }

	    virtual bool IsLineComment(char currentChar)
	    {
	        if (currentChar == '/' && PeekChar() == '/') return true;
	        return false;
	    }

	public:

		/**
		 * \brief
		 * \param bNoConsts ignore Const
		 * \return
		 */
		Re::SharedPtr<Token> GetToken(bool bNoConsts = false);

		Re::Vector<Re::SharedPtr<Token>> GetTokensUntil(Re::Func<bool(Token&)> Condition, bool bNoConst = false, const Re::String& DebugMessage = "");
		Re::Vector<Re::SharedPtr<Token>> GetTokenUntilMatch(const char Match, bool bNoConst = false, const Re::String& DebugMessage = "");
		Re::Vector<Re::SharedPtr<Token>> GetTokenUntilMatch(const char* Match, bool bNoConst = false, const Re::String& DebugMessage = "");
		Re::Vector<Re::SharedPtr<Token>> GetTokensUntilPairMatch(const char Left, const char Right, const Re::String& DebugMessage = "");

        void UngetToken(const Token& Token);
		void UngetToken(const Re::SharedPtr<Token>& Token);
		void ResetToToken(const Token& Token);

		Re::SharedPtr<Token>  GetIdentifier(bool bNoConsts = false);
		Re::SharedPtr<Token> GetSymbol();

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
		bool IsEndOfLine(int currentLine);
		bool MatchToken(Re::Func<bool(const Token&)> Condition);
		bool MatchSemi();
		bool PeekSymbol(char Match);

		// Requiring predefined text.
		bool RequireIdentifier(const char* Match, const char* Tag);
		bool RequireSymbol(char Match, const char* Tag);
		bool RequireSymbol(char Match, const Re::Func<Re::String()>& TagGetter);
		bool RequireConstInt(const char* Match, const char* Tag);
		bool RequireAnyConstInt(const char* Tag);
		bool RequireSemi();

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

		Re::Vector<Re::String> Errors;
	};

	class BaseParserWithFile : public BaseParser
	{
	public:

		void Parse(ICodeFile* file);

		virtual void PreParserProcess(ICodeFile* file) { }

		virtual void PostParserProcess(ICodeFile* file) { }

		Re::String GetFileLocation(ICodeFile* file) const
		{
			if(!file)
			{
				return "file: 'UNKNOWN' position : " + GetLocation();
			}
			else
			{
				return "file: '" + file->GetFilePath() + "' position : " + GetLocation();
			}
		}

	    bool CompileDeclaration(const Token& token) final
		{
			return CompileDeclaration(nullptr, token);
		}

		virtual bool CompileDeclaration(ICodeFile* file, const Token& token)
		{
			return true;
		}
	};

}
