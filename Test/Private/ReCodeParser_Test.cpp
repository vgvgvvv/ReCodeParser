
#include "ReCodeParser_Test.h"
#include "CommonPrefix.h"
#include "ReCodeParser.h"

int main()
{
	auto path = std::filesystem::path{__FILE__}.parent_path() / "Test.ini";
	auto iniFile = ReParser::Ini::IniFile::Parse(path.string());
	RE_ASSERT(iniFile);
	RE_LOG(iniFile->ToString())
	return 0;
}
