import os

sources = Split("""
luascript/luascript_unittest.cpp
luascript/luascript.cpp
luascript/lua/lua-files.c
Runner.cpp
gtest/gtest-all.cc
""")

MS_VC = "C:\\Program Files\\Microsoft Visual Studio 9.0\\VC"
VC_INC = MS_VC + "\\include"
VC_LIB = MS_VC + "\\lib"

MS_SDK = "C:\\Program Files\\Microsoft SDKs\\Windows\\v6.0a"
MS_SDK_INC = MS_SDK + "\\include"
MS_SDK_LIB = MS_SDK + "\\lib"

libs = Split("""
""")

env = Environment(CXXFLAGS = ['/Zi', '/EHsc', '/DWIN32', '/nologo'])

env.Program(
  "luascript_unittest_vs2008", 
  sources, 
  LIBS=libs, 
  LIBPATH=['.', VC_LIB, MS_SDK_LIB],
  CPPPATH = [VC_INC, MS_SDK_INC, '.', 'gtest']
)

