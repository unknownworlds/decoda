solution "Decoda"
    configurations { "Debug", "Release" }
    location "build"
	--debugdir "working"
	flags { "No64BitChecks" }
    
	defines { "_CRT_SECURE_NO_WARNINGS", "WIN32" }
    
    vpaths { 
        ["Header Files"] = { "**.h" },
        ["Source Files"] = { "**.cpp" },
		["Resource Files"] = { "**.rc" },
    }

project "Frontend"
    kind "WindowedApp"
	flags { "WinMain" }
    location "build"
    language "C++"
    files {
		"src/Frontend/*.h",
		"src/Frontend/*.cpp",
		"src/Frontend/*.rc",
	}		
    includedirs {
		"src/Shared",
		"libs/wxWidgets/include",
		"libs/wxScintilla/include",
		"libs/Update/include",
	}
	libdirs {
		"libs/wxWidgets/lib/vc_lib",
		"libs/wxScintilla/lib",
		"libs/Update/lib",
	}
    links {
		"comctl32",
		"rpcrt4",
	}

    configuration "Debug"
        defines { "DEBUG" }
        flags { "Symbols" }
        targetdir "bin/debug"
		includedirs { "libs/wxWidgets/lib/vc_lib/mswd" }
		links {
			"wxbase28d",
			"wxmsw28d_core",
			"wxmsw28d_aui",
			"wxscintillad",
			"wxbase28d_xml",
			"wxexpatd",
			"wxmsw28d_adv",
			"wxmsw28d_qa",
			"wxzlibd",
			"wxmsw28d_richtext",
			"wxmsw28d_html",
			"wxpngd",
			"imagehlp",
			"Update",
			"Shared",
		}

    configuration "Release"
        defines { "NDEBUG" }
        flags { "Optimize" }
        targetdir "bin/release"
		
project "LuaInject"
    kind "SharedLib"
    location "build"
    language "C++"
	defines { "TIXML_USE_STL" }
    files {
		"src/LuaInject/*.h",
		"src/LuaInject/*.cpp",
	}		
    includedirs {
		"src/Shared",
		"libs/lua/include",
		"libs/LuaPlus/include",
		"libs/tinyxml/include",
		"libs/dbghlp/include",
	}
	libdirs {
		"libs/tinyxml/lib",
		"libs/dbghlp/lib",
	}
    links {
		"Shared",
		"psapi",
	}

    configuration "Debug"
        defines { "DEBUG" }
        flags { "Symbols" }
        targetdir "bin/debug"
		links { "tinyxmld_STL" }

    configuration "Release"
        defines { "NDEBUG" }
        flags { "Optimize" }
        targetdir "bin/release"				
		links { "tinyxml_STL" }
		
project "Shared"
    kind "StaticLib"
    location "build"
    language "C++"
    files {
		"src/Shared/*.h",
		"src/Shared/*.cpp",
	}		
    includedirs {
	}
	libdirs {
	}
    links {
	}

    configuration "Debug"
        defines { "DEBUG" }
        flags { "Symbols" }
        targetdir "bin/debug"

    configuration "Release"
        defines { "NDEBUG" }
        flags { "Optimize" }
        targetdir "bin/release"		
		