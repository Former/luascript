call cl2008.cmd
cl /MP4 /nologo /EHsc /I. /Feluascript_unittest_vs2008.exe /DUNIT_TEST /DWIN32 ^
  luascript\luascript.cpp luascript\luascript_unittest.cpp runner.cpp ^
  luascript\lua\lua-files.c gtest\gtest-all.cc
