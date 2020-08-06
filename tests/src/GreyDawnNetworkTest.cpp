#include <gtest/gtest.h>
#include <utility/Utility.h>
using namespace GreyDawn;

TEST(tests, ThreadPoolTest)
{
	std::shared_ptr<int> count_sum;
	size_t task_number = 0;
	{
		ThreadPool tp("ThreadPool");
		std::shared_ptr<int> sp_counter = std::make_shared<int>(0);
		std::shared_ptr<std::mutex> spMutex = std::make_shared<std::mutex>();
		bool Set = false;
		for (size_t i = 0; i < task_number; i++)
		{
			tp.AsyncExecuteTask([&, sp_counter, spMutex] {
				for (size_t i = 0; i < 1024; i++)
				{
					std::lock_guard<std::mutex> Lock(*spMutex);
					(*sp_counter)++;
				}
			});
		}
		count_sum = sp_counter;
	}
	ASSERT_EQ(*count_sum, task_number * 1024);
	ASSERT_TRUE("Pass");
}



//template<typename T>
//std::optional<T> GetJsonField(Json::Value& json_value, std::string_view field)
//{
//	std::optional<T> opt;
//	if (!json_value[field.data()].is<T>())
//		opt = json_value[field.data()].as<T>();
//	return opt;
//}

TEST(tests, ParseJsonTest)
{
	std::string json_string = "{\"test\":\"pass\"}";
	Json::Value json_value;
	ASSERT_TRUE(ParseJsonStringToJsonValue(json_string, json_value));
	ASSERT_EQ(json_value["test"].asString(), "pass");
	auto opt = GetJsonField<std::string>(json_value, "test");
	ASSERT_TRUE(opt.has_value());
	ASSERT_EQ(opt.value(), "pass");
}

