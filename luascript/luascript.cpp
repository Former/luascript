﻿// Copyright (c) 2009-2011 by Alexander Demin and Alexei Bezborodov

#include "luascript/luascript.h"
#include <algorithm>

#ifdef UNIT_TEST 
#  include "gtest/gtest.h"

  LuaStackChecker::LuaStackChecker(lua_State* a_LuaState,
                                const char* a_FileName, int a_Line):
    m_LuaState(a_LuaState),
    m_FileName(a_FileName),
    m_Line(a_Line)
  {
    m_TopValue = lua_gettop(m_LuaState);
  }

  LuaStackChecker::~LuaStackChecker()
  {
    int newTop = lua_gettop(m_LuaState);
    if(m_TopValue != newTop)
    {
      std::stringstream fmt;
      fmt << "Lua stack corrupted!\n"
        "File " << m_FileName << " line " << m_Line << "\n"
        "last_top = " << m_TopValue << " new_top " << newTop;
      OnFail(fmt.str());
    }
  }
  
  void LuaStackChecker::OnFail(const std::string& a_Message)
  {
    FAIL() << a_Message;
  }
#endif // UNIT_TEST

LuaScript::LuaScript() 
{
  m_LuaState = lua_open();
  luaL_openlibs(m_LuaState);
}

LuaScript::~LuaScript() 
{
  lua_close(m_LuaState); 
}

LuaScript::iLuaArg* LuaScript::Bool_LuaArg::Clone() const
{ 
  return new Bool_LuaArg(m_Value);
}

void LuaScript::Bool_LuaArg::Unpack(lua_State* a_LuaState, int a_ParamIndex)
{
  CHECK_LUA_STACK(a_LuaState);

  if( lua_isboolean(a_LuaState, a_ParamIndex) )
    m_Value = lua_toboolean(a_LuaState, a_ParamIndex) ? true : false;
  else
    throw LuaException("Bool_LuaArg::Unpack(), value is not boolean");
}

void LuaScript::Bool_LuaArg::Pack(lua_State* a_LuaState)
{
  lua_pushboolean(a_LuaState, m_Value);
}

std::string LuaScript::Bool_LuaArg::AsString() const
{
  std::stringstream fmt;
  fmt << m_Value;
  return fmt.str();
}

const LuaScript::Bool_LuaArg::LuaValueType& 
LuaScript::Bool_LuaArg::GetValue() const
{ 
  return m_Value; 
}

void 
LuaScript::Bool_LuaArg::SetValue(LuaValueType a_Value)
{
  m_Value = a_Value;
}

LuaScript::iLuaArg* LuaScript::Int_LuaArg::Clone() const
{ 
  return new Int_LuaArg(m_Value);
}

void LuaScript::Int_LuaArg::Unpack(lua_State* a_LuaState, int a_ParamIndex)
{
  CHECK_LUA_STACK(a_LuaState);
  
  if( lua_isnumber(a_LuaState, a_ParamIndex) )
    m_Value = LuaValueType(lua_tointeger(a_LuaState, a_ParamIndex));
  else
    throw LuaException("Int_LuaArg::Unpack(), value is not integer");
}

void LuaScript::Int_LuaArg::Pack(lua_State* a_LuaState)
{
  lua_pushinteger(a_LuaState, m_Value);
}

std::string LuaScript::Int_LuaArg::AsString() const
{
  std::stringstream fmt;
  fmt << m_Value;
  return fmt.str();
}

const LuaScript::Int_LuaArg::LuaValueType& 
LuaScript::Int_LuaArg::GetValue() const
{ 
  return m_Value; 
}

void 
LuaScript::Int_LuaArg::SetValue(LuaValueType a_Value)
{
  m_Value = a_Value;
}

LuaScript::iLuaArg* LuaScript::String_LuaArg::Clone() const
{ 
  return new String_LuaArg(m_Value);
}

void LuaScript::String_LuaArg::Unpack(lua_State* a_LuaState, int a_ParamIndex)
{
  CHECK_LUA_STACK(a_LuaState);

  if( lua_isstring(a_LuaState, a_ParamIndex) )
    m_Value = lua_tostring(a_LuaState, a_ParamIndex);
  else
    throw LuaException("String_LuaArg::Unpack(), value is not string");
}

void LuaScript::String_LuaArg::Pack(lua_State* a_LuaState)
{
  lua_pushstring(a_LuaState, m_Value.c_str());
}

std::string LuaScript::String_LuaArg::AsString() const
{
  return m_Value;
}

const LuaScript::String_LuaArg::LuaValueType& 
LuaScript::String_LuaArg::GetValue() const
{ 
  return m_Value; 
}

void LuaScript::String_LuaArg::SetValue(LuaValueType a_Value)
{
  m_Value = a_Value;
}

LuaScript::iLuaArg* LuaScript::VectorInt_LuaArg::Clone() const
{
  return new VectorInt_LuaArg(m_Value);
}

void LuaScript::VectorInt_LuaArg::Unpack(lua_State* a_LuaState,
                                                int a_ParamIndex)
{
  CHECK_LUA_STACK(a_LuaState);

  if( !lua_istable(a_LuaState, a_ParamIndex) )
    throw LuaException("VectorInt_LuaArg::Unpack(), value is not table");

  lua_pushvalue(a_LuaState, a_ParamIndex);
  
  m_Value.clear();
  const int count = luaL_getn(a_LuaState, -1);
  for( int i = 1; i <= count; ++i )
  {
    lua_pushnumber(a_LuaState, i);
    lua_gettable(a_LuaState, -2);
    
    if( !lua_isnumber(a_LuaState, -1) )
    {
      std::stringstream fmt;
      fmt << "VectorInt_LuaArg::Unpack(), value [" << i << "] is not number";
      throw LuaException(fmt.str());
    }
    
    int val = static_cast<int>(lua_tonumber(a_LuaState, -1));
    m_Value.push_back(val);
    lua_pop(a_LuaState, 1);
  }
  lua_pop(a_LuaState, 1);
}

void LuaScript::VectorInt_LuaArg::Pack(lua_State* a_LuaState)
{
  lua_newtable(a_LuaState);
  const size_t size = m_Value.size();
  for( size_t i = 0; i < m_Value.size() ; ++i )
  {
    lua_pushnumber(a_LuaState, i + 1);
    lua_pushnumber(a_LuaState, m_Value[i]);
    lua_settable(a_LuaState, -3);
  }
}

std::string LuaScript::VectorInt_LuaArg::AsString() const
{
  std::stringstream fmt;
  fmt << &m_Value;
  return fmt.str();
}

const LuaScript::VectorInt_LuaArg::LuaValueType&
LuaScript::VectorInt_LuaArg::GetValue() const
{ 
  return m_Value; 
}

void LuaScript::VectorInt_LuaArg::SetValue(LuaValueType a_Value)
{
  m_Value = a_Value;
}

LuaScript::iLuaArg* LuaScript::VectorString_LuaArg::Clone() const
{
  return new VectorString_LuaArg(m_Value);
}

void LuaScript::VectorString_LuaArg::Unpack(lua_State* a_LuaState,
                                                    int a_ParamIndex)
{
  CHECK_LUA_STACK(a_LuaState);

  if( !lua_istable(a_LuaState, a_ParamIndex) )
    throw LuaException("VectorString_LuaArg::Unpack(), value is not table");

  lua_pushvalue(a_LuaState, a_ParamIndex);
  
  m_Value.clear();
  const int count = luaL_getn(a_LuaState, -1);
  for( int i = 1; i <= count; ++i )
  {
    lua_pushnumber(a_LuaState, i);
    lua_gettable(a_LuaState, -2);
    
    if( !lua_isstring(a_LuaState, -1) )
    {
      std::stringstream fmt;
      fmt << "VectorString_LuaArg::Unpack(), "
        "value [" << i << "] is not string";
      throw LuaException(fmt.str());
    }
    
    std::string val = lua_tostring(a_LuaState, -1);
    m_Value.push_back(val);
    lua_pop(a_LuaState, 1);
  }
  lua_pop(a_LuaState, 1);
}

void LuaScript::VectorString_LuaArg::Pack(lua_State* a_LuaState)
{
  lua_newtable(a_LuaState);
  const size_t size = m_Value.size();
  for( size_t i = 0; i < m_Value.size() ; ++i )
  {
    lua_pushnumber(a_LuaState, i + 1);
    lua_pushstring(a_LuaState, m_Value[i].c_str());
    lua_settable(a_LuaState, -3);
  }
}

std::string LuaScript::VectorString_LuaArg::AsString() const
{
  std::stringstream fmt;
  fmt << &m_Value;
  return fmt.str();
}

const LuaScript::VectorString_LuaArg::LuaValueType& 
LuaScript::VectorString_LuaArg::GetValue() const
{ 
  return m_Value; 
}

void LuaScript::VectorString_LuaArg::SetValue(LuaValueType a_Value)
{
  m_Value = a_Value;
}

LuaScript::LuaArgArray::LuaArgArray(const LuaScript::LuaArgArray& rhs)
{
  clear();
  for( const_iterator i = rhs.begin(); i != rhs.end(); ++i )
    push_back((*i)->Clone());
}

LuaScript::LuaArgArray::~LuaArgArray()
{
  std::for_each(begin(), end(), LuaDeleter());
}

LuaScript::LuaArgArray& LuaScript::LuaArgArray::Add(iLuaArg* arg)
{ 
  push_back(arg); 
  return *this; 
}

void LuaScript::LuaArgArray::PopFromLua(lua_State* a_LuaState)
{
  for( size_t i = 0; i < size(); ++i )
    this->at(i)->Unpack(a_LuaState, (int)(i + 1));
}

void LuaScript::LuaArgArray::PushToLua(lua_State* a_LuaState)
{
  for( LuaArgArray::const_iterator i = begin(); i != end(); ++i )
    (*i)->Pack(a_LuaState);
}

LuaScript::LuaArgArray* LuaScript::LuaArgArray::Clone() const
{
  LuaScript::LuaArgArray* copy = new LuaArgArray();
  for( const_iterator i = begin(); i != end(); ++i )
    copy->push_back((*i)->Clone());
  return copy;
}

LuaScript::LuaException::LuaException(const std::string& a_Message):
  m_Message(a_Message),
  m_Error(a_Message)
{
  size_t i = a_Message.find("]:");
  if( i == std::string::npos )
    m_Line = 0;
  else
  {
    std::sscanf(a_Message.c_str() + i + 2, "%d", &m_Line);
    i = a_Message.rfind(": ");
    if( i != std::string::npos )
      m_Error = a_Message.substr(i + 2);
  }
}

void LuaScript::Execute(const std::string& a_ScriptString)
{
  int nError = luaL_dostring(m_LuaState, a_ScriptString.c_str());
  if( nError )
    throw LuaException(lua_tostring(m_LuaState, -1));
}
