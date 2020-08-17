#include <gtest/gtest.h>
#include <utility/Utility.h>
using namespace GreyDawn;



TEST(tests, SubprocessTest)
{
	Subprocess subprocess;
	subprocess.Create(
		GetExecuteFileDirectoryAbsolutePath() + "/SubprocessExample.exe",
		{},
		[] { GD_LOG_INFO("child_exited"); },
		[] { GD_LOG_INFO("child_crashed"); }
	);

}




