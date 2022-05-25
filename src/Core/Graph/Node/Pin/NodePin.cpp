#include "NodePin.h"

NodePin::NodePin() {
	pinId = 0;
	nodeId = 0;
	type = Types::DataType::DataType_Bool;
	dir = Direction::In;
	displayName = "";
	inNodeId = 0;
	inPinId = 0;
	outNodeIds = std::vector<int>();
	outPinIds = std::vector<int>();
}

NodePin::NodePin(int pinId_, int nodeId_, Types::DataType type_, Direction dir_, std::string displayName_) {
	pinId = pinId_;
	nodeId = nodeId_;
	type = type_;
	dir = dir_;
	displayName = displayName_;
}
