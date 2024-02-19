#pragma once
#include <utility>

#include "Private/Internal/BaseParser.h"

namespace ReParser
{
	enum class IniSectionItemType
	{
		String,
		Single,
		List,
		Map
	};

	class IniSectionItem
	{
	public:
		virtual ~IniSectionItem() = default;
		virtual IniSectionItemType GetType() = 0;
	};

	class IniSection
	{
	public:
		explicit IniSection(Re::String name) : Name(std::move(name)) {}
		Re::String Name;
		Re::Map<Re::String, Re::UniquePtr<IniSectionItem>> Properties;
	};

	class IniFile : public ICodeFile
	{
	private:
		IniFile(const Re::String& filePath)
			: FilePath(filePath)
			, Content()
		{

		}
	public:

		const Re::String& GetFilePath() const override { return FilePath; }
		const Re::String& GetContent() const override { return Content; }

		Re::String FilePath;
		Re::String Content;
		Re::Map<Re::String, IniSection> Sections;
	};

	class IniParser : public BaseParserWithFile
	{

	};

}
