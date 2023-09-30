/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Resource.h
 * Author: rafael.luiz.cancian
 *
 * Created on 21 de Agosto de 2018, 16:52
 */

#ifndef RESOURCE_H
#define RESOURCE_H

#include "../../kernel/simulator/ModelDataDefinition.h"
#include "../../kernel/simulator/StatisticsCollector.h"
#include "../../kernel/simulator/ModelDataManager.h"
#include "../../kernel/simulator/Counter.h"
#include "../../kernel/simulator/Plugin.h"
#include "../../kernel/simulator/OnEventManager.h"

#include "Failure.h"
#include "Schedule.h"

#include <functional>



class SeizableItem;

/*!
Resource module
DESCRIPTION
This data module defines the resources in the simulation system, including costing
information and resource availability. Resources may have a fixed capacity that does
not vary over the simulation run or may operate based on a schedule. Resource
failures and states can also be specified in this module.
TYPICAL USES
 * Equipment (machinery, cash register, phone line)
 * People (clerical, order processing, sales clerks, operators)
PROMPTS
Prompt Description
Name The name of the resource whose characteristics are being
defined. This name must be unique.
Type Method for determining the capacity for a resource. Fixed
Capacity will not change during the simulation run. Based on
Schedule signifies that a Schedule module is used to specify the
capacity and duration information for the resource.
Capacity Number of resource units of a given name that are available to
the system for processing. Applies only when Type is Fixed
Capacity.
Schedule Name Identifies the name of the schedule to be used by the resource.
The schedule defines the capacity of a resource for a given
period of time. Applies only when type is Schedule.
Schedule Rule Dictates when the actual capacity change is to occur when a
decrease in capacity is required for a busy resource unit. Applies
only when Type is Schedule.
Busy/TimeUnit Cost per timeUnit of a resource that is processing an entity. The
resource becomes busy when it is originally allocated to an entity
and becomes idle when it is released. During the time when it is
busy, cost will accumulate based on the busy/timeUnit cost. The busy
cost per timeUnit is automatically converted to the appropriate base
time unit specified within the Replication Parameters page of the
Run > Setup menu item.
Idle/TimeUnit Cost per timeUnit of a resource that is idle. The resource is idle while
it is not processing an entity. During the time when it is idle, cost
will accumulate based on the idle/timeUnit cost. The idle cost per
timeUnit is automatically converted to the appropriate base time unit
specified within the Replication Parameters page of the Run >
Setup menu item.
Per Use Cost of a resource on a usage basis, regardless of the time for
which it is used. Each time the resource is allocated to an entity,
it will incur a per-use cost.
StateSet Name Name of states that the resource may be assigned during the
simulation run.
Initial State Initial state of a resource. If specified, the name must be defined
within the repeat group of state names. This field is shown only
when a StateSet Name is defined.
Failures Lists all failures that will be associated with the resource.
 * Failure Name–Name of the failure associated with the
resource.
 * Failure Rule–Behavior that should occur when a failure is to
occur for a busy resource unit.
Report Statistics Specifies whether or not statistics will be collected automatically
and stored in the report database for this resource.
 */
class Resource : public ModelDataDefinition {
public:
	typedef std::function<void(Resource*) > ResourceEventHandler;
	typedef std::pair<std::pair<ResourceEventHandler, ModelComponent*>, unsigned int> SortedResourceEventHandler;

	template<typename Class>
	static ResourceEventHandler SetResourceEventHandler(void (Class::*function)(Resource*), Class * object) {
		return std::bind(function, object, std::placeholders::_1);
	}

	enum class ResourceState : int {
		IDLE = 1, BUSY = 2, FAILED = 3, INACTIVE = 4, OTHER = 5
	};

public:
	//Resource(Model* model);
	Resource(Model* model, std::string name = "");
	virtual ~Resource() = default;
public:
	virtual std::string show();
public: // static
	static PluginInformation* GetPluginInformation();
	static ModelDataDefinition* LoadInstance(Model* model, PersistenceRecord *fields);
	static ModelDataDefinition* NewInstance(Model* model, std::string name = "");
public:
	bool seize(unsigned int quantity, double priority = 0);
	void release(unsigned int quantity);
	void insertFailure(Failure* failure);
	void removeFailure(Failure* failure);
	double getInstantCapacityUtilization() const;
	double getCapacityUtilization() const;
	double getSeizedUtilization() const;
	double getLastTimeSeized() const; // used only by "Release" component
	void addReleaseResourceEventHandler(ResourceEventHandler eventHandler, ModelComponent* component, unsigned int priority);
public: // g&s
	void setResourceState(ResourceState _resourceState);
	Resource::ResourceState getResourceState() const;
	void setCapacity(unsigned int _capacity);
	unsigned int getCapacity() const;
	void setCostBusyTimeUnit(double _costBusyTimeUnit);
	double getCostBusyTimeUnit() const;
	void setCostIdleTimeUnit(double _costIdleTimeUnit);
	double getCostIdleTimeUnit() const;
	void setCostPerUse(double _costPerUse);
	double getCostPerUse() const;
	void setCapacitySchedule(Schedule* _capacitySchedule);
	Schedule* getCapacitySchedule() const;
	unsigned int getNumberBusy() const;

protected: // protected must override
	virtual bool _loadInstance(PersistenceRecord *fields);
	virtual void _saveInstance(PersistenceRecord *fields, bool saveDefaultValues);
protected: // protected could override
	virtual bool _check(std::string* errorMessage);
	virtual void _createInternalAndAttachedData();
	virtual void _initBetweenReplications();

private: //methods
	void _notifyReleaseEventHandlers(); ///< Notify observer classes that some of the resource capacity has been released. It is useful for allocation components (such as Seize) to know when an entity waiting into a queue can try to seize the resource again
	void _onReplicationEnd(SimulationEvent* se); ///< Nofified whe replication ended to update cstats based on final replication length
	void _fail();
	void _active();
	void _checkFailByCount();
	friend class Failure;

private:

	const struct DEFAULT_VALUES {
		const unsigned int capacity = 1;
		const double cost = 1.0;
		const ResourceState resourceState = ResourceState::IDLE;
	} DEFAULT;
	unsigned int _capacity = DEFAULT.capacity;
	double _costBusyTimeUnit = DEFAULT.cost;
	double _costIdleTimeUnit = DEFAULT.cost;
	double _costPerUse = DEFAULT.cost;
	ResourceState _resourceState = DEFAULT.resourceState;
private: // only gets
	unsigned int _numberBusy = 0;
	double _lastTimeSeized = 0.0; // @TODO: It won't work for resources with capacity>1, when not all capacity is seized and them some more are seized. Seized time of first units will be lost. I don't have a solution so far
	double _lastTimeReleased = 0.0;
	double _lastTimeFailed = 0.0;
	double _lastTimeCapacityEvaluated = 0.0;
	double _lastTimeAnythingNumberBusy = 0.0;
	double _lastTimeIdle = 0.0;
	double _lastTimeBusy = 0.0;
	double _sumNumberBusyOverTime = 0.0;
	double _sumCapacityOverTime = 0.0;
	bool _isActive = true;
private: // not gets nor sets
	unsigned int _originalCapacity; // used for failing purposes, when _capacity changes to 0
private: //1::n
	List<SortedResourceEventHandler*>* _resourceEventHandlers = new List<SortedResourceEventHandler*>();
	List<Failure*>* _failures = new List<Failure*>();
private: // attached elements
	Schedule* _capacitySchedule = nullptr;
private: // internal elements
	StatisticsCollector* _cstatTimeSeized = nullptr;
	StatisticsCollector* _cstatTimeFailed = nullptr;
	StatisticsCollector* _cstatProportionSeized = nullptr;
	StatisticsCollector* _cstatCapacityUtilization = nullptr;
	Counter* _counterTotalTimeSeized;
	Counter* _counterTotalTimeFailed;
	Counter* _counterNumSeizes;
	Counter* _counterNumReleases;
	Counter* _counterTotalCostPerUse;
	Counter* _counterTotalCostBusy;
	Counter* _counterTotalCostIdle;
};

#endif /* RESOURCE_H */

