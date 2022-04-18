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
            std::string actual = io::getAppDataDirectoryPath();
			Assert::IsFalse(actual.empty());
		}

		TEST_METHOD(createSettings) {
			// Should not return an empty unordered map.
            std::unordered_map<std::string, std::string> umap = io::createSettings();
			Assert::IsFalse(umap.empty());

			// "runAtStartup" should exist and be initialized to "true".
            Assert::AreEqual(umap["runAtStartup"], std::string("true"));
		}
	};

} // namespace tests
