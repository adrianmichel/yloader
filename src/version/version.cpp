/*
Copyright (C) 2017  YLoader.com

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define V5_1 _T("5.1.0.0")
#define V5_BUILD1 _T( "5.0.0.1")
#define V5_BETA2 _T( "5.0.0.2")
#define V5 _T( "5.0")
#define V4 _T( "4.0.0.0")

#define BAD_VERSION_1 _T( "A.a.B.B")
#define BAD_VERSION_2 _T( "ASDF" )
#define BAD_VERSION_3 _T( "5,0,0,0")
#define BAD_VERSION_4 _T( "5.0.0.0.0")

using namespace yloader;

namespace version
{		
	TEST_CLASS(VersionUnitTest)
	{
	public:
		
		TEST_METHOD(TestSuccess)
		{
			Version v5_1(V5_1);
			Version v5(V5);
			Version v5_beta1(V5_BUILD1);
			Version v5_beta2(V5_BETA2);
			Version v4(V4);

			Assert::IsTrue(v5_1 > v5_beta1);
			Assert::IsTrue(v5_1 > v5_beta2);
			Assert::IsTrue(v5_beta2 > v5_beta1);
			Assert::IsTrue(v5_1 > v4);
			Assert::IsTrue(v5_1 > v5);
			Assert::IsTrue(v5_1 > v5_beta1);
			Assert::IsTrue(v5_1 > v5_beta2);
		}

		TEST_METHOD(TestFailure)
		{
			Assert::ExpectException< VersionException>([]() { Version V(BAD_VERSION_1); });
			Assert::ExpectException< VersionException>([]() { Version V(BAD_VERSION_2); });
			Assert::ExpectException< VersionException>([]() { Version V(BAD_VERSION_3); });
			Assert::ExpectException< VersionException>([]() { Version V(BAD_VERSION_4); });
		}
	};
}