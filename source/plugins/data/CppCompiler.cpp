/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/*
 * File:   DynamicLinkedCode.cpp
 * Author: rlcancian
 *
 * Created on 11 de janeiro de 2022, 22:24
 */

#include "CppCompiler.h"

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/param.h>
#include <grp.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <bits/stdc++.h>
#include <thread>
// dynamic load
#include <dlfcn.h>

#include <iostream>
#include <string>
// execv
#include <unistd.h>
//
#include <thread>


#include "../../kernel/simulator/Model.h"


#ifdef PLUGINCONNECT_DYNAMIC

extern "C" StaticGetPluginInformation getPluginInformation() {
	return &CppCompiler::getPluginInformation;
}
#endif

ModelDataDefinition* CppCompiler::NewInstance(Model* model, std::string name) {
	return new CppCompiler(model, name);
}

CppCompiler::CppCompiler(Model* model, std::string name) : ModelDataDefinition(model, Util::TypeOf<CppCompiler>(), name) {
}

// static

ModelDataDefinition* CppCompiler::LoadInstance(Model* model, PersistenceRecord *fields) {
	CppCompiler* newElement = new CppCompiler(model);
	try {
		newElement->_loadInstance(fields);
	} catch (const std::exception& e) {

	}
	return newElement;
}

PluginInformation* CppCompiler::GetPluginInformation() {
	PluginInformation* info = new PluginInformation(Util::TypeOf<CppCompiler>(), &CppCompiler::LoadInstance, &CppCompiler::NewInstance);
	info->setDescriptionHelp("//@TODO");
	//info->setObservation("");
	//info->setMinimumOutputs();
	//info->setDynamicLibFilenameDependencies();
	//info->setFields();
	// ...
	return info;
}

//

std::string CppCompiler::show() {
	return ModelDataDefinition::show();
}

void CppCompiler::setSourceFilename(std::string _code) {
	this->_sourceFilename = _code;
}

std::string CppCompiler::getSourceFilename() const {
	return _sourceFilename;
}
// must be overriden

bool CppCompiler::_loadInstance(PersistenceRecord *fields) {
	bool res = ModelDataDefinition::_loadInstance(fields);
	if (res) {
		// @TODO: not implemented yet
	}
	return res;
}

void CppCompiler::_saveInstance(PersistenceRecord *fields, bool saveDefaultValues) {
	ModelDataDefinition::_saveInstance(fields, saveDefaultValues);
	// @TODO: not implemented yet
}

// could be overriden

bool CppCompiler::_check(std::string* errorMessage) {
	//@ TODO check if compiler command exists
	return true;
}

void CppCompiler::_createInternalAndAttachedData() {
}

void CppCompiler::_initBetweenReplications() {

}


CppCompiler::CompilationResult CppCompiler::compileToExecutable() {
	CppCompiler::CompilationResult result;
	Util::FileDelete(this->_outputFilename);
	std::string command(_compilerCommand + " " + _flagsGeneral + " " + _flagsExecutable + " " + _objectFiles + " " + _sourceFilename + " -o " + _outputFilename);
	result = _invokeCompiler(command);
	_compiledToDynamicLibrary = false;
	return result;
}

CppCompiler::CompilationResult CppCompiler::compileToDynamicLibrary() {
	CppCompiler::CompilationResult result;
	std::string command(_compilerCommand + " " + _flagsGeneral + " " + _flagsDynamicLibrary + " " + _objectFiles + " " + _sourceFilename + " -o " + _outputFilename);
	result = _invokeCompiler(command);
	_compiledToDynamicLibrary = result.success;
	return result;
}

CppCompiler::CompilationResult CppCompiler::compileToStaticLibrary() {
	CppCompiler::CompilationResult result;
	Util::FileDelete(_outputFilename);
	std::string command(_compilerCommand + " " + _flagsGeneral + " " + _flagsStaticLibrary + " " + _objectFiles + " " + _sourceFilename + " -o " + _outputFilename);
	result = _invokeCompiler(command);
	_compiledToDynamicLibrary = false;
	return result;
}

bool CppCompiler::loadLibrary(std::string* errorMessage) {
	try {
		_dynamicLibraryHandle = dlopen(_outputFilename.c_str(), RTLD_LAZY);
	} catch (const std::exception& e) {
		if (dlerror() != NULL)
			*errorMessage += dlerror();
		*errorMessage += e.what();
		return false;
	}
	if (_dynamicLibraryHandle == nullptr) {
		*errorMessage += dlerror();
	}
	_libraryLoaded = _dynamicLibraryHandle != nullptr;
	return _libraryLoaded;
}

bool CppCompiler::unloadLibrary() {
	if (_libraryLoaded) {
		try {
			dlclose(_dynamicLibraryHandle);
			_dynamicLibraryHandle = nullptr;
			_libraryLoaded = false;
			return true;
			_libraryLoaded = false;
		} catch (const std::exception& e) {
			return false;
		}
	}
	return true;
}

void* CppCompiler::getDynamicLibraryHandler() const {
	return _dynamicLibraryHandle;
}

void CppCompiler::setObjectFiles(std::string _objectFiles) {
	this->_objectFiles = _objectFiles;
}

std::string CppCompiler::getObjectFiles() const {
	return _objectFiles;
}

void CppCompiler::setFlagsExecutable(std::string _flagsExecutable) {
	this->_flagsExecutable = _flagsExecutable;
}

std::string CppCompiler::getFlagsExecutable() const {
	return _flagsExecutable;
}

void CppCompiler::setLibraryLoaded(bool libraryLoaded) {
	this->_libraryLoaded = libraryLoaded;
}

void CppCompiler::setFlagsStaticLibrary(std::string _flagsStaticLibrary) {
	this->_flagsStaticLibrary = _flagsStaticLibrary;
}

std::string CppCompiler::getFlagsStaticLibrary() const {
	return _flagsStaticLibrary;
}

void CppCompiler::setFlagsDynamicLibrary(std::string _flagsDynamicLibrary) {
	this->_flagsDynamicLibrary = _flagsDynamicLibrary;
}

std::string CppCompiler::getFlagsDynamicLibrary() const {
	return _flagsDynamicLibrary;
}

void CppCompiler::setFlagsGeneral(std::string _flagsGeneral) {
	this->_flagsGeneral = _flagsGeneral;
}

std::string CppCompiler::getFlagsGeneral() const {
	return _flagsGeneral;
}

void CppCompiler::setCompilerCommand(std::string _compilerCommand) {
	this->_compilerCommand = _compilerCommand;
}

std::string CppCompiler::getCompilerCommand() const {
	return _compilerCommand;
}

void CppCompiler::setOutputDir(std::string _outputDir) {
	this->_outputDir = _outputDir;
}

std::string CppCompiler::getOutputDir() const {
	return _outputDir;
}

void CppCompiler::setTempDir(std::string _tempDir) {
	this->_tempDir = _tempDir;
}

std::string CppCompiler::getTempDir() const {
	return _tempDir;
}

void CppCompiler::setOutputFilename(std::string _outputFilename) {
	this->_outputFilename = _outputFilename;
}

std::string CppCompiler::getOutputFilename() const {
	return _outputFilename;
}

bool CppCompiler::IsLibraryLoaded() const {
	return _libraryLoaded;
}

std::string CppCompiler::_read(std::string filename) {
	std::string res = "";
	std::ifstream file(filename);
	if (file.is_open()) {
		std::string line;
		while (std::getline(file, line)) {
			res += line+"\n";
		}
		file.close();
	}
	return res;
}

CppCompiler::CompilationResult CppCompiler::_invokeCompiler(std::string command) {
	const std::string destPath = "";
	const std::string redirect = " >" + destPath + "stdout.log 2>" + destPath + "stdout.log";

	Util::IncIndent();

	Util::FileDelete(_outputFilename);
	Util::FileDelete(destPath + "stdout.log");
	Util::FileDelete(destPath + "stdout.log");

	const std::string execCommand = command + redirect;
	//trace(execCommand);
	system(execCommand.c_str());
	for (short i = 0; i < 32; i++)
		std::this_thread::yield(); // give the system some time
	const std::string resultStdout = _read(destPath+"stdout.log");
	const std::string resultStderr = _read(destPath+"stderr.log");
	//trace(resultStdout);
	//trace(resultStderr);

	CppCompiler::CompilationResult result;
	std::ifstream f(_outputFilename.c_str());
	result.success =  f.good();
	result.compilationStdOutput = resultStdout;
	result.compilationErrOutput = resultStderr;
	result.destinationPath = destPath;

	Util::FileDelete(destPath + "stdout.log");
	Util::FileDelete(destPath + "stderr.log");

	Util::DecIndent();

	return result;
}
