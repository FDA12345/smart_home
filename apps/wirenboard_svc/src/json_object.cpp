#include "stdafx.h"
#include "json_object.h"

using namespace json;

const Value& json::Value::NullValue()
{
	//static Value nullValue;
	//return nullValue;
	return *((Value*)(0));
}

RootPtr json::CreateRoot()
{
	return nullptr; //ValueImpl();
}
