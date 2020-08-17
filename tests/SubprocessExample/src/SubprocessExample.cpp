#include <utility/Utility.h>
#include <filesystem>
using namespace GreyDawn;


int main(int argc, char* argv[])
{
	std::vector<std::string> command_line_args;
	for (size_t i = 0; i < argc; i++)
	{
		command_line_args.push_back(argv[i+1]);
	}
	Subprocess subprocess;
	if (subprocess.ContactParent(command_line_args))
	{
		subprocess.PipeMessageLoop();
	}
	else
		GD_LOG_ERROR("Can't ContactParent");
	return 0;
}








