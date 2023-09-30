/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   GenesysShell.cpp
 * Author: rafael.luiz.cancian
 * 
 * Created on 23 de Maio de 2019, 13:02
 */

#include "GenesysShell.h"
#include "../../../kernel/simulator/Simulator.h"
#include <regex>
#include <fstream>
#include <assert.h>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <stdlib.h>


using namespace std;
//#include "ProbDistribDefaultImpl1.h"

GenesysShell::GenesysShell() {
	setDefaultTraceHandlers(simulator->getTracer());
}

void GenesysShell::Trace(string message) {
	cout<<message<<endl;
}

/*int i;
const char progress[] = "|/-\\";

for (i = 0; i < 100; i += 10) {
  printf("Processing: %3d%%\r",i); /* \r returns the caret to the line start //
  fflush(stdout);
  //sleep(1);
}
printf("\n"); /* goes to the next line //
fflush(stdout);

printf("Processing: ");
for (i = 0; i < 100; i += 10) {
  printf("%c\b", progress[(i/10)%sizeof(progress)]); /* \b goes one back //
  fflush(stdout);
  //sleep(1);
}
printf("\n"); /* goes to the next line //
fflush(stdout);
return 0;*/

void GenesysShell::run(List<string>* commandlineArgs) {
	simulator->getTracer()->setTraceLevel(TraceManager::Level::L1_errorFatal);
	simulator->getPlugins()->autoInsertPlugins("autoloadplugins.txt");
	simulator->getTracer()->setTraceLevel(TraceManager::Level::L7_internal);
	_history->resize(100);
	Trace("Genesys Shell is running. Type your command. For help, type the command \"help\".");
	string inputText, historyText; //, shortPrefix, longPrefix, separator; //,longPrefix, separator;
	char c, c1, c2, c3;
	unsigned short historyPosition = 0;
	while (!_exitRequested) {
		if (!commandlineArgs->empty()) {
			inputText = commandlineArgs->front();
			commandlineArgs->pop_front();
			_history->push_back(inputText);
			tryExecuteCommand(inputText);
		} else {
			cout<<_prompt<<" ";
			//getline(cin, inputText);
			std::cin>>std::noskipws; //don't skip whitespaces
			inputText = "";
			//std::basic_ios::sync_with_stdio(false);
			while (c = getchar()) {
				if (c=='\n')
					break;
				//cout << int(c)<<','<<flush;//<<int(c1)<<','<<int(c2)<<endl;
				if ((c==65||c==66)&&c1==91&&c2==27) { // Up or down pressed. Show history
					printf("\b\b\b%c%c%c\b\b\b", ' ', ' ', ' ');
					fflush(stdout);
					if (_history->size()>0) {
						historyText = _history->at(historyPosition);
						if (c==65) {
							historyPosition++;
						} else {
							historyPosition--;
						}
						if (historyPosition>_history->size())
							historyPosition = 0;
						inputText = historyText;
						cout<<inputText;
					}
				} else { // normal key. Add to the text being typed
					inputText.push_back(c);
				}
				//c3 = c2;
				c2 = c1;
				c1 = c;
			}
			if (inputText!="") {
				_history->push_back(inputText);
				tryExecuteCommand(inputText);
			}
		}
	}
}

std::vector<std::string> GenesysShell::split(std::string text, std::string separatorRegex) {
	string expression = "(["+separatorRegex+"]+)";
	regex regex{expression};
	sregex_token_iterator it{text.begin(), text.end(), regex, -1};
	vector<string> fields{it,{}};
	return fields;
}

void GenesysShell::tryExecuteCommand(string inputText) {
	//regex regex{R"([\s]+)"}; // split on space R"([\s]+)"
	//sregex_token_iterator it{inputText.begin(), inputText.end(), regex, -1};
	//vector<string> fields{it,{}};
	vector<string> fields = split(inputText, "\\s");
	string typedCommandStr = fields[0];
	bool found = false;
	_typedWords->clear();
	for (string w : fields) {
		_typedWords->insert(_typedWords->end(), w);
	}
	transform(typedCommandStr.begin(), typedCommandStr.end(), typedCommandStr.begin(), ::tolower);
	if (typedCommandStr.substr(0, 1)!="#") {
		for (ShellCommand *cmd : *_commands->list()) {
			if (cmd->longname==typedCommandStr) {
				found = true;
				cmd->executer();
			}
		}
		if (!found) {
			Trace("Command \""+typedCommandStr+"\" not found. Type \"h\" or \"help\" for help.");
		}
	}

}

int GenesysShell::main(int argc, char** argv) {
	List<string>* commandlineArgs = new List<string>();
	for (unsigned short i = 1; i<argc; i++) {
		string arg = argv[i];
		commandlineArgs->insert(arg);
	}
	defineCommands();
	this->run(commandlineArgs);
	delete commandlineArgs;
	return 0;
}

void GenesysShell::defineCommands() {
	_commands->insert(new ShellCommand("", "help", "", "Show the list of commands", DefineExecuterMember<GenesysShell>(this, &GenesysShell::cmdHelp)));
	_commands->insert(new ShellCommand("", "quit", "", "Exit the simulator", DefineExecuterMember<GenesysShell>(this, &GenesysShell::cmdQuit)));
	//_commands->insert(new ShellCommand("", "dir", "<path>", "List the files in the <path>", DefineExecuterMember<GenesysShell>(this, &GenesysShell::cmdListFiles)));
	_commands->insert(new ShellCommand("", "bash", "<bash command>", "Execute a bash command", DefineExecuterMember<GenesysShell>(this, &GenesysShell::cmdBash)));
	_commands->insert(new ShellCommand("", "script", "[-r|--run|-s|--show]=<script filename>", "Execute or show commands in a script file", DefineExecuterMember<GenesysShell>(this, &GenesysShell::cmdScript)));
	_commands->insert(new ShellCommand("", "trace", "<level 1-9>", "Set the trace level", DefineExecuterMember<GenesysShell>(this, &GenesysShell::cmdTraceLevel)));
	_commands->insert(new ShellCommand("", "plugin", "[-l|--list] [-i|--info]=<plugin typename>", "List or get information about installed plugins", DefineExecuterMember<GenesysShell>(this, &GenesysShell::cmdPlugin)));
	//_commands->insert(new ShellCommand("", "plugininfo", "<plugin name>", "Show infos about a plugin", DefineExecuterMember<GenesysShell>(this, &GenesysShell::cmdPluginInfo)));
	//_commands->insert(new ShellCommand("", "pluginadd", "<plugin filename>", "", DefineExecuterMember<GenesysShell>(this, &GenesysShell::cmdPluginAdd)));
	//_commands->insert(new ShellCommand("", "pluginremove", "<classname>", "", DefineExecuterMember<GenesysShell>(this, &GenesysShell::cmdPluginRemove)));
	_commands->insert(new ShellCommand("", "simul", "[-s|--start|-p|--step]", "Control simulation", DefineExecuterMember<GenesysShell>(this, &GenesysShell::cmdSimulation)));
	//_commands->insert(new ShellCommand("", "step", "", "Step simulation", DefineExecuterMember<GenesysShell>(this, &GenesysShell::cmdSimulationStep)));
	//_commands->insert(new ShellCommand("", "stop", "", "Stop simulation", DefineExecuterMember<GenesysShell>(this, &GenesysShell::cmdSimulationStop)));
	_commands->insert(new ShellCommand("", "replication", "[-num=<number>] [-len=<replication>] [-tu=<time unit>] [-s|--show]", "Configure simulation", DefineExecuterMember<GenesysShell>(this, &GenesysShell::cmdReplication)));
	//_commands->insert(new ShellCommand("", "showsetup", "", "Show simulation info", DefineExecuterMember<GenesysShell>(this, &GenesysShell::cmdSimulationInfo)));
	//_commands->insert(new ShellCommand("", "showreport", "", "Show simulation report", DefineExecuterMember<GenesysShell>(this, &GenesysShell::cmdShowReport)));
	//_commands->insert(new ShellCommand("", "show", "", "Show the model structure", DefineExecuterMember<GenesysShell>(this, &GenesysShell::cmdModelShow)));
	_commands->insert(new ShellCommand("", "model", "[-n|--new|-r|--remove|-c|--check|-s|--show]", "Create, check, show or close a model", DefineExecuterMember<GenesysShell>(this, &GenesysShell::cmdModel)));
	//_commands->insert(new ShellCommand("", "close", "", "Close the odel", DefineExecuterMember<GenesysShell>(this, &GenesysShell::cmdModelClose)));
	//_commands->insert(new ShellCommand("", "check", "", "Check the model", DefineExecuterMember<GenesysShell>(this, &GenesysShell::cmdModelCheck)));
	_commands->insert(new ShellCommand("", "load", "<filename>", "Load a model from a file", DefineExecuterMember<GenesysShell>(this, &GenesysShell::cmdModelLoad)));
	_commands->insert(new ShellCommand("", "save", "<filename>", "Save a model to a file", DefineExecuterMember<GenesysShell>(this, &GenesysShell::cmdModelSave)));
	//_commands->insert(new ShellCommand("", "setInfos", "", "Set information of the model", DefineExecuterMember<GenesysShell>(this, &GenesysShell::cmdModelSetInfos)));
}

void GenesysShell::cmdHelp() {
	cout<<"List of commands:"<<endl;
	string parameters;
	for (ShellCommand *command : *_commands->list()) {
		parameters = command->parameters;
		if (parameters=="")
			parameters = "\t\t\t";
		cout<<command->longname<<"\t"<<parameters<<"\t\t; "<<command->descrition<<endl;
	}
}

void GenesysShell::cmdQuit() {
	cout<<"Quiting. Bye."<<endl;
	_exitRequested = true;
}

void GenesysShell::cmdBash() {
	if (_typedWords->size()<2) {
		cout<<"Wrong number of parameters"<<endl;
		return;
	}
	std::string parameters = "";
	for (unsigned short i = 1; i<_typedWords->size(); i++) {
		parameters += _typedWords->at(i)+" ";
	}
	const std::string command = parameters;
	system(command.c_str());
}

void GenesysShell::cmdScript() {
	if (_typedWords->size()!=2) {
		cout<<"Wrong number of parameters"<<endl;
		return;
	}
	string parameter = _typedWords->at(1);
	string key = "";
	string value = "";
	Util::SepKeyVal(parameter, key, value);
	bool run = key=="-r"||key=="-run";
	bool show = key=="-s"||key=="--show";
	if (!run && !show) {
		cout<<"Syntax error on "<<parameter<<endl;
		return;
	}
	if (show)
		cout<<"Showing script "<<value<<endl;
	if (run)
		cout<<"Running script "<<value<<endl;
	ifstream file(value);
	if (file.is_open()) {
		string line;
		while (getline(file, line)) {
			if (show) {
				cout<<line<<endl;
			}
			if (run) {
				if (line!="") {
					if (line[0]!='#') { // not a comment
						cout<<"$script> "<<line<<endl;
						tryExecuteCommand(line);
					}
				}
			}
		}
		file.close();
	} else {
		cout<<"Error: Could not load the script"<<endl;
	}
}

void GenesysShell::cmdTraceLevel() {
	if (_typedWords->size()!=2) {
		cout<<"Wrong number of parameters"<<endl;
		return;
	}
	int levelInt = stoi(_typedWords->at(1));
	if (levelInt>=1) {
		cout<<"Setting tracelevel to "<<levelInt<<endl;
		TraceManager::Level level = static_cast<TraceManager::Level> (levelInt);
		simulator->getTracer()->setTraceLevel(level);
	} else {
		cout<<"Error: "<<_typedWords->at(1)<<" is not a valid level"<<endl;
	}
}

void GenesysShell::cmdPlugin() {
	if (_typedWords->size()<2) {
		cout<<"Wrong number of parameters"<<endl;
		return;
	}
	string parameter;
	string key, value;
	for (unsigned short i = 1; i<_typedWords->size(); i++) {
		parameter = _typedWords->at(i);
		key = "";
		value = "";
		Util::SepKeyVal(parameter, key, value);
		if (key=="-l"||key=="--list") {
			PluginInformation* p;
			cout<<"List of installed plugins:"<<endl;
			for (unsigned short i = 0; i<simulator->getPlugins()->size(); i++) {
				p = simulator->getPlugins()->getAtRank(i)->getPluginInfo();
				if (p->isSource())
					cout<<"Source ";
				if (p->isSink())
					cout<<"Sink ";
				if (p->isSendTransfer())
					cout<<"SendTransfer ";
				if (p->isSendTransfer())
					cout<<"ReceiveTransfer ";
				if (p->isGenerateReport())
					cout<<"GenerateReport ";
				if (p->isComponent())
					cout<<"Component ";
				else
					cout<<"DataElement ";
				cout<<p->getPluginTypename()<<" ("<<p->getCategory()<<")";
				cout<<endl;
			}
		} else if (key=="-i"||key=="--info") {
			Plugin* p;
			for (unsigned short i = 0; i<simulator->getPlugins()->size(); i++) {
				p = simulator->getPlugins()->getAtRank(i);
				if (p->getPluginInfo()->getPluginTypename()==value) {
					cout<<"Information for plugin "<<value<<":"<<endl;
					cout<<p->show()<<endl;
					return;
				}
			}
			cout<<"Error: Could not find plugin of type "<<value<<endl;
		} else if (key=="-t"||key=="--template") {
			Plugin* p;
			string pf = "";
			for (unsigned short i = 0; i<simulator->getPlugins()->size(); i++) {
				p = simulator->getPlugins()->getAtRank(i);
				if (p->getPluginInfo()->getPluginTypename()==value) {
					//cout<<"Template for plugin "<<value<<":"<<endl;
					for (std::pair<std::string, std::string> field : *p->getPluginInfo()->getFields()) {
						pf += field.first+", ";
					}
					pf = pf.substr(0, pf.length()-1);
					cout<<"Parameters: "<<pf<<endl;
					cout<<"Template: "<<p->getPluginInfo()->getLanguageTemplate()<<endl;
					return;
				}
			}
			cout<<"Error: Could not find plugin of type "<<value<<endl;
		} else {
			cout<<"Syntax error on "<<parameter<<endl;
		}
	}
}

void GenesysShell::cmdSimulation() {
	if (model==nullptr) {
		cout<<"Error: There is no loaded model to simulate."<<endl;
		return;
	}
	if (_typedWords->size()!=2) {
		cout<<"Wrong number of parameters"<<endl;
		return;
	}
	string parameter = _typedWords->at(1);
	if (parameter=="-s"||parameter=="--start")
		model->getSimulation()->start();
	else if (parameter=="-p"||parameter=="--step")
		model->getSimulation()->step();
	else
		cout<<"Syntax error on "+parameter<<endl;
}

void GenesysShell::cmdReplication() {
	if (model==nullptr) {
		cout<<"Error: There is no loaded model to setup its simulation."<<endl;
		return;
	}
	if (_typedWords->size()<2) {
		cout<<"Wrong number of parameters"<<endl;
		return;
	}
	string parameter;
	string key, value;
	for (unsigned short i = 1; i<_typedWords->size(); i++) {
		parameter = _typedWords->at(i);
		//vector<string> keyvalue = split(parameter, "=");
		key = "";
		value = "";
		//cout<<parameter<<endl;
		Util::SepKeyVal(parameter, key, value);
		if (key=="-num") {
			cout<<"Setting number of replications to "<<value<<endl;
			model->getSimulation()->setNumberOfReplications(stoi(value));
		} else if (key=="-len") {
			cout<<"Setting replication length to "<<value<<endl;
			model->getSimulation()->setReplicationLength(stod(value));
		} else if (key=="-tu") {
			cout<<"Setting replication time unit to "<<value<<endl;
			model->getSimulation()->setReplicationLengthTimeUnit(static_cast<Util::TimeUnit> (stoi(value)));
		} else if (key=="-s"||key=="--show") {
			cout<<model->getSimulation()->show()<<endl;
			//model->getSimulation()->setReplicationLengthTimeUnit(static_cast<Util::TimeUnit> (stoi(value)));
		} else {
			cout<<"Syntax error on "<<parameter<<endl;
		}
	}
}

void GenesysShell::cmdModel() {
	if (_typedWords->size()!=2) {
		cout<<"Wrong number of parameters"<<endl;
		return;
	}

	string parameter;
	string key, value;
	for (unsigned short i = 1; i<_typedWords->size(); i++) {
		parameter = _typedWords->at(i);
		if (parameter=="-n"||parameter=="--new") {
			if (model!=nullptr) {
				//if (_typedWords->size()!=2) {
				//	cout<<"There is an open model. Close it before create a new one, of force close with -f or --force parameters"<<endl;
				//	return;
				//}
				//std::string parameter = _typedWords->at(1);
				//if (parameter!="-f"&&parameter!="--force") {
				//	cout<<"Wrong number of parameters"<<endl;
				//	return;
				//} else {
				simulator->getModels()->remove(model);
				model = nullptr;
				//}
			}
			cout<<"Creating a new model"<<endl;
			model = simulator->getModels()->newModel();
		} else if (parameter=="-r"||parameter=="--remove") {
			if (model==nullptr) {
				cout<<"Error: There is no loaded model to close."<<endl;
				return;
			}
			cout<<"Closing the model"<<endl;
			simulator->getModels()->remove(model);
			model = nullptr;
		} else if (parameter=="-c"||parameter=="--check") {
			if (model==nullptr) {
				cout<<"Error: There is no loaded model to close."<<endl;
				return;
			}
			model->check();
		} else if (parameter=="-s"||parameter=="--show") {
			if (model==nullptr) {
				cout<<"Error: There is no loaded model to close."<<endl;
				return;
			}
			cout<<model->showLanguage()<<endl;
			//model->show();
		} else {
			cout<<"Syntax error on "+parameter<<endl;
		}
	}

}

void GenesysShell::cmdModelLoad() {
	if (_typedWords->size()!=2) {
		cout<<"Wrong number of parameters"<<endl;
		return;
	}
	std::string parameter = _typedWords->at(1);
	cout<<"Loading model "<<parameter<<endl;
	model = simulator->getModels()->loadModel(parameter);
	if (model==nullptr) {
		cout<<"Error: Could not load the model"<<endl;
	} else {
		cout<<"Model loaded"<<endl;
	}
}

void GenesysShell::cmdModelSave() {
	if (model==nullptr) {
		cout<<"Error: There is no loaded model to save."<<endl;
		return;
	}
	if (_typedWords->size()!=2) {
		cout<<"Wrong number of parameters"<<endl;
		return;
	}
	std::string parameter = _typedWords->at(1);
	cout<<"Saving model "<<parameter<<endl;
	if (model->save(parameter)) {
		cout<<"Model saved"<<endl;
	} else {
		cout<<"Error: Could not save the model"<<endl;
	}
}

/*
 void GenesysShell::cmdShowReport() {
	if (model==nullptr) {
		cout<<"Error: There is no loaded model to show report."<<endl;
		return;
	}
}

 void GenesysShell::cmdPluginInfo() {
	if (_typedWords->size()!=2) {
		cout<<"Wrong number of parameters"<<endl;
		return;
	}
	std::string parameter = _typedWords->at(1);
	Plugin* p;
	for (unsigned short i = 0; i<simulator->getPlugins()->size(); i++) {
		p = simulator->getPlugins()->getAtRank(i);
		if (p->getPluginInfo()->getPluginTypename()==parameter) {
			cout<<"Information for plugin "<<parameter<<":"<<endl;
			cout<<p->show()<<endl;
			return;
		}
	}
	cout<<"Error: Could not find plugin of type "<<parameter<<endl;
}


void GenesysShell::cmdSimulationStep() {
	if (model==nullptr) {
		cout<<"Error: There is no loaded model to simulate."<<endl;
		return;
	}
	model->getSimulation()->step();
}

void GenesysShell::cmdSimulationStop() {
	if (model==nullptr) {
		cout<<"Error: There is no loaded model to simulate."<<endl;
		return;
	}
	cout<<"Stoping simulation"<<endl;
	model->getSimulation()->stop();
}

void GenesysShell::cmdSimulationInfo() {
	if (model==nullptr) {
		cout<<"Error: There is no loaded model to simulate."<<endl;
		return;
	}
	cout<<model->getSimulation()->show()<<endl;
}

void GenesysShell::cmdModelClose() {
	if (model==nullptr) {
		cout<<"Error: There is no loaded model to close."<<endl;
		return;
	}
	cout<<"Closing the model"<<endl;
	simulator->getModels()->remove(model);
	model = nullptr;
}

void GenesysShell::cmdModelCheck() {
	if (model==nullptr) {
		cout<<"Error: There is no loaded model to check."<<endl;
		return;
	}
	model->check();
}



void GenesysShell::cmdModelShow() {
	if (model==nullptr) {
		cout<<"Error: There is no loaded model to show."<<endl;
		return;
	}
	model->show();
}

void GenesysShell::cmdModelSetInfos() {
	if (model==nullptr) {
		cout<<"Error: There is no loaded model to set information."<<endl;
		return;
	}
}

void GenesysShell::cmdListFiles() {
	if (_typedWords->size()!=2) {
		cout<<"Wrong number of parameters"<<endl;
		return;
	}
	std::string parameter = _typedWords->at(1);
	const std::string command = "ls -l "+parameter;
	system(command.c_str());
}
 */