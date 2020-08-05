#include <gtest/gtest.h>
#include <utility/ThreadPool.h>
using namespace GreyDawn;

TEST(tests, ThreadPoolTest)
{
	std::shared_ptr<int> spVar;
	size_t taskNumber = 0;
	{
		ThreadPool tp("ThreadPool");
		std::shared_ptr<int> spCount = std::make_shared<int>(0);
		std::shared_ptr<std::mutex> spMutex = std::make_shared<std::mutex>();
		bool Set = false;
		for (size_t i = 0; i < taskNumber; i++)
		{
			tp.AsyncExecuteTask([&, spCount, spMutex] {
				for (size_t i = 0; i < 1024; i++)
				{
					std::lock_guard<std::mutex> Lock(*spMutex);
					(*spCount)++;
				}
			});
		}
		spVar = spCount;
	}
	ASSERT_EQ(*spVar, taskNumber * 1024);
	ASSERT_TRUE("Pass");
}



