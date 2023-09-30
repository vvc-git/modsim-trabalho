/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ParserChangesInformation.h
 * Author: rlcancian
 *
 * Created on 11 de Setembro de 2019, 20:42
 */

#ifndef PARSERCHANGESINFORMATION_H
#define PARSERCHANGESINFORMATION_H

#include <string>
#include <list>

class ParserChangesInformation {
public:
	ParserChangesInformation();
	virtual ~ParserChangesInformation() = default;

public: // gets and sets
private:
	std::string _includes = "";
	std::string _tokens = "";
	std::string _typeObjs = "";
	std::string _expressions;
	std::string _expressionProductions = "";
	std::string _assignments = "";
	std::string efunctionProdutions = "";

};

#endif /* PARSERCHANGESINFORMATION_H */

