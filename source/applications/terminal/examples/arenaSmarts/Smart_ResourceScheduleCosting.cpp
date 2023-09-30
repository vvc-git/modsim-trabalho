/*
 * This C++ program was automatically generated by Genesys 220517 (gowndictator)
 * For your own safety, please review this file before compiling and running it.
 */

#include "Smart_ResourceScheduleCosting.h"

#include "../../../../kernel/simulator/Simulator.h"
#include "../../../../plugins/components/Create.h"
#include "../../../../plugins/components/Dispose.h"
#include "../../../../plugins/components/Process.h"
#include "../../../../plugins/components/QueueableItem.h"
#include "../../../../plugins/data/Resource.h"
#include "../../../../plugins/data/Schedule.h"
#include "../../../TraitsApp.h"

Smart_ResourceScheduleCosting::Smart_ResourceScheduleCosting() {
}

int Smart_ResourceScheduleCosting::main(int argc, char** argv) {
	Simulator* genesys = new Simulator();
	genesys->getTracer()->setTraceLevel(TraitsApp<GenesysApplication_if>::traceLevel);
	setDefaultTraceHandlers(genesys->getTracer());
	PluginManager* plugins = genesys->getPlugins();
	plugins->autoInsertPlugins("autoloadplugins.txt");
	Model* model = genesys->getModels()->newModel();
	// create model

	// Initialize resources
	Schedule* Schedule_1 = plugins->newInstance<Schedule>(model, "Schedule");
	Schedule_1->getSchedulableItems()->insert(new SchedulableItem ("1",135));
	Schedule_1->getSchedulableItems()->insert(new SchedulableItem ("0",15));
	Schedule_1->getSchedulableItems()->insert(new SchedulableItem ("1",90));
	Schedule_1->getSchedulableItems()->insert(new SchedulableItem ("0",60));
	Schedule_1->getSchedulableItems()->insert(new SchedulableItem ("1",90));
	Schedule_1->getSchedulableItems()->insert(new SchedulableItem ("0",15));
	Schedule_1->getSchedulableItems()->insert(new SchedulableItem ("1",45));
	Schedule_1->getSchedulableItems()->insert(new SchedulableItem ("1",9999999999999)); // 1, Infinito


	Resource* Resource_1 = plugins->newInstance<Resource>(model, "Biller");
	Resource_1->setCapacitySchedule(Schedule_1);
	Resource_1->setCostBusyTimeUnit(7.75);
	Resource_1->setCostIdleTimeUnit(7.75);
	Resource_1->setCostPerUse(0.02);

	Resource* Resource_2 = plugins->newInstance<Resource>(model, "Mailer");
	Resource_2->setCapacitySchedule(Schedule_1);
	Resource_2->setCostBusyTimeUnit(5.15);
	Resource_2->setCostIdleTimeUnit(5.15);
	Resource_2->setCostPerUse(0.02);
	
	// initialize model parts

	// Create 2
	Create* Create_2 = plugins->newInstance<Create>(model, "Arrival");
	Create_2->setEntityTypeName("Arrival");
	Create_2->setTimeBetweenCreationsExpression("expo(1.5)", Util::TimeUnit::minute);
	Create_2->setEntitiesPerCreation(1);
	Create_2->setFirstCreation(0.0);

	// Process 3
	Process* Process_3 =  plugins->newInstance<Process>(model, "Billing");
	Process_3->getSeizeRequests()->insert(new SeizableItem(Resource_1));
	Process_3->setQueueableItem(new QueueableItem(model, "BillingQueue"));
	Process_3->setDelayExpression("tria(0.5,1,1.5)");
	Process_3->setDelayTimeUnit(Util::TimeUnit::minute);
	Process_3->setAllocationType(Util::AllocationType::ValueAdded);

	// process 4
	Process* Process_4 =  plugins->newInstance<Process>(model, "MailRoom");
	Process_4->getSeizeRequests()->insert(new SeizableItem(Resource_2));
	Process_4->setQueueableItem(new QueueableItem(model, "MailRoomQueue")); // ??
	Process_4->setDelayExpression("tria(0.5,1,1.5)");
	Process_4->setDelayTimeUnit(Util::TimeUnit::minute);
	Process_4->setAllocationType(Util::AllocationType::ValueAdded);
	// dispose 1
	Dispose* Dispose_1 = plugins->newInstance<Dispose>(model, "Depart");

	// connect model components
	Create_2->getConnections()->insert(Process_3);
	Process_3->getConnections()->insert(Process_4);
	Process_4->getConnections()->insert(Dispose_1);
	
	// set simulation parameters
	ModelSimulation* sim = model->getSimulation();
	sim->setNumberOfReplications(300);
	model->getSimulation()->setReplicationLength(1500, Util::TimeUnit::minute);
	sim->setWarmUpPeriod(75);
	sim->setWarmUpPeriodTimeUnit(Util::TimeUnit::minute);
	model->getSimulation()->setTerminatingCondition("count(Depart.CountNumberIn)>1000");
	model->save("./models/Smart_ResourceScheduleCosting.gen");
	model->getSimulation()->start();
	// run the simulation
	sim->start();
	for (int i = 0; i < 1e9; i++); // give UI some time to finish std::cout
	// free memory
	delete genesys;
	
	return 0;
}
