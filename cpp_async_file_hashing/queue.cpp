#include "common.hpp"

void dir_queue::queue_up(const std::string& directory)
{
	utils::file_enumeration_callback([&](HANDLE handle, std::string path) 
	{
		if (!std::filesystem::is_directory(path))
		{
			queue.push_back({ handle, path });
		}
	});
}

void dir_queue::hash_function(file_struct_t file)
{
	const auto position = file.path.find_last_of('\\');
	const auto file_name = file.path.substr(position + 1);
	const auto hash = utils::get_file_hash_md5(file.handle);

	data.push_back(file_name + " : " + hash);

	hashed_files++;
}

void dir_queue::launch_hashing_futures()
{
	for (const auto file : queue)
	{
		hashing_futures.push_back(std::async(std::launch::async, hash_function, file));
	}
}

void dir_queue::cleanup()
{	
	for (const auto& file : queue) 
	{
		CloseHandle(file.handle);
	}

	queue.clear();
	hashing_futures.clear();
	data.clear();
	
	hashed_files = 0;
}

void dir_queue::print_hashes()
{
	// wait until it's done
	while (hashed_files != hashing_futures.size()) std::this_thread::sleep_for(0ms);

	for (const auto& string : data)
	{
		std::cout << string << std::endl;
	}
}
