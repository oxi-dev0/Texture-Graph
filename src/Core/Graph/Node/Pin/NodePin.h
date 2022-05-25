#pragma once

#include "../../../Data/Types.h"

#include <vector>
#include <string>

enum Direction {
	In,
	Out
};

class NodePin
{
public:
	int pinId;
	int nodeId;
	Types::DataType type;
	Direction dir;

	std::string displayName;

	int inNodeId;
	int inPinId;

	std::vector<int> outNodeIds;
	std::vector<int> outPinIds;

	NodePin();
	NodePin(int pinId_, int nodeId_, Types::DataType type_, Direction dir_, std::string displayName_);
};

