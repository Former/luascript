// Copyright (c) 2009-2011 by Alexander Demin and Alexei Bezborodov

#pragma once

#include <string>
#include <vector>
#include <deque>
#include <sstream>
#include <exception>
#include <memory>
#include <windows.h> // only for PtrToInt and IntToPtr

extern "C" 
{
	#include "lua/lua.h"
	#include "lua/lualib.h"
	#include "lua/lauxlib.h"
}

#ifdef UNIT_TEST 
	class LuaStackChecker
	{
	public:
		LuaStackChecker(lua_State* a_LuaState, const char* a_FileName = "", int a_Line = 0);
		~LuaStackChecker();

	private:
		void OnFail(const std::string& a_Message);
		
		lua_State* 	m_LuaState;
		int 		m_TopValue;
		const char* m_FileName;
		int 		m_Line;
	};
#	define CHECK_LUA_STACK(a_LuaState) LuaStackChecker guad(a_LuaState, __FILE__, __LINE__)
#else 
#	define CHECK_LUA_STACK(a_LuaState)
#endif // UNIT_TEST

class LuaScript
{
public:
	LuaScript();
	~LuaScript();

	class iLuaArg
	{
	public:
		virtual ~iLuaArg() {}
		virtual void		Unpack(lua_State* a_LuaState, int a_ParamIndex) = 0;
		virtual void		Pack(lua_State* a_LuaState) = 0;
		virtual std::string AsString() = 0;
		virtual iLuaArg*	Clone() const = 0;
	};

	class Bool_LuaArg: public iLuaArg
	{
	public:
		typedef bool LuaValueType;

		Bool_LuaArg() : m_Value(false) { }
		Bool_LuaArg(bool a_Value) : m_Value(a_Value) { }

		virtual iLuaArg*	Clone() const { return new Bool_LuaArg(m_Value); }
		virtual void		Unpack(lua_State* a_LuaState, int a_ParamIndex);
		virtual void		Pack(lua_State* a_LuaState);
		std::string			AsString();
		const bool&			GetValue() const;
		void				SetValue(bool a_Value);

	private:
		bool m_Value;
	};

	class Int_LuaArg: public iLuaArg
	{
	public:
		typedef int LuaValueType;

		Int_LuaArg() : m_Value(0) { }
		Int_LuaArg(LuaValueType a_Value) : m_Value(a_Value) { }

		virtual iLuaArg*	Clone() const { return new Int_LuaArg(m_Value); }
		virtual void		Unpack(lua_State* a_LuaState, int a_ParamIndex);
		virtual void		Pack(lua_State* a_LuaState);
		std::string			AsString();
		const LuaValueType& GetValue() const;
		void				SetValue(LuaValueType a_Value);

	private:
		LuaValueType m_Value;
	};

	class String_LuaArg: public iLuaArg
	{
	public:
		typedef std::string LuaValueType;

		String_LuaArg() : m_Value() {}
		String_LuaArg(const LuaValueType& a_Value) : m_Value(a_Value) {}

		virtual iLuaArg*	Clone() const { return new String_LuaArg(m_Value); }
		virtual void		Unpack(lua_State* a_LuaState, int a_ParamIndex);
		virtual void		Pack(lua_State* a_LuaState);
		std::string			AsString();
		const LuaValueType& GetValue() const;
		void				SetValue(LuaValueType a_Value);

	private:
		LuaValueType m_Value;
	};

	class VectorInt_LuaArg: public iLuaArg
	{
	public:
		typedef std::vector<int> LuaValueType;

		VectorInt_LuaArg() : m_Value() {}
		VectorInt_LuaArg(const LuaValueType& a_Value) : m_Value(a_Value) {}

		virtual iLuaArg*	Clone() const { return new VectorInt_LuaArg(m_Value); }
		virtual void		Unpack(lua_State* a_LuaState, int a_ParamIndex);
		virtual void		Pack(lua_State* a_LuaState);
		std::string			AsString();
		const LuaValueType& GetValue() const;
		void				SetValue(LuaValueType a_Value);

	private:
		LuaValueType m_Value;
	};

	class VectorString_LuaArg: public iLuaArg
	{
	public:
		typedef std::vector<std::string> LuaValueType;

		VectorString_LuaArg() : m_Value() {}
		VectorString_LuaArg(const LuaValueType& a_Value) : m_Value(a_Value) {}

		virtual iLuaArg*	Clone() const { return new VectorString_LuaArg(m_Value); }
		virtual void		Unpack(lua_State* a_LuaState, int a_ParamIndex);
		virtual void		Pack(lua_State* a_LuaState);
		std::string			AsString();
		const LuaValueType& GetValue() const;
		void				SetValue(LuaValueType a_Value);

	private:
		LuaValueType m_Value;
	};

	class LuaArgArray: public std::deque<iLuaArg*>
	{
	public:
		LuaArgArray() { clear(); }
		LuaArgArray(const LuaArgArray& rhs);
		virtual ~LuaArgArray();

		LuaArgArray*	Clone() const;
		void			PopFromLua(lua_State* a_LuaState);
		void			PushToLua(lua_State* a_LuaState);
		LuaArgArray&	Add(iLuaArg* arg);
	};

	template<class LuaImplFuncType>
	static int LuaCallback(lua_State* a_LuaState);

	class LuaException : public std::exception 
	{
	public:
		explicit LuaException(const std::string& msg);
		virtual ~LuaException() throw() { }
		
		const char*			What() const throw() { return m_Message.c_str(); }
		int					Line() const throw() { return m_Line; }
		const std::string&	Error() const throw() { return m_Error; }

	private:
		std::string		m_Message;
		int				m_Line;
		std::string		m_Error;
	};

	void		Execute(const std::string& script);

	template<class LuaArgType>
	LuaArgType	GetVariable(const std::string& a_Name);

	template<class LuaArgType>
	void		SetVariable(const std::string& a_Name, const typename LuaArgType::LuaValueType& a_Value);

	template<class LuaArgType>
	void		RegisterFunction();

protected:
	LuaScript(const LuaScript&);
	void operator = (const LuaScript&);

private:
	class LuaDeleter
	{
	public:
		template <typename LuaArgType> void operator() (const LuaArgType* a_Item) const 
		{
			delete a_Item;
		}
	};

	lua_State* m_LuaState;
};

template<class LuaImplFuncType>
class LuaFunc 
{
public:
	static const LuaScript::LuaArgArray* GetInputArgs() 
	{
		static LuaScript::LuaArgArray* args = 0;
		if( !args )
		{
			args = const_cast<LuaScript::LuaArgArray *>(LuaImplFuncType::GetInputArgs());
			if( !LuaImplFuncType::NameSpace().empty() )
				args->push_front(new LuaScript::Int_LuaArg());
		}
		return args;
	}

	static const LuaScript::LuaArgArray* GetOutputArgs()
	{
		static LuaScript::LuaArgArray* args = 0;
		if( !args )
			args = const_cast<LuaScript::LuaArgArray *>(LuaImplFuncType::GetOutputArgs());
		return args;
	}

	static const std::string NameSpace()
	{
		return LuaImplFuncType::NameSpace();
	}
	
	static const std::string Name()
	{
		return LuaImplFuncType::Name();
	}

	static void Calc(const LuaScript::LuaArgArray& in, LuaScript::LuaArgArray& out)
	{
		LuaImplFuncType::Calc(in, out);
	}
};

template<class LuaImplFuncType>
int LuaScript::LuaCallback(lua_State* a_LuaState) 
{
	const std::string sNameSpace	= LuaFunc<LuaImplFuncType>::NameSpace();
	const std::string sName			= LuaFunc<LuaImplFuncType>::Name();

	int argCount = lua_gettop(a_LuaState);
	int inputArgCount = static_cast<int>(LuaFunc<LuaImplFuncType>::GetInputArgs()->size());
	if( inputArgCount != argCount ) 
	{
		std::stringstream fmt;
		fmt << "function '" << sName << "'"
			<< " requires " << (sNameSpace.empty() ?  inputArgCount : inputArgCount - 1)
			<< " arguments, but " << (sNameSpace.empty() ? argCount : argCount - 1) << " given";
		throw LuaScript::LuaException(fmt.str());
	}

	std::auto_ptr<LuaScript::LuaArgArray> inArgs(LuaFunc<LuaImplFuncType>::GetInputArgs()->Clone());
	inArgs->PopFromLua(a_LuaState);
	std::auto_ptr<LuaScript::LuaArgArray> outArgs(LuaFunc<LuaImplFuncType>::GetOutputArgs()->Clone());

	LuaScript* parent = 0;
	if( !sNameSpace.empty() )
	{
		LuaScript::Int_LuaArg firstParametr = dynamic_cast<LuaScript::Int_LuaArg&>(*inArgs->at(0));
		parent = static_cast<LuaScript*>(IntToPtr(firstParametr.GetValue()));
		inArgs->pop_front();
	}

	LuaImplFuncType funcClass(parent);
	{
		CHECK_LUA_STACK(a_LuaState);
		funcClass.Calc(*inArgs, *outArgs);
	}
	outArgs->PushToLua(a_LuaState);
	return (int)(outArgs->size());
}

template<class LuaImplFuncType>
void LuaScript::RegisterFunction() 
{
	CHECK_LUA_STACK(m_LuaState);
	
	const std::string sNameSpace	= LuaFunc<LuaImplFuncType>::NameSpace();
	const std::string sName			= LuaFunc<LuaImplFuncType>::Name();
	if( sNameSpace.empty() )
	{
		lua_register(m_LuaState, sName.c_str(), LuaCallback<LuaImplFuncType>);
		return;
	}
	
	std::stringstream strThis;
	strThis << PtrToInt(this);

	std::string script = 
		std::string("if ") + sNameSpace + " == nil or " + sNameSpace + ".m_This == 0 then " + 
		sNameSpace + " = { m_This = " + strThis.str() + " }; "
		"end";
	Execute(script.c_str());
	
	lua_register(m_LuaState, "t_CurFunction", LuaCallback<LuaImplFuncType>);

	script = 
		sNameSpace + "." + sName + "_CppFunction = t_CurFunction; "
		"t_CurFunction = nil;";
	Execute(script.c_str());

	script = 
		std::string("function ") + sNameSpace + "." + sName + "(...) "
		"	local curArg = {}; "
		"	local sArgs = ''; "
		"	for ind, val in ipairs(arg) "
		"	do "
		"		curArg[ind] = val; "
		"		sArgs = sArgs..', curArg['..ind..']'; "
		"	end "
		"	return loadstring( "
		"				'return function (curArg) return " + sNameSpace + "." + sName + "_CppFunction(" + sNameSpace + ".m_This'..sArgs..' ); end'"
		"				)()(curArg); "
		"end ";
	
	Execute(script.c_str());
}

template<class LuaArgType>
LuaArgType LuaScript::GetVariable(const std::string& name) 
{
	CHECK_LUA_STACK(m_LuaState);

	lua_getglobal(m_LuaState, name.c_str());
	LuaArgType val;
	int index = lua_gettop(m_LuaState);
	val.Unpack(m_LuaState, index);
	lua_pop(m_LuaState, index);
	return val;
}

template<class LuaArgType>
void LuaScript::SetVariable(const std::string& a_Name, const typename LuaArgType::LuaValueType& a_Value) 
{
	LuaArgType var(a_Value);
	var.Pack(m_LuaState);
	lua_setglobal(m_LuaState, a_Name.c_str());
}

