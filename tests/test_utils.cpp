#include "pch.h"
#include "CppUnitTest.h"
#include "../MasterPaster/utils.h"
#include "../MasterPaster/utils.cpp"
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace tests {

    TEST_CLASS(tests) {
        public:

            TEST_METHOD(stringToBool) {
                // Should handle true strings.
                std::vector<std::string> trueStrs = { "true", "True" };
                for (int i = 0; i < trueStrs.size(); i++) { 
                    Assert::IsTrue(utils::stringToBool(trueStrs[i]));
                }

                // Should handle false strings.
                std::vector<std::string> falseStrs = { "false", "False" };
                for (int i = 0; i < falseStrs.size(); i++) {
                    Assert::IsFalse(utils::stringToBool(falseStrs[i]));
                }
            }

            TEST_METHOD(boolToString) {
                // When given true
                // Should return "true".
                Assert::AreEqual(utils::boolToString(true), std::string("true"));

                // When given false
                // Should return "false".
                Assert::AreEqual(utils::boolToString(false), std::string("false"));
            }
    };

} // namespace test
