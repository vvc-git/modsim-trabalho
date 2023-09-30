/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Element.h
 * Author: rafael.luiz.cancian
 *
 * Created on 21 de Junho de 2018, 15:56
 */

#ifndef MODELCOMPONENT_H
#define MODELCOMPONENT_H

#include <string>
#include <list>

#include "Plugin.h"
#include "../util/List.h"
#include "Entity.h"
#include "ModelDataDefinition.h"
#include "ConnectionManager.h"
//namespace GenesysKernel {

class Model;
class Event;

/*!
 * A component of the model is a block that represents a specific behavior to be simulated. The behavior is triggered when an entity arrives at the component, which corresponds to the occurrence of an event. A simulation model corresponds to a set of interconnected components to form the process by which the entity is submitted.
 * @param model The model this component belongs to
 */
class ModelComponent : public ModelDataDefinition {
public:
	ModelComponent(Model* model, std::string componentTypename, std::string name = "");
	virtual ~ModelComponent();
public:
	virtual std::string show();
public:
	ConnectionManager* getConnections() const; ///< Returns a list of components directly connected to the output. Usually the components have a single output, but they may have none (such as Dispose) or more than one (as Decide). In addition to the component, NextComponents specifies the inputPortNumber of the next component where the entity will be sent to. Ussually the components have a single input, but they may have none (such as Create) or more than one (as Match).
	bool hasBreakpointAt();
public: // static
	static void DispatchEvent(Event* event); ///< This method triggers the simulation of the behavior of the component. It is invoked when an event (corresponding to this component) is taken from the list of future events or when an entity arrives at this component by connection.
	//static void InitBetweenReplications(ModelComponent* component);
	static void CreateInternalData(ModelComponent* component);
	static bool Check(ModelComponent* component);
	static ModelComponent* LoadInstance(Model* model, PersistenceRecord *fields);
	static void SaveInstance(PersistenceRecord *fields, ModelComponent* component);
	void setDescription(std::string _description);
	std::string getDescription() const;
protected: // must be overriden by derived classes
	virtual void _saveInstance(PersistenceRecord *fields, bool saveDefaultValues);
	virtual bool _loadInstance(PersistenceRecord *fields);
	// new virtual methods
	virtual void _onDispatchEvent(Entity* entity, unsigned int inputPortNumber) = 0;
protected: // could be overriden by derived classes
	//virtual bool _check(std::string* errorMessage);
	/*! This method returns all changes in the parser that are needed by plugins of this ModelDatas. When connecting a new plugin, ParserChangesInformation are used to change parser source code, whch is after compiled and dinamically linked to to simulator kernel to reflect the changes */
	//virtual ParserChangesInformation* _getParserChangesInformation();
	//virtual void _initBetweenReplications();
	/*! This method is necessary only for those components that instantiate internal elements that must exist before simulation starts and even before model checking. That's the case of components that have internal StatisticsCollectors, since others components may refer to them as expressions (as in "TVAG(ThisCSTAT)") and therefore the modeldatum must exist before checking such expression */
	//virtual void _createInternalAndAttachedData(); /*< A ModelDataDefinition or ModelComponent that includes (internal) ou refers to (attach) other ModelDataDefinition must register them inside this method. */
	//virtual void _addProperty(PropertyBase* property);

protected:  // just an easy access to trace
	//void traceSimulation(void* thisobject, double time, Entity* entity, ModelComponent* component, std::string text, TraceManager::Level level = TraceManager::Level::L8_detailed);


protected:

	const struct DEFAULT_VALUES {
		const unsigned int nextSize = 1;
		const unsigned int nextinputPortNumber = 0;
		const std::string description = "";
	} DEFAULT;
	std::string _description = DEFAULT.description;
	ConnectionManager* _connections = new ConnectionManager();
};
//namespace\\}
#endif /* MODELCOMPONENT_H */

