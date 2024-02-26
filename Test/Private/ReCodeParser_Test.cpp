
#include "ReCodeParser_Test.h"
#include "CommonPrefix.h"
#include "ReCodeParser.h"

int main()
{
	auto iniFile = ReParser::Ini::IniFile::Parse("D:\\MyProjects\\TestProject\\TestCustomEngine\\Assets\\StreamingAssets\\IniConfig\\DefaultSetting.ini");
	RE_ASSERT(iniFile);
	return 0;
}
