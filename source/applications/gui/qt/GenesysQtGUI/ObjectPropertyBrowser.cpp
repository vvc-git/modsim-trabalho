#include "ObjectPropertyBrowser.h"
#include <QDebug>
//#include <QProperty>
#include <iostream>

ObjectPropertyBrowser::ObjectPropertyBrowser(QWidget* parent) : QtTreePropertyBrowser(parent) {
	variantManager = new QtVariantPropertyManager(this);
	setFactoryForManager(variantManager, new QtVariantEditorFactory);
	this->setResizeMode(QtTreePropertyBrowser::Interactive);
	this->setAccessibleDescription("Editor for model elements");
	this->setHeaderVisible(true);
	this->setIndentation(10);
	this->setRootIsDecorated(true);
}

void ObjectPropertyBrowser::setActiveObject(QObject *obj, ModelDataDefinition* mdd) {
	clear();
	variantManager->clear();
	propertyMap.clear();
	if(currentlyConnectedObject) currentlyConnectedObject->disconnect(this);
	currentlyConnectedObject = obj;
	if(!obj) return;

	std::map<std::string, QtVariantProperty*> propGroupMap;
	QtVariantProperty *groupProperty = nullptr;
	QtVariantProperty *property = nullptr;

	if (mdd != nullptr) {
		std::string className;
		for (PropertyBase* prop: *mdd->getProperties()->list()) {
			className = prop->getName(); //@TODO Era para ser getClassName();
			std::map<std::string, QtVariantProperty*>::iterator it;
			if ((it = propGroupMap.find(className))==propGroupMap.end()) {
				groupProperty = variantManager->addProperty(QVariant::Char, className.c_str());
				propGroupMap.insert({className, groupProperty});
			} else {
				groupProperty = (*it).second;
			}
			this->addProperty(groupProperty);
			obj->setProperty(prop->getName().c_str(), 1);
			property = variantManager->addProperty(QVariant::Double, prop->getName().c_str());
			if (property != nullptr) {
				property->setEnabled(true);//mp.isWritable());
				propertyMap[property] = prop->getName().c_str();
				groupProperty->addSubProperty(property);
				//this->addProperty(property);
			}
		}
	}

	groupProperty = variantManager->addProperty(QVariant::Char,"Graphical");
	this->addProperty(groupProperty);
	propGroupMap.insert({"Graphical", groupProperty});

	// add properties based on meta-object
	for(int i=1; i< obj->metaObject()->propertyCount();i++) {
		QMetaProperty mp = obj->metaObject()->property(i);
		property =variantManager->addProperty(mp.type(),mp.name());
		if (property != nullptr) {
			property->setEnabled(mp.isWritable());
			propertyMap[property] = mp.name();
			groupProperty->addSubProperty(property);
			//this->addProperty(property);
		}
	}
	/*
	for(QByteArray name :obj->dynamicPropertyNames()) {
		//QMetaProperty mp = obj->metaObject()->property(i);
		QVariant variant = obj->property(name);
		QtVariantProperty *property = variantManager->addProperty(2, name);//addProperty(mp.type(),mp.name());
		if (property != nullptr) {
			property->setEnabled(true);//mp.isWritable());
			propertyMap[property] = name;//mp.name();
			addProperty(property);
		}
	}
	*/

	/*
	if (mdd != nullptr) {
		std::string className;
		for (PropertyG* prop: *mdd->getProperties()->list()) {
			className = prop->getClassname();
			std::map<std::string, QtVariantProperty*>::iterator it;
			//if ((it = propGroupMap.find(className))==propGroupMap.end()) {
			//	groupProperty = variantManager->addProperty(2, className.c_str());
			//	propGroupMap.insert({className, groupProperty});
			//} else {
			//	groupProperty = (*it).second;
			//}
			QtVariantProperty *property = variantManager->addProperty(1, prop->getName().c_str());
			if (property != nullptr) {
				property->setEnabled(true);//mp.isWritable());
				propertyMap[property] = prop->getName().c_str();
				//groupProperty->addSubProperty(property);
				addProperty(property);
			}
		}
	}

	*/

	connect(obj, SIGNAL(propertyChanged()), this, SLOT(objectUpdated()));
	objectUpdated();
}

void ObjectPropertyBrowser::valueChanged(QtProperty *property, const QVariant &value)
{
	currentlyConnectedObject->setProperty(propertyMap[property], value);
	objectUpdated();
}

void ObjectPropertyBrowser::objectUpdated()
{
	disconnect(variantManager, SIGNAL(valueChanged(QtProperty*, QVariant)), this, SLOT(valueChanged(QtProperty*, QVariant)));
	QMapIterator<QtProperty*, const char*> i(propertyMap);
	while(i.hasNext()) {
		i.next();
		variantManager->setValue(i.key(), currentlyConnectedObject->property(i.value()));
	}
	connect(variantManager, SIGNAL(valueChanged(QtProperty*, QVariant)), this, SLOT(valueChanged(QtProperty*, QVariant)));
}
