#include "NodePin.h"

NodePin::NodePin() {
	pinIndex = 0;
	type = Types::DataType::DataType_Bool;
	dir = Direction::In;
	displayName = "";
	inNodeId = -1;
	inPinIndex = -1;
	outNodeIds = std::vector<int>();
	outPinIndexes = std::vector<int>();
}

NodePin::NodePin(int pinId_, Types::DataType type_, Direction dir_, std::string displayName_) : NodePin()  {
	pinIndex = pinId_;
	type = type_;
	dir = dir_;
	displayName = displayName_;
}
