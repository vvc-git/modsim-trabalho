/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   ProcessAnalyserDefaultImpl1.cpp
 * Author: rlcancian
 *
 * Created on 20 de Maio de 2019, 20:45
 */

#include "ExperimentManagerDefaultImpl1.h"

ExperimentManagerDefaultImpl1::ExperimentManagerDefaultImpl1() {
}

List<SimulationScenario*>* ExperimentManagerDefaultImpl1::getScenarios() const {
	// @TODO: implement
	return nullptr;
}

//List<PropertyBase*>* ExperimentManagerDefaultImpl1::getControls() const {
//	return _controls;
//}

//List<PropertyBase*>* ExperimentManagerDefaultImpl1::getResponses() const {
// @TODO: implement
//	return nullptr;
//}

//List<PropertyBase*>* ExperimentManagerDefaultImpl1::extractControlsFromModel(std::string modelFilename) const {
// @TODO: implement
//	modelFilename = ""; //just to use it
//	return nullptr;
//}

//List<PropertyBase*>* ExperimentManagerDefaultImpl1::extractResponsesFromModel(std::string modelFilename) const {
// @TODO: implement
//	modelFilename = ""; // juts to use it
//	return nullptr;
//}

void ExperimentManagerDefaultImpl1::startSimulationOfScenario(SimulationScenario* scenario) {
	// @TODO: implement
}

void ExperimentManagerDefaultImpl1::startExperiment() {
	// @TODO: implement
}

void ExperimentManagerDefaultImpl1::stopExperiment() {
	// @TODO: implement
}

void ExperimentManagerDefaultImpl1::addTraceSimulationHandler(traceSimulationProcessListener traceSimulationProcessListener) {
}
