/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SimulationScenario.cpp
 * Author: rafael.luiz.cancian
 * 
 * Created on 10 de Outubro de 2018, 18:21
 */

#include "SimulationScenario.h"

SimulationScenario::SimulationScenario() = default;

bool SimulationScenario::startSimulation(Simulator *sim, std::string* errorMessage) {
	// @TODO: To implement!
	// *Simulator??????s
	*errorMessage = ""; // just clean the error message
	return false;
}

void SimulationScenario::setScenarioName(std::string _name) {
	this->_scenarioName = _name;
}

std::string SimulationScenario::getScenarioName() const {
	return _scenarioName;
}

void SimulationScenario::setModelFilename(std::string _modelFilename) {
	this->_modelFilename = _modelFilename;
}

std::string SimulationScenario::getModelFilename() const {
	return _modelFilename;
}

std::list<std::string>* SimulationScenario::getSelectedResponses() const {
	return _selectedResponses;
}

std::list<std::string>* SimulationScenario::getSelectedControls() const {
	return _selectedControls;
}

void SimulationScenario::setScenarioDescription(std::string _scenarioDescription) {
	this->_scenarioDescription = _scenarioDescription;
}

std::string SimulationScenario::getScenarioDescription() const {
	return _scenarioDescription;
}

std::list<std::pair<std::string, double> *> *SimulationScenario::getResponseValues() const {
	return _responseValues;
}

std::list<std::pair<std::string, double> *> *SimulationScenario::getControlValues() const {
	return _controlValues;
}

double SimulationScenario::getResponseValue(const std::string& responseName) {
	for (auto const& r : *_responseValues) {
		if (r->first == responseName) {
			return r->second;
		}
	}
	throw std::invalid_argument("Response does not exist");
}

double SimulationScenario::getControlValue(const std::string& controlName) {
	for (auto const& r : *_controlValues) {
		if (r->first == controlName) {
			return r->second;
		}
	}
	throw std::invalid_argument("Control does not exist");
}

void SimulationScenario::setControl(std::string name, double value) const {
	auto aux = new std::pair<std::string, double>(name, value);
	_controlValues->push_back(aux);
}

void SimulationScenario::setSelectedControls(std::list<std::string> *selectedControls) {
	auto* controls = new std::list<std::string>();
	std::copy(selectedControls->begin(), selectedControls->end(), std::back_inserter(*controls));
	_selectedControls = controls;
}