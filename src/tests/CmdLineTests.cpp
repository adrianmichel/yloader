#include "pch.h"
#include "CppUnitTest.h"

#include <CmdLine.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

std::wstring lopt(const LPCWSTR* name) {
	return L"--" + wLongName(name);
}

std::wstring sopt(const LPCWSTR* name) {
	auto shortName = wShortName(name);
	if (shortName) {
		return L"-" + *shortName;
	}
	else {
		throw std::exception();
	}
}

std::wstring larg(const LPCWSTR* name, const std::wstring& value) {
	return lopt(name) + L" \"" + value + L"\"";
}

std::wstring sarg(const LPCWSTR* name, const std::wstring& value) {
	return sopt(name) + L" \"" + value + L"\"";
}

using Opt = std::wstring(*)(const LPCWSTR*);
using Arg = std::wstring(*)(const LPCWSTR*, const std::wstring&);

namespace tests
{
	TEST_CLASS(tests)
	{
	public:

		TEST_METHOD(HelpTest)
		{
			CmdLine cl1(L"");
			Assert::IsFalse(cl1.help());

			CmdLine cl2(lopt(COMMAND_LINE_HELP));
			Assert::IsTrue(cl2.help());

			CmdLine cl3(sopt(COMMAND_LINE_HELP));
			Assert::IsTrue(cl3.help());
		}

		TEST_METHOD(AutoStartTest) {
			CmdLine cl1(L"");
			Assert::IsFalse(cl1.autoStart().isSet());

			CmdLine cl2(lopt(AUTO_START));
			Assert::IsTrue(cl2.autoStart());

			CmdLine cl3(sopt(AUTO_START));
			Assert::IsTrue(cl3.autoStart());
		}

		TEST_METHOD(AdjustTest) {
			// not on the cl
			CmdLine cl1(L"");
			Assert::IsFalse(cl1.autoStart().isSet());

			// needs an argument
			auto test1 = [](Opt f) {
				Assert::ExpectException< CmdLineException >([&f]() { CmdLine cl2(f(ADJUST)); });
			};

			test1(lopt);
			test1(sopt);


			auto test2 = [](Arg f) {
				CmdLine cl1(f(ADJUST, L"true"));
				Assert::IsTrue(cl1.adjust());

				CmdLine cl2(f(ADJUST, L"false"));
				Assert::IsFalse(cl2.adjust());
			};

			test2(larg);
			test2(sarg);
		}
	};
}
