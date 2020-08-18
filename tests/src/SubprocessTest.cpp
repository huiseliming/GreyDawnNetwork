#include <gtest/gtest.h>
#include <utility/Utility.h>
using namespace GreyDawn;



TEST(tests, SubprocessTest)
{

	bool child_exited_check = true;
	bool child_crashed_check = false;
	{
		Subprocess subprocess;
		subprocess.Create(
			GetExecuteFileDirectoryAbsolutePath() + "\\SubprocessExample.exe",
			{},
			[&] 
			{
				child_exited_check = true;
			},
			[&] 
			{
				child_crashed_check = true;
			}
		);
	}
	{
		ASSERT_TRUE(child_exited_check);
		ASSERT_FALSE(child_crashed_check);
	}
}




