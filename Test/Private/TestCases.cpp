#include "TestCases.h"

#include <filesystem>

#include "IniParser.h"
#include "../../Src/ReCodeParser/Public/BNFParser.h"

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