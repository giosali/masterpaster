#include "pch.h"
#include "CppUnitTest.h"
#include "../MasterPaster/io.h"
#include "../MasterPaster/io.cpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace tests {

	TEST_CLASS(tests) {
	public:
		
		TEST_METHOD(getAppDataDirectoryPath) {
			// Should not return an empty string.
            std::filesystem::path actual = io::getAppDataDirectoryPath();
			Assert::IsFalse(actual.empty());
		}

		TEST_METHOD(getSettingsIniPath) {
            // Should not return an empty string.
            std::filesystem::path actual = io::getSettingsIniPath();
            Assert::IsFalse(actual.empty());
		}
	};

} // namespace tests
