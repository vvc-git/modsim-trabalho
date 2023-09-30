/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ParserDefaultImpl2.h
 * Author: Joao Ortigara (20180801-20181207) rafael.luiz.cancian (20181208-...(
 *
 * 
 */

#ifndef PARSERDEFAULTIMPL2_H
#define PARSERDEFAULTIMPL2_H

#include <string>
#include "Parser_if.h"
#include "Model.h"
#include "../../parser/Genesys++-driver.h"


//namespace GenesysKernel {

class ParserDefaultImpl2 : public Parser_if {
public:
	ParserDefaultImpl2(Model* model, Sampler_if* sampler, bool throws = false);
	virtual ~ParserDefaultImpl2() = default;
public:
	virtual double parse(const std::string expression) override; // may throw exception
	virtual double parse(const std::string expression, bool* success, std::string* errorMessage) override;
	virtual std::string* getErrorMessage() override;
	virtual void setSampler(Sampler_if* _sampler) override;
	virtual Sampler_if* getSampler() const override;
	virtual genesyspp_driver getParser() const override;
private:
	Model* _model;
	genesyspp_driver _wrapper;
};
//namespace\\}
#endif /* PARSERDEFAULTIMPL2_H */

