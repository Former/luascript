#include "luascript/luascript.h"

#include <algorithm>

lua::lua() {
  L_ = lua_open();
  luaL_openlibs(L_);
}

lua::~lua() {
  lua_close(L_); 
}

void lua::bool_arg_t::unpack(lua_State* L) {
  if (lua_isboolean(L, lua_gettop(L)))
    value_ = lua_toboolean(L, lua_gettop(L));
  else
    throw lua::exception("bool_arg_t::unpack(), value is not boolean");
}

void lua::bool_arg_t::pack(lua_State* L) {
  lua_pushboolean(L, value_);
}

std::string lua::bool_arg_t::asString() {
  std::stringstream fmt;
  fmt << value_;
  return fmt.str();
}

void lua::int_arg_t::unpack(lua_State* L) {
  if (lua_isnumber(L, lua_gettop(L)))
    value_ = lua_tointeger(L, lua_gettop(L));
  else
    throw lua::exception("int_arg_t::unpack(), value is not integer");
}

void lua::int_arg_t::pack(lua_State* L) {
  lua_pushinteger(L, value_);
}

std::string lua::int_arg_t::asString() {
  std::stringstream fmt;
  fmt << value_;
  return fmt.str();
}

void lua::string_arg_t::unpack(lua_State* L) {
  if (lua_isstring(L, lua_gettop(L)))
    value_ = lua_tostring(L, lua_gettop(L));
  else
    throw lua::exception("string_arg_t::unpack(), value is not string");
}

void lua::string_arg_t::pack(lua_State* L) {
  lua_pushstring(L, value_.c_str());
}

std::string lua::string_arg_t::asString() {
  return value_;
}

lua::args_t::args_t(const lua::args_t& rhs) {
  clear();
  for (const_iterator i = rhs.begin(); i != rhs.end(); ++i)
    push_back((*i)->clone());
}

lua::args_t::~args_t() {
  std::for_each(begin(), end(), deleter());
}

lua::args_t& lua::args_t::add(arg_t* arg) { 
  push_back(arg); 
  return *this; 
}

void lua::args_t::unpack(lua_State* L) {
  for (const_iterator i = begin(); i != end(); ++i)
    (*i)->unpack(L);
}

void lua::args_t::pack(lua_State* L) {
  for (args_t::const_iterator i = begin(); i != end(); ++i)
    (*i)->pack(L);
}

lua::args_t* lua::args_t::clone() const {
  lua::args_t* copy = new args_t();
  for (const_iterator i = begin(); i != end(); ++i)
    copy->push_back((*i)->clone());
  return copy;
}

lua::exception::exception(const std::string& msg) : msg_(msg), error_(msg) {
  size_t i = msg.find("]:");
  if (i == std::string::npos)
    line_ = 0;
  else {
    std::sscanf(msg.c_str() + i + 2, "%d", &line_);
    i = msg.rfind(": ");
    if (i != std::string::npos)
      error_ = msg.substr(i + 2);
  }
}

void lua::exec(const std::string& script) {
  int error = luaL_dostring(L_, script.c_str());
  if (error)
    throw lua::exception(lua_tostring(L_, -1));
}
