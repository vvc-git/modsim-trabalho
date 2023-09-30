#pragma once

#include <string>
#include <functional>
#include <list>
#include "../util/Util.h"

//namespace GenesysKernel {


class PropertyGenesysBase {
public:
	PropertyGenesysBase(std::string className, std::string elementName, std::string propertyName, std::string whatsThis="") {
		_className=className;
		_elementName=elementName;
		_propertyName=propertyName;
		_whatsThis=whatsThis;
	}
	virtual ~PropertyGenesysBase() = default;
public:
	std::string whatsThis() const { return _whatsThis; }
	std::string getClassname() const { return _className; }
	std::string getElementName() const { return _elementName; }
	std::string getName() const { return _propertyName; }
	std::string propertyType() const { return _propertyType; }
	void setWhatsThis(const std::string text);
	void setName(std::string name) { _propertyName = name; }
protected:
	std::string _elementName;
	std::string _className;
	std::string _propertyName;
	std::string _propertyType;
	std::string _whatsThis;
};

class PropertyGenesys: public PropertyGenesysBase {
public:
	PropertyGenesys(std::string className, std::string elementName, std::string propertyName, std::string whatsThis="", std::string propertyId="", std::string toolTip="", std::string statusTip="")
		: PropertyGenesysBase(className, elementName, propertyName, whatsThis){
		_propertyId=propertyId;
		_toolTip=toolTip;
		_statusTip=statusTip;
	}
	virtual ~PropertyGenesys() = default;
public:
	std::list<PropertyGenesys *> subProperties() const { return _subProperties; };
	std::string toolTip() const { return _toolTip; }
	std::string statusTip() const { return _statusTip; }
	std::string propertyName() const { return _propertyName; }
	std::string propertyId() const { return _propertyId; }
	bool isEnabled() const { return _isEnabled; }
	bool isModified() const { return _isModified; }
	bool isSubProperty()const { return _isSubProperty; }
	bool hasValue() const { return _hasValue; }
	std::string valueText() const { return "?"; } // TODO
	void setToolTip(const std::string text);
	void setStatusTip(const std::string text);
	void setWhatsThis(const std::string text);
	void setPropertyName(const std::string text);
	void setPropertyId(const std::string text);
	void setEnabled(bool enable);
	void setModified(bool modified);
public:
	virtual bool compare(PropertyGenesys* otherProperty)const { return true; } // TODO
public:
	void addSubProperty(PropertyGenesys *property) {}
	void insertSubProperty(PropertyGenesys *property, PropertyGenesys *afterProperty) {}
	void removeSubProperty(PropertyGenesys *property) {}
protected:
	void propertyChanged() {}
protected:
	//friend class QtAbstractPropertyManager;
	std::string _className;
	std::string _propertyName;
	std::string _whatsThis;
	std::string _propertyId;
	std::string _toolTip;
	std::string _statusTip;
	std::list<PropertyGenesys*> _subProperties;
	bool _isEnabled;
	bool _isModified;
	bool _hasValue;
	bool _isSubProperty;
};

class SimulationControl: public PropertyGenesysBase {
public:
	SimulationControl(std::string className, std::string elementName, std::string propertyName, std::string whatsThis="")
		: PropertyGenesysBase(className, elementName, propertyName, whatsThis){
	}
	std::string show() const {
		std::string msg = "classname="+_className+ ", elementName="+_elementName+", name=\""+_propertyName+"\"";
		if (_readonly) {
			msg += " (readonly)";
		}
		//msg += ", type="+_propertyType;
		msg += ", value="+getValue();
		return msg;
	}
	bool isReadOnly() const { return _readonly; }
public:
	virtual std::string getValue() const  = 0;
	virtual void setValue(std::string value) = 0;
protected:
	bool _readonly;
};

// -----------------------------------------------------------

typedef std::function<std::string()> GetterString;
typedef std::function<void(std::string)> SetterString;
class SimulationControlString: public SimulationControl {
public:
//	SimulationControlString(GetterString getter, std::string className, std::string elementName, std::string propertyName, std::string whatsThis="") : SimulationControl(className, elementName, propertyName, whatsThis) {
//		SimulationControlString(getter, nullptr, className, propertyName, whatsThis);
//	}
	SimulationControlString(GetterString getter, SetterString setter, std::string className, std::string elementName, std::string propertyName, std::string whatsThis="") : SimulationControl(className, elementName, propertyName, whatsThis) {
		_getter= getter;
		_setter = setter;
		_readonly = setter == nullptr;
		_propertyType = Util::TypeOf<std::string>();
	}
public:
	virtual std::string getValue() const override { return _getter(); }
	virtual void setValue(std::string value) override { _setter(value); };
private:
	GetterString _getter;
	SetterString _setter;
};


// -----------------------------------------------------------

typedef std::function<double()> GetterDouble;
typedef std::function<void(double)> SetterDouble;
class SimulationControlDouble: public SimulationControl {
public:
//	SimulationControlDouble(GetterDouble getter, std::string className, std::string elementName, std::string propertyName, std::string whatsThis="") : SimulationControl(className, elementName, propertyName, whatsThis) {
//		SimulationControlDouble(getter, nullptr, className, propertyName, whatsThis);
//	}
	SimulationControlDouble(GetterDouble getter, SetterDouble setter, std::string className, std::string elementName, std::string propertyName, std::string whatsThis="") : SimulationControl(className, elementName, propertyName, whatsThis){
		_getter= getter;
		_setter = setter;
		_readonly = setter == nullptr;
		_propertyType = Util::TypeOf<double>();
	}
public:
	virtual std::string getValue() const override { return std::to_string(_getter()); }
	virtual void setValue(std::string value) override { _setter(std::stod(value)); };
private:
	GetterDouble _getter;
	SetterDouble _setter;
};

// -----------------------------------------------------------

typedef std::function<bool()> GetterBool;
typedef std::function<void(bool)> SetterBool;
class SimulationControlBool: public SimulationControl {
public:
//	SimulationControlBool(GetterBool getter, std::string className, std::string elementName, std::string propertyName, std::string whatsThis="") : SimulationControl(className, elementName, propertyName, whatsThis) {
//		SimulationControlBool(getter, nullptr, className, propertyName, whatsThis);
//	}
	SimulationControlBool(GetterBool getter, SetterBool setter, std::string className, std::string elementName, std::string propertyName, std::string whatsThis="") : SimulationControl(className, elementName, propertyName, whatsThis){
		_getter= getter;
		_setter = setter;
		_readonly = setter == nullptr;
		_propertyType = Util::TypeOf<bool>();
	}
public:
	virtual std::string getValue() const override { return std::to_string(_getter()); }
	virtual void setValue(std::string value) override { _setter(std::stoi(value)); };
private:
	GetterBool _getter;
	SetterBool _setter;
};


// -----------------------------------------------------------

typedef std::function<unsigned int()> GetterUInt;
typedef std::function<void(unsigned int)> SetterUInt;
class SimulationControlUInt: public SimulationControl {
public:
//	SimulationControlUInt(GetterUInt getter, std::string className, std::string elementName, std::string propertyName, std::string whatsThis="") : SimulationControl(className, elementName, propertyName, whatsThis) {
//		SimulationControlUInt(getter, nullptr, className, propertyName, whatsThis);
//	}
	SimulationControlUInt(GetterUInt getter, SetterUInt setter, std::string className, std::string elementName, std::string propertyName, std::string whatsThis="") : SimulationControl(className, elementName, propertyName, whatsThis){
		_getter= getter;
		_setter = setter;
		_readonly = setter == nullptr;
		_propertyType = Util::TypeOf<unsigned int>();
	}
public:
	virtual std::string getValue() const override { return std::to_string(_getter()); }
	virtual void setValue(std::string value) override { _setter(std::stoul(value)); };
private:
	GetterUInt _getter;
	SetterUInt _setter;
};

// -----------------------------------------------------------

typedef std::function<unsigned short()> GetterUShort;
typedef std::function<void(unsigned short)> SetterUShort;
class SimulationControlUShort: public SimulationControl {
public:
//	SimulationControlUShort(GetterUShort getter, std::string className, std::string elementName, std::string propertyName, std::string whatsThis="") : SimulationControl(className, elementName, propertyName, whatsThis) {
//		SimulationControlUShort(getter, nullptr, className, propertyName, whatsThis);
//	}
	SimulationControlUShort(GetterUShort getter, SetterUShort setter, std::string className, std::string elementName, std::string propertyName, std::string whatsThis="") : SimulationControl(className, elementName, propertyName, whatsThis){
		_getter= getter;
		_setter = setter;
		_readonly = setter == nullptr;
		_propertyType = Util::TypeOf<unsigned int>();
	}
public:
	virtual std::string getValue() const override { return std::to_string(_getter()); }
	virtual void setValue(std::string value) override { _setter(std::stoul(value)); };
private:
	GetterUShort _getter;
	SetterUShort _setter;
};


// -----------------------------------------------------------

typedef std::function<int()> GetterInt;
typedef std::function<void(int)> SetterInt;
class SimulationControlInt: public SimulationControl {
public:
//	SimulationControlInt(GetterInt getter, std::string className, std::string elementName, std::string propertyName, std::string whatsThis="") : SimulationControl(className, elementName, propertyName, whatsThis) {
//		SimulationControlInt(getter, nullptr, className, propertyName, whatsThis);
//	}
	SimulationControlInt(GetterInt getter, SetterInt setter, std::string className, std::string elementName, std::string propertyName, std::string whatsThis="") : SimulationControl(className, elementName, propertyName, whatsThis){
		_getter= getter;
		_setter = setter;
		_readonly = setter == nullptr;
		_propertyType = Util::TypeOf<unsigned int>();
	}
public:
	virtual std::string getValue() const override { return std::to_string(_getter()); }
	virtual void setValue(std::string value) override { _setter(std::stoi(value)); };
private:
	GetterInt _getter;
	SetterInt _setter;
};

// -----------------------------------------------------------

typedef std::function<Util::TimeUnit()> GetterTimeUnit;
typedef std::function<void(Util::TimeUnit)> SetterTimeUnit;
class SimulationControlTimeUnit: public SimulationControl {
public:
//	SimulationControlTimeUnit(GetterTimeUnit getter, std::string className, std::string elementName, std::string propertyName, std::string whatsThis="") : SimulationControl(className, elementName, propertyName, whatsThis) {
//		SimulationControlTimeUnit(getter, nullptr, className, propertyName, whatsThis);
//	}
	SimulationControlTimeUnit(GetterTimeUnit getter, SetterTimeUnit setter, std::string className, std::string elementName, std::string propertyName, std::string whatsThis="") : SimulationControl(className, elementName, propertyName, whatsThis){
		_getter= getter;
		_setter = setter;
		_readonly = setter == nullptr;
		_propertyType = Util::TypeOf<Util::TimeUnit>();
	}
public:
	virtual std::string getValue() const override {
		int intVal = static_cast<int>(_getter());
		return std::to_string(intVal);
	}
	virtual void setValue(std::string value) override {
		int intVal = std::stoul(value);
		_setter(static_cast<Util::TimeUnit>(intVal));
	};
private:
	GetterTimeUnit _getter;
	SetterTimeUnit _setter;
};


typedef SimulationControl PropertyBase;


//namespace\\}
