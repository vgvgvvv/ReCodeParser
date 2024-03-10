#include "TestCases.h"

#include <filesystem>

#include "IniParser.h"
#include "BNFParser.h"
#include "ASTParser/Nodes.h"

void TestIni()
{
	auto path = std::filesystem::path{__FILE__}.parent_path() / "Test.ini";
	auto iniFile = ReParser::Ini::IniFile::Parse(path.string());
	RE_ASSERT(iniFile);
	RE_LOG(iniFile->ToString())
}

void TestBNF()
{
	auto path = std::filesystem::path{__FILE__}.parent_path() / "Test.bnf";
	auto bnfFile = ReParser::BNF::BNFFile::Parse(path.string());
	RE_ASSERT(bnfFile);
	RE_LOG(bnfFile->ToString())
}

namespace ReParser::AST
{
	class VariableNodeParser : public ASTNodeParser
	{
		DECLARE_DERIVED_CLASS(VariableNodeParser, ASTNodeParser)
	public:
		bool Parse(ReParser::ICodeFile* file, ReParser::AST::ASTParser& context, const ReParser::Token& token, ReParser::AST::ASTNodePtr* outNode) override
		{
			using namespace ReParser;
			if(token.GetTokenType() == ReParser::ETokenType::Identifier)
			{
				*outNode = AST::CreateASTNode<AST::IdentifierNode>(token);
				return true;
			}
			return false;
		}

	};
	DEFINE_DERIVED_CLASS(VariableNodeParser, ASTNodeParser)
}


void TestASTParser()
{
	auto path = std::filesystem::path{__FILE__}.parent_path() / "Test.bnf";
	auto bnfFile = ReParser::BNF::BNFFile::Parse(path.string());

	auto parser = bnfFile->GenerateASTParser();
	parser->InitParserSource("{TestValue} > 100");
	parser->ParseWithoutFile();
	auto astTree = parser->GetASTTree();

	RE_LOG(astTree.ToString());
}