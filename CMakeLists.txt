cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 11)  # enable C++11 standard
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")

if(UNIX)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -no-pie")
endif(UNIX)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++ -static")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS} -g")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS} -O3")

project(hspp)
set(PROJECT_VERSION "0.1")

add_executable(
	${PROJECT_NAME}

	LICENSE
	README.md
	
	src/defs.h
	src/Help.h

	src/config/defs.h
	src/config/ConfigFile.h
	src/config/Config.h
	src/config/Config.cpp
	src/Console.h
	src/Console.cpp
	src/DataContainer.h
	src/DataContainer.cpp
	src/HashTable.h
	src/HashTable.cpp
	src/String.h
	src/String.cpp

	src/main.h
	src/main.cpp

	src/filesystem/FileSystem.h
	src/filesystem/FileSystem.cpp
	src/filesystem/FileSystemWorker.h
	src/filesystem/FileSystemWorker.cpp

	src/general/visitor/VisitorInterface.h

	src/general/tokenizer/elements/GeneralToken.h
	src/general/tokenizer/elements/GeneralToken.cpp
	src/general/tokenizer/GeneralTokenizer.h
	src/general/tokenizer/GeneralTokenizer.cpp

	src/general/parser/GeneralParser.h
	src/general/parser/GeneralParser.cpp

	src/general/minifier/GeneralMinifier.h
	src/general/minifier/GeneralMinifier.cpp

	src/general/modifier/IdentPair.h
	src/general/modifier/GeneralModifier.h
	src/general/modifier/GeneralModifier.cpp

	src/css/tokenizer/elements/CssToken.h
	src/css/tokenizer/elements/CssToken.cpp
	src/css/tokenizer/CssTokenizer.h
	src/css/tokenizer/CssTokenizer.cpp

	src/css/parser/elements/CssBaseElement.h
	src/css/parser/elements/CssBaseElement.cpp
	src/css/parser/elements/CssComment.h
	src/css/parser/elements/CssComment.cpp
	src/css/parser/elements/CssIdentifier.h
	src/css/parser/elements/CssIdentifier.cpp
	src/css/parser/elements/CssCustomProperty.h
	src/css/parser/elements/CssCustomProperty.cpp
	src/css/parser/elements/CssNumber.h
	src/css/parser/elements/CssNumber.cpp
	src/css/parser/elements/CssColor.h
	src/css/parser/elements/CssColor.cpp
	src/css/parser/elements/CssPercentage.h
	src/css/parser/elements/CssPercentage.cpp
	src/css/parser/elements/CssDimension.h
	src/css/parser/elements/CssDimension.cpp
	src/css/parser/elements/CssString.h
	src/css/parser/elements/CssString.cpp
	src/css/parser/elements/CssDelimiter.h
	src/css/parser/elements/CssDelimiter.cpp
	src/css/parser/elements/CssDeclaration.h
	src/css/parser/elements/CssDeclaration.cpp
	src/css/parser/elements/CssFunction.h
	src/css/parser/elements/CssFunction.cpp
	src/css/parser/elements/CssBlock.h
	src/css/parser/elements/CssBlock.cpp
	src/css/parser/elements/CssQualifiedRule.h
	src/css/parser/elements/CssQualifiedRule.cpp
	src/css/parser/elements/CssAtRule.h
	src/css/parser/elements/CssAtRule.cpp
	src/css/parser/elements/CssSelector.h
	src/css/parser/elements/CssSelector.cpp
	src/css/parser/elements/CssSelectorAttribute.h
	src/css/parser/elements/CssSelectorAttribute.cpp
	src/css/parser/elements/CssSelectorCombinator.h
	src/css/parser/elements/CssSelectorCombinator.cpp
	src/css/parser/elements/CssUnicodeRange.h
	src/css/parser/elements/CssUnicodeRange.cpp
	src/css/parser/elements/CssSupportsCondition.h
	src/css/parser/elements/CssSupportsCondition.cpp

	src/css/CssVendorPrefixes.h

	src/css/parser/includes.h
	src/css/parser/CssParser.h
	src/css/parser/CssParser.cpp

	src/css/minifier/CssMinifier.h
	src/css/minifier/CssMinifier.cpp

	src/css/modifier/CssColorTable.h
	src/css/modifier/CssModifier.h
	src/css/modifier/CssModifier.cpp

	src/css/generator/CssGenerator.h
	src/css/generator/CssGenerator.cpp
)

target_compile_definitions(
	${PROJECT_NAME} PUBLIC
	APP_AUTHOR="Waldemar Zimpel"
	APP_LICENSE="GNU GPLv3"
	APP_NAME="${PROJECT_NAME}"
	APP_FULL_NAME="HyperSheetsPreprocessor"
	APP_VERSION="${PROJECT_VERSION}"
	APP_RELEASE_YEAR="2019"
	CONFIG_FILE_PATH="${PROJECT_NAME}.ini"
)

target_link_libraries(hspp stdc++)
