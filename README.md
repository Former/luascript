### Introduction
    http://demin.ws/blog/russian/2009/06/12/lua-scripts-in-cpp/
    http://demin.ws/blog/russian/2011/03/08/lua-scripts-in-cpp-version-2/


### Calling Lua code from C++ code

This example passes the variable a set to "test" to the script, the script appends "123" to it and assigns the result to the variable b. The C++ program picks the variable b up and prints "test123".

	#include "luascript/luascript.h"
	...
	try
	{
		LuaScript script;
		script.SetVariable<LuaScript::String_LuaArg>("a", "test");
		script.Execute("b = a .. '123';");
		std::cout << script.GetVariable<LuaScript::String_LuaArg>("b").GetValue();
	}
	catch (LuaScript::LuaException& e)
	{
		std::cerr << "error: " << e.Error() << ", line " << e.Line();
	}

### Adding a new function

	class FileExistsFunction
	{
	public:
	
	// Constructor parametr - pointer to execute class - LuaScript
	FileExistsFunction(LuaScript* a_Parent)  {}
	
	// This method registers the function arguments.
	// Available type are:
	// - LuaScript::String_LuaArg
	// - LuaScript::Int_LuaArg
	// - LuaScript::Bool_LuaArg
	// - LuaScript::Double_LuaArg
	// - LuaScript::Vector_LuaArg<LuaScript::Any_LuaArg>
	static const LuaScript::LuaArgArray* GetInputArgs()
	{
		LuaScript::LuaArgArray* args = new LuaScript::LuaArgArray();
		args->Add(new LuaScript::String_LuaArg());
		return args;
	}
	
	// This method registers the set of return values.
	// Lua allows to have multiple return values.
	// Available type are:
	// - LuaScript::String_LuaArg
	// - LuaScript::Int_LuaArg
	// - LuaScript::Bool_LuaArg
	// - LuaScript::Double_LuaArg
	// - LuaScript::Vector_LuaArg<LuaScript::Any_LuaArg>
	static const LuaScript::LuaArgArray* GetOutputArgs()
	{
		LuaScript::LuaArgArray* args = new LuaScript::LuaArgArray();
		args->Add(new LuaScript::Bool_LuaArg());
		return args;
	}
	
	// These two methods set the function name: namespace and name.
	// In this example the function name is "fs.file_exists".
	static const std::string NameSpace()
	{
		return "fs";
	}
	static const std::string Name()
	{
		return "file_exists";
	}
	
	// This method performs the function logic. 
	// The method has to parse the arguments and fill in the array of
	// return values. The method must take care that it uses valid types of 
	// arguments and return values.
	static void Calc(const LuaScript::LuaArgArray& in, LuaScript::LuaArgArray& out)
	{
		std::string filename = dynamic_cast<LuaScript::String_LuaArg&>(*in[0]).GetValue();
		std::ifstream is(filename.c_str());
		dynamic_cast<LuaScript::Bool_LuaArg&>(*out[0]).SetValue(is.good());
	}
	};
	...
	try
	{
		const char filename[] = "readme.txt";
		// Create the script executor.
		LuaScript script;
		// Register new function "fs.file_exists()".
		script.RegisterFunction<FileExistsFunction>();
		// Set the variable "fname" to "readme.txt".
		script.SetVariable<LuaScript::String_LuaArg>("fname", filename);
		// Execute the script calling this function.
		script.Execute("exists = fs.file_exists(fname);");
		// Call "get_variable" method to extract the value of the
		// "exists" variable.
		bool exists = script.GetVariable<LuaScript::Bool_LuaArg>("exists").GetValue();
		std::cout << "file: " << filename << " " << (exists ? "exist" : "not exist");
	}
	catch (LuaScript::LuaException& e) 
	{
		std::cerr << "error: " << e.Error() << ", line " << e.Line();
	}
	...

### Adding a new non-static function

This example shows how to work in lua with non-global objects.

	// Some class for any work
	class Worker
	{
	public:
	bool Start(const std::string& a_StartMessage)
	{
		std::cout << "Worker: Start work (" << a_StartMessage << ")\n";
		return true;
	}
	void Stop()
	{
		std::cout << "Worker: Stop work\n";
	}
	};
	
	// LuaScript class contains Worker
	class WorkerLuaScript: public LuaScript
	{
	public:
	WorkerLuaScript(Worker* a_Worker)
	{
		m_Worker = a_Worker;
	}
	
	Worker* GetWorker() const
	{
		return m_Worker;
	}
	
	private:
	Worker* m_Worker;
	};
	
	class StartWorkerFunction 
	{
	public:
	StartWorkerFunction(LuaScript* a_Parent)
	{
		m_LuaScript = static_cast<WorkerLuaScript*>(a_Parent);
	}
	
	static const LuaScript::LuaArgArray* GetInputArgs()
	{
		LuaScript::LuaArgArray* args = new LuaScript::LuaArgArray();
		args->Add(new LuaScript::String_LuaArg());
		return args;
	}
	
	static const LuaScript::LuaArgArray* GetOutputArgs()
	{
		LuaScript::LuaArgArray* args = new LuaScript::LuaArgArray();
		args->Add(new LuaScript::Bool_LuaArg());
		return args;
	}
	
	static const std::string NameSpace()
	{
		return "worker";
	}
	static const std::string Name()
	{
		return "start";
	}
	
	void Calc(const LuaScript::LuaArgArray& in, LuaScript::LuaArgArray& out)
	{
		bool ret = false;
		std::string message = 
		dynamic_cast<LuaScript::String_LuaArg&>(*in[0]).GetValue();
		if( m_LuaScript )
		ret = m_LuaScript->GetWorker()->Start(message);
		dynamic_cast<LuaScript::Bool_LuaArg&>(*out[0]).SetValue(ret);
	}
	
	private:
	WorkerLuaScript* m_LuaScript;
	};
	
	class StopWorkerFunction 
	{
	public:
	StopWorkerFunction(LuaScript* a_Parent)
	{
		m_LuaScript = static_cast<WorkerLuaScript*>(a_Parent);
	}
	
	static const LuaScript::LuaArgArray* GetInputArgs()
	{
		return new LuaScript::LuaArgArray();
	}
	
	static const LuaScript::LuaArgArray* GetOutputArgs()
	{
		return new LuaScript::LuaArgArray();
	}
	
	static const std::string NameSpace()
	{
		return "worker";
	}
	static const std::string Name()
	{
		return "stop";
	}
	
	void Calc(const LuaScript::LuaArgArray& in, LuaScript::LuaArgArray& out)
	{
		if( m_LuaScript )
		m_LuaScript->GetWorker()->Stop();
	}
	
	private:  
	WorkerLuaScript* m_LuaScript;
	};
	...
	try
	{
		// Create Worker in stack (not in global)
		Worker worker;
		// Create contain Worker LuaScript class
		WorkerLuaScript script(&worker);
		// Register start/stop function
		script.RegisterFunction<StartWorkerFunction>();
		script.RegisterFunction<StopWorkerFunction>();
		// Execute some script. In script call worker.start() and worker.stop()
		script.Execute("started = worker.start('run in lua'); worker.stop();");
		bool started = script.GetVariable<LuaScript::Bool_LuaArg>("started").GetValue();
		std::cout << "result: worker " << (started ? "started" : "not started");
	}
	catch (LuaScript::LuaException& e) 
	{
		std::cerr << "error: " << e.Error() << ", line " << e.Line();
	}
	...


