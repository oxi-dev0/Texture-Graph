#pragma once

#include "../../../Types.h"

#include <vector>
#include <string>

enum Direction {
	In,
	Out
};

class NodePin
{
public:
	int pinIndex;
	Types::DataType type;
	Direction dir;

	std::string displayName;

	int inNodeId;
	int inPinIndex;

	std::vector<int> outNodeIds;
	std::vector<int> outPinIndexes;

	NodePin();
	NodePin(int pinId_, Types::DataType type_, Direction dir_, std::string displayName_);
};

