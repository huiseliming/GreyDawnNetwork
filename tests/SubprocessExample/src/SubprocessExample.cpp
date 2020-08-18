#include <utility/Utility.h>
#include <filesystem>
using namespace GreyDawn;


int main(int argc, char* argv[])
{
	std::vector<std::string> command_line_args;
	for (size_t i = 1.; i < argc; i++)
	{
		command_line_args.push_back(argv[i]);
	}
	for (size_t i = 0; i < command_line_args.size(); i++)
	{
		GD_LOG_ERROR("{}",command_line_args[i]);
	}
	
	Subprocess subprocess;
	if (subprocess.ContactParent(command_line_args))
	{
		subprocess.PipeMessage();
	}
	else
		GD_LOG_ERROR("Can't ContactParent");
	return 0;
}








