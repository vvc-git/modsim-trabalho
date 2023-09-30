/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CollectorDefaultImpl1.h
 * Author: rafael.luiz.cancian
 *
 * Created on 1 de Agosto de 2018, 20:43
 */

#ifndef COLLECTORDEFAULTIMPL1_H
#define COLLECTORDEFAULTIMPL1_H

#include "Collector_if.h"
//namespace GenesysKernel {

class CollectorDefaultImpl1 : public Collector_if {
public:
	CollectorDefaultImpl1();
	virtual ~CollectorDefaultImpl1() = default;
public:
	void clear();
	void addValue(double value);
	double getLastValue();
	unsigned long numElements();
public:
	void setAddValueHandler(CollectorAddValueHandler addValueHandler);
	void setClearHandler(CollectorClearHandler clearHandler);
private:
	double _lastValue;
	unsigned long _numElements = 0;
	CollectorAddValueHandler _addValueHandler = nullptr;
	CollectorClearHandler _clearHandler = nullptr;
};
//namespace\\}
#endif /* COLLECTORDEFAULTIMPL1_H */

