#include <string>
#include "Genesys++-driver.h"
//#include "../Traits.h"

//using namespace GenesysKernel;

genesyspp_driver::genesyspp_driver() {
}

genesyspp_driver::genesyspp_driver(/*GenesysKernel::*/Model* model, Sampler_if* sampler, bool throws) {
	_model = model;
	_sampler = sampler;
	throwsException = throws;
}

/*
void genesyspp_driver::scan_begin_file() {
	// IMPLEMENTED IN LEXPARSER.LL
}

void genesyspp_driver::scan_end_file() {
	// IMPLEMENTED IN LEXPARSER.LL
}
 

void genesyspp_driver::scan_begin_str() {
	// // IMPLEMENTED IN LEXPARSER.LL
}

void genesyspp_driver::scan_end_str() {
	// IMPLEMENTED IN LEXPARSER.LL
}
 */
int genesyspp_driver::parse_file(const std::string &f) {
	result = 0;
	file = f;
	setErrorMessage("");
	scan_begin_file();
	yy::genesyspp_parser parser(*this);
	int res = parser.parse();
	scan_end_file();
	return res;
}

int genesyspp_driver::parse_str(const std::string &str) {
	result = 0;
	str_to_parse = str;
	setErrorMessage("");
	scan_begin_str();
	yy::genesyspp_parser parser(*this);
	int res = parser.parse();
	scan_end_str();
	return res;
}

void genesyspp_driver::setResult(double value) {
	result = value;
}

double genesyspp_driver::getResult() {
	return result;
}

bool genesyspp_driver::getThrowsException() {
	return throwsException;
}

void genesyspp_driver::setThrowsException(bool throws) {
	throwsException = throws;
}

void genesyspp_driver::setErrorMessage(std::string message) {
	errorMessage = message;
}

std::string genesyspp_driver::getErrorMessage() {
	return errorMessage;
}

/*GenesysKernel::*/Model* genesyspp_driver::getModel() {
	return _model;
}

std::string genesyspp_driver::getFile() {
	return file;
}

void genesyspp_driver::setFile(std::string f) {
	file = f;
}

std::string genesyspp_driver::getStrToParse() {
	return str_to_parse;
}

void genesyspp_driver::setStrToParse(std::string str) {
	str_to_parse = str;
}

void genesyspp_driver::setSampler(Sampler_if* _sampler) {
	this->_sampler = _sampler;
}

Sampler_if* genesyspp_driver::getSampler() const {
	return _sampler;
}

void genesyspp_driver::setRegisterReferedDataElements(bool value) {
	_isRegisterReferedDataElements = value;
}

bool genesyspp_driver::getRegisterReferedDataElements() {
	return _isRegisterReferedDataElements;
}

std::map<std::string, std::list<std::string>*>* genesyspp_driver::getReferedDataElements() {
	return _referedDataElements;
}
void genesyspp_driver::clearReferedDataElements() {
	_referedDataElements->clear();
}

void genesyspp_driver::addRefered(std::pair<std::string,std::string> referedElement) { // pair<DataElementTypename, name>
	if (_isRegisterReferedDataElements) {
		std::list<std::string> *listNamesRefered;
		if (_referedDataElements->find(referedElement.first) == _referedDataElements->end()) { // dataElemTyper never refered before
			_referedDataElements->insert({referedElement.first, new std::list<std::string>()});
		}
		auto it = _referedDataElements->find(referedElement.first);
		listNamesRefered = (*it).second;
		if (listNamesRefered==nullptr) {
			listNamesRefered = new std::list<std::string>();
			(*it).second = listNamesRefered;
		}
		for(std::string name: *listNamesRefered) {
			if (name==referedElement.second)
				return; // already exists. return and do not insert again
		}
		// not found. insert
		listNamesRefered->insert(listNamesRefered->end(), referedElement.second);
	}
	//_referedDataElements->insert(_referedDataElements->end(), referedElement);
}

void genesyspp_driver::error(const yy::location& l, const std::string& m) {
	std::string erro(m);
	erro.append("\n");
	setErrorMessage(m);
	setResult(-1);
	_model->getTracer()->trace(/*GenesysKernel::*/TraceManager::Level::L1_errorFatal, erro);
}

void
genesyspp_driver::error(const std::string& m) {
	setErrorMessage(m);
	setResult(-1);
	_model->getTracer()->trace(/*GenesysKernel::*/TraceManager::Level::L1_errorFatal, m);
}
