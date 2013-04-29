

#ifndef _INCLUDE_SMJS_PLUGIN_H_
#define _INCLUDE_SMJS_PLUGIN_H_

#include <string>
#include <map>
#include "smsdk_ext.h"

#include "SMJS.h"
#include "SMJS_Module.h"
#include "SMJS_BaseWrapped.h"

typedef SMJS_Module* (*SMJS_ModuleCreator)();

void SMJS_AddModule(SMJS_Module *module);

v8::Handle<v8::Value> JSN_Print(const v8::Arguments& args);
v8::Handle<v8::Value> JSN_Require(const v8::Arguments& args);

class SMJS_Plugin;
typedef void(*DestroyCallback)(SMJS_Plugin*);

/**
	Gets the plugin that's running in the current context
*/
inline SMJS_Plugin *GetPluginRunning(){
	return (SMJS_Plugin*) v8::Handle<External>::Cast(v8::Context::GetCurrent()->GetEmbedderData(1))->Value();
}

SMJS_Plugin *GetPlugin(PLUGIN_ID id);
int GetNumPlugins();

class SMJS_Plugin {
public: // Public variables
	PLUGIN_ID id;
	v8::Isolate *isolate;
	v8::Persistent<v8::Context> context;
	int apiVersion;
	bool isSandboxed;

protected: // Protected variables
	std::string path;
	std::string dir;

	std::vector<DestroyCallback> destroyCallbackFuncs;
	std::vector<IPluginDestroyedHandler*> destroyCallbackHandlers;
	std::vector<SMJS_Module*> loadedModules;

	std::map<std::string, std::vector<v8::Persistent<v8::Function>>> hooks;
	std::map<std::string, std::vector<v8::Persistent<v8::Function>>> eventHooks;
	std::map<std::string, std::vector<v8::Persistent<v8::Function>>> eventPostHooks;

public: // Public functions
	SMJS_Plugin();
	~SMJS_Plugin();
	// Virtuals for the public interface, do NOT change the order
	virtual PLUGIN_ID GetId(){return id;}
	virtual v8::Persistent<v8::Context> GetContext(){return context;}
	virtual v8::Isolate *GetIsolate(){return isolate;}
	virtual void RegisterDestroyCallback(DestroyCallback func);
	virtual void RegisterDestroyCallback(IPluginDestroyedHandler *ptr);
	//virtual int GetApiVersion(char const *id, int curVersion){return curVersion;};
	virtual std::vector<v8::Persistent<v8::Function>>* GetHooks(char const *type);
	virtual std::vector<v8::Persistent<v8::Function>>* GetEventHooks(char const *type);
	virtual std::vector<v8::Persistent<v8::Function>>* GetEventPostHooks(char const *type);
	// 

	bool RunString(const char* name, const char *source, bool asGlobal, v8::Handle<v8::Value> *result = NULL);
	bool LoadFile(const char* file, bool asGlobal, v8::Handle<v8::Value> *result = NULL);
	void LoadModules();

	void CheckApi();

	inline int GetApiVersion(){
		return apiVersion;
	}

	void SetDir(const char* newDir){
		dir.assign(newDir);
	}
	
	const char* GetDir(){
		return dir.c_str();
	}

	void SetPath(const char* newPath){
		path.assign(newPath);
	}
	
	const char* GetPath(){
		return path.c_str();
	}

	static SMJS_Plugin *GetPluginByDir(const char *dir);

private: // Private functions
	void ReportException(v8::TryCatch* try_catch);


	friend SMJS_Module;
};

SMJS_Plugin *GetPlugin(PLUGIN_ID id);

#endif