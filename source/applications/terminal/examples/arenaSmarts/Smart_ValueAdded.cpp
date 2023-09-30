/*
 * This C++ program was automatically generated by Genesys 220517 (gowndictator)
 * For your own safety, please review this file before compiling and running it.
 */

#include "Smart_ValueAdded.h"

#include "../../../../kernel/simulator/Simulator.h"
#include "../../../../plugins/components/Create.h"
#include "../../../../plugins/components/Dispose.h"
#include "../../../../plugins/components/DummyComponent.h"
#include "../../../../plugins/components/Assign.h"
#include "../../../../plugins/components/Process.h"
#include "../../../../plugins/components/QueueableItem.h"
#include "../../../../plugins/components/Seize.h"
#include "../../../../plugins/components/Delay.h"
#include "../../../../plugins/components/Release.h"
#include "../../../../plugins/data/Queue.h"
#include "../../../../plugins/data/Resource.h"
#include "../../../../plugins/data/Variable.h"
#include "../../../TraitsApp.h"

Smart_ValueAdded::Smart_ValueAdded() {
}

int Smart_ValueAdded::main(int argc, char** argv) {
	Simulator* genesys = new Simulator();
	genesys->getTracer()->setTraceLevel(TraitsApp<GenesysApplication_if>::traceLevel);
	setDefaultTraceHandlers(genesys->getTracer());
	PluginManager* plugins = genesys->getPlugins();
	plugins->autoInsertPlugins("autoloadplugins.txt");
	Model* model = genesys->getModels()->newModel();
	// create model

	// Initialize resources
	Resource* Resource_1 = plugins->newInstance<Resource>(model, "Processor");
	Resource_1->setCapacity(1);
	Resource_1->setCostBusyTimeUnit(8.0);
	Resource_1->setCostIdleTimeUnit(8.0);
	Resource_1->setCostPerUse(0.0);

	Resource* Resource_2 = plugins->newInstance<Resource>(model, "Notary");
	Resource_2->setCapacity(1);
	Resource_2->setCostBusyTimeUnit(0.0);
	Resource_2->setCostIdleTimeUnit(0.0);
	Resource_2->setCostPerUse(10.0);
	
	// initialize model parts

	// Create 2 ContractArrives
	Create* Create_2 = plugins->newInstance<Create>(model, "ContractArrives");
	Create_2->setEntityTypeName("Contract");
	Create_2->setTimeBetweenCreationsExpression("expo(1)", Util::TimeUnit::hour);
	Create_2->setEntitiesPerCreation(1);
	Create_2->setFirstCreation(0.0);

	// Process 3 AddContractAddendum
	Process* Process_3 =  plugins->newInstance<Process>(model, "AddContractAddendum");
	Process_3->getSeizeRequests()->insert(new SeizableItem(Resource_1));
	Process_3->setQueueableItem(new QueueableItem(model, "AddContractAddendumQueue")); // ??
	Process_3->setDelayExpression("tria(0.5,1,1.5)");
	Process_3->setDelayTimeUnit(Util::TimeUnit::hour);
	Process_3->setAllocationType(Util::AllocationType::ValueAdded);

	// process 4 SignandNotorizeContract
	Process* Process_4 =  plugins->newInstance<Process>(model, "SignandNotorizeContract");
	Process_4->getSeizeRequests()->insert(new SeizableItem(Resource_2));
	Process_4->setQueueableItem(new QueueableItem(model, "SignandNotorizeContractQueue")); // ??
	Process_4->setDelayExpression("tria(0.5,1,1.5)");
	Process_4->setDelayTimeUnit(Util::TimeUnit::hour);
	Process_4->setAllocationType(Util::AllocationType::ValueAdded);

	// dispose 1 SendContract
	Dispose* Dispose_1 = plugins->newInstance<Dispose>(model, "SendContract");

	// connect model components
	Create_2->getConnections()->insert(Process_3);
	Process_3->getConnections()->insert(Process_4);
	Process_4->getConnections()->insert(Dispose_1);
	
	// set simulation parameters
	ModelSimulation* sim = model->getSimulation();
	sim->setNumberOfReplications(300);
	model->getSimulation()->setReplicationLength(1000, Util::TimeUnit::hour);
	sim->setWarmUpPeriod(50);
	sim->setWarmUpPeriodTimeUnit(Util::TimeUnit::hour);
	model->getSimulation()->setTerminatingCondition("count(SendContract.CountNumberIn)>1000");
	model->save("./models/Smart_ValueAdded.gen");
	model->getSimulation()->start();
	// run the simulation
	model->check();
	model->getDataManager()->show();
	sim->start();
	for (int i = 0; i < 1e9; i++); // give UI some time to finish std::cout
	// free memory
	delete genesys;
	
	return 0;
}
