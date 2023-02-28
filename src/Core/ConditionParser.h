#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <functional>
#include <unordered_map>

namespace ConditionParser
{
	enum ExpressionOperation
	{
		EQUAL,
		NOTEQUAL
	};

	const std::unordered_map<std::string, ExpressionOperation> expressionOperationMap = {
		{"==", ExpressionOperation::EQUAL},
		{"!=", ExpressionOperation::NOTEQUAL}
	};

	const std::unordered_map<ExpressionOperation, std::string> expressionOperationToString = {
		{ExpressionOperation::EQUAL, "=="},
		{ExpressionOperation::NOTEQUAL, "!="}
	};

	class ExpressionBase
	{
	private:
		std::string expr;

	public:
		ExpressionBase() : expr("") {}
		ExpressionBase(std::string _expr) : expr(_expr) {}

		virtual inline bool Eval(std::function<bool(bool, std::string, ExpressionOperation, bool, std::string)> evalFunc) const {
			bool invertVar1 = false;
			std::stringstream var1;
			bool invertVar2 = false;
			std::stringstream var2;
			std::stringstream op;
			int stage = 0;

			for (const char& c : expr) {
				switch (stage)
				{
				case 0:
				{
					if ((c == '!') && (invertVar1 == false)) {
						invertVar1 = true;
					}
					else if (std::string("=!").find(c) != std::string::npos) {
						stage = 1;
						op << c;
					}
					else {
						var1 << c;
					}
				} break;
				case 1:
				{
					if (std::string("=!").find(c) != std::string::npos) {
						if (c == '!' && op.str().size() > 0) {
							stage = 2;
							invertVar2 = true;
						}
						else {
							op << c;
						}
					}
					else {
						stage = 2;
						var2 << c;
					}
				} break;
				case 2:
				{
					var2 << c;
				} break;
				}
			}

			ExpressionOperation opEnum = expressionOperationMap.find(op.str())->second;
			return evalFunc(invertVar1, var1.str(), opEnum, invertVar2, var2.str());
		}
	};

	class ExpressionPair : public ExpressionBase
	{
	private:
		ExpressionBase* lVal;
		ExpressionBase* rVal;
		std::string combiner; // &&, ||
	
	public:
		ExpressionPair(ExpressionBase* _lVal, ExpressionBase* _rVal, std::string _combiner) : lVal(_lVal), rVal(_rVal), combiner(_combiner) {}
		~ExpressionPair() { delete lVal; delete rVal; }

		static inline ExpressionBase* Parse(const std::string& expr) {
			bool combinerInTopLevel = false;
			int scanLevel = 0;
			std::stringstream combinerCache;
			for (const char& c : expr) {
				if (c == '(') { scanLevel += 1; }
				if (c == ')') { scanLevel -= 1; }
				if (std::string("&|").find(c) != std::string::npos) {
					combinerCache << c;
					if (combinerCache.str() == "&&" || combinerCache.str() == "||") {
						if (scanLevel == 0) {
							combinerInTopLevel = true;
							combinerCache.str("");
							break;
						}
						else {
							combinerCache.str("");
						}
					}
					if (combinerCache.str().size() > 2) {
						combinerCache.str("");
					}
				}
			}

			if (combinerInTopLevel) {
				std::stringstream lValStr;
				std::stringstream rValStr;
				std::stringstream combinerStr;
				std::stringstream cache;
				std::stringstream specialCache;
				int level = 0;
				
				for (size_t i = 0; i < expr.size(); i++) {
					const char& c = expr[i];
					if (std::string("&|").find(c) != std::string::npos) {
						specialCache << c;
						if ((specialCache.str() == "&&" || specialCache.str() == "||") && level == 0) {
							combinerStr.str(specialCache.str());
							lValStr.str(cache.str());
							rValStr.str(expr.substr(i + 1));
							if (rValStr.str()[0] == '(') {
								rValStr.str(rValStr.str().substr(1, rValStr.str().size() - 2));
							}
							cache.str("");
							specialCache.str("");
							break;
						}
						else if (specialCache.str() == "&&" || specialCache.str() == "||") {
							cache << specialCache.str();
							specialCache.str("");
						}
					}
					else if (std::string("()").find(c) != std::string::npos){
						if (c == '(') {
							level += 1;
						}
						else {
							level -= 1;
						}
					}
					else {
						cache << c;
					}
				}

				return new ExpressionPair(ExpressionPair::Parse(lValStr.str()), ExpressionPair::Parse(rValStr.str()), combinerStr.str());
			}
			else {
				if (expr[0] == '(') {
					return ExpressionPair::Parse(expr.substr(1, expr.size() - 2));
				}
				else {
					return new ExpressionBase(expr);
				}
			}
		}

		virtual inline bool Eval(std::function<bool(bool, std::string, ExpressionOperation, bool, std::string)> evalFunc) const override {
			bool lValR = lVal->Eval(evalFunc);
			bool rValR = rVal->Eval(evalFunc);
			if (combiner == "&&") {
				return lValR && rValR;
			} else if (combiner == "||") {
				return lValR || rValR;
			}
			return false;
		}
	};
}