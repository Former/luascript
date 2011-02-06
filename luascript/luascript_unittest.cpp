// Copyright (c) 2009 by Alexander Demin

#include "gtest/gtest.h"

#include <fstream>
#include <string>

#include "luascript/luascript.h"

TEST(LuaScript, UrlParsingExample) {
  try {
    lua script;
    script.set_variable<lua::string_arg_t>(
      "url",
      "URL:host=live.system,user=test");
    std::string filter =
      "if url:find('user=test') then "
      "url = url:gsub('(host=)[^.]*', '%1test') end";
    script.exec(filter);
    EXPECT_EQ(std::string("URL:host=test.system,user=test"),
              script.get_variable<lua::string_arg_t>("url").value());
  } catch(const lua::exception& e) {
    FAIL() << "error: " << e.error() << ", line " << e.line();
  }
}

TEST(LuaScript, SyntaxErrorUnknownFunction) {
  try {
    lua script;
    script.exec("exists = fs.file_exists('test')");
  } catch(const lua::exception& e) {
    EXPECT_EQ(std::string("attempt to index global 'fs' (a nil value)"),
              e.error());
    EXPECT_EQ(1, e.line());
  }
}

TEST(LuaScript, SetGetVariable) {
  try {
    lua script;

    script.set_variable<lua::string_arg_t>("a", "test");
    script.exec("b = a;");
    EXPECT_EQ(std::string("test"),
              script.get_variable<lua::string_arg_t>("b").value());
    script.set_variable<lua::int_arg_t>("a", 100);
    script.exec("b = a;");
    EXPECT_EQ(100, script.get_variable<lua::int_arg_t>("b").value());
    script.set_variable<lua::bool_arg_t>("a", true);
    script.exec("b = a;");
    EXPECT_EQ(true, script.get_variable<lua::bool_arg_t>("b").value());
    script.set_variable<lua::bool_arg_t>("a", false);
    script.exec("b = a;");
    EXPECT_EQ(false, script.get_variable<lua::bool_arg_t>("b").value());
  } catch(const lua::exception& e) {
    FAIL() << "error: " << e.error() << ", line " << e.line();
  }
}

TEST(LuaScript, RequirePackageBase64) {
  try {
    lua script;
    script.exec("package.path = package.path .. ';./lib/?.lua'");
    script.exec("require('base64'); a = base64.encode('test');");
    EXPECT_EQ(std::string("dGVzdA=="),
              script.get_variable<lua::string_arg_t>("a").value());
  } catch(const lua::exception& e) {
    FAIL() << "error: " << e.error() << ", line " << e.line();
  }
}

class file_exists_func_t {
 public:
  static const lua::args_t* in_args() {
    lua::args_t* args = new lua::args_t();
    args->add(new lua::string_arg_t());
    return args;
  }

  static const lua::args_t* out_args() {
    lua::args_t* args = new lua::args_t();
    args->add(new lua::bool_arg_t());
    return args;
  }

  static const std::string ns() { return "fs"; }
  static const std::string name() { return "file_exists"; }

  static void calc(const lua::args_t& in, lua::args_t& out) {
    std::string filename = dynamic_cast<lua::string_arg_t&>(*in[0]).value();
    std::ifstream is(filename.c_str());
    dynamic_cast<lua::bool_arg_t&>(*out[0]).value() = is.good();
  }
};

TEST(LuaScript, FileExistsFunction) {
  try {
    lua script;

    script.register_function< file_exists_func_t >();

    script.set_variable<lua::string_arg_t>("fname", "SConstruct");
    script.exec("exists = fs.file_exists(fname);");
    EXPECT_EQ(true, script.get_variable<lua::bool_arg_t>("exists").value());

    script.set_variable<lua::string_arg_t>("fname",
                              std::string("its_nonexisting_h"));
    script.exec("exists = fs.file_exists(fname);");
    EXPECT_EQ(false, script.get_variable<lua::bool_arg_t>("exists").value());
  } catch(const lua::exception& e) {
    FAIL() << "error: " << e.error() << ", line " << e.line();
  }
}

class return_list_func_t {
 public:
  static const lua::args_t* in_args() {
    lua::args_t* args = new lua::args_t();
    return args;
  }

  static const lua::args_t* out_args() {
    lua::args_t* args = new lua::args_t();
    args->add(new lua::bool_arg_t());
    args->add(new lua::int_arg_t());
    args->add(new lua::string_arg_t());
    return args;
  }

  static const std::string ns() { return "test"; }
  static const std::string name() { return "return_list"; }

  static void calc(const lua::args_t& in, lua::args_t& out) {
    dynamic_cast<lua::bool_arg_t&>(*out[0]).value() = true;
    dynamic_cast<lua::int_arg_t&>(*out[1]).value() = 100;
    dynamic_cast<lua::string_arg_t&>(*out[2]).value() = "test";
  }
};

TEST(LuaScript, ReturnListFunction) {
  try {
    lua script;
    script.register_function< return_list_func_t >();
    script.exec("b, i, s = test.return_list();");
    EXPECT_EQ(true, script.get_variable<lua::bool_arg_t>("b").value());
    EXPECT_EQ(100, script.get_variable<lua::int_arg_t>("i").value());
    EXPECT_EQ("test", script.get_variable<lua::string_arg_t>("s").value());
  } catch(const lua::exception& e) {
    FAIL() << "error: " << e.error() << ", line " << e.line();
  }
}

class return_equal_func_t {
public:
  static const lua::args_t* in_args() {
    lua::args_t* args = new lua::args_t();
    args->add(new lua::bool_arg_t());
    args->add(new lua::int_arg_t());
    args->add(new lua::string_arg_t());
    args->add(new lua::bool_arg_t());
    return args;
  }

  static const lua::args_t* out_args() {
    lua::args_t* args = new lua::args_t();
    args->add(new lua::bool_arg_t());
    args->add(new lua::int_arg_t());
    args->add(new lua::string_arg_t());
    args->add(new lua::bool_arg_t());
    return args;
  }

  static const std::string ns() { return "test"; }
  static const std::string name() { return "return_list"; }

  static void calc(const lua::args_t& in, lua::args_t& out) {
    dynamic_cast<lua::bool_arg_t&>(*out[0]).value() = 
                  dynamic_cast<lua::bool_arg_t&>(*in[0]).value();
    dynamic_cast<lua::int_arg_t&>(*out[1]).value() = 
                  dynamic_cast<lua::int_arg_t&>(*in[1]).value();
    dynamic_cast<lua::string_arg_t&>(*out[2]).value() = 
                  dynamic_cast<lua::string_arg_t&>(*in[2]).value();
    dynamic_cast<lua::bool_arg_t&>(*out[3]).value() = 
                  dynamic_cast<lua::bool_arg_t&>(*in[3]).value();
  }
};

TEST(LuaScript, MultiReturnEqualFunction) {
  try {
    lua script;
    script.register_function< return_equal_func_t >();
    script.exec("b, i, s, b1 = test.return_list(true, 10, \"test\", false);");
    EXPECT_EQ(true, script.get_variable<lua::bool_arg_t>("b").value());
    EXPECT_EQ(10, script.get_variable<lua::int_arg_t>("i").value());
    EXPECT_EQ("test", script.get_variable<lua::string_arg_t>("s").value());
    EXPECT_EQ(false, script.get_variable<lua::bool_arg_t>("b1").value());
  } catch(const lua::exception& e) {
    FAIL() << "error: " << e.error() << ", line " << e.line();
  }
}
