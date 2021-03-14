#pragma once

namespace dir_queue
{
	// the actual queue of files and open handles
	std::vector<file_struct_t> queue;

	// store the futures so that they run asynchronously
	std::vector<std::future<void>> hashing_futures;
	
	// store hashes and file names, prepare for outputting
	std::mutex string_data_mutex;
	std::vector<std::string> data;

	// keep an eye on how many files we've hashed correctly so far
	int hashed_files = 0;

	// queue a directory containing files for hashing
	void queue_up(const std::string& directory = ".\\")
	{
		utils::file_enumeration_callback([&](HANDLE handle, std::string path)
			{
				if (!std::filesystem::is_directory(path))
				{
					queue.push_back({ handle, path });
				}
			});
	}

	// function the futures will run
	void hash_function(file_struct_t file)
	{
		const auto position = file.path.find_last_of('\\');
		const auto file_name = file.path.substr(position + 1);
		const auto md5_hash = utils::get_file_hash_md5(file.handle);

		string_data_mutex.lock();

		data.push_back(file_name + " md5: " + md5_hash);

		hashed_files++;

		string_data_mutex.unlock();
	}

	// prepare and launch all the futures
	void launch_hashing_futures()
	{
		for (const auto file : queue)
		{
			hashing_futures.push_back(std::async(std::launch::async, hash_function, file));
		}
	}

	// clean up
	void cleanup()
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

	// wait until all the futures are done
	void wait_for_hashing()
	{
		using namespace std::chrono_literals;

		while (true)
		{
			string_data_mutex.lock();

			auto are_files_not_done_hashing = (hashed_files != hashing_futures.size());

			string_data_mutex.unlock();

			if (are_files_not_done_hashing)
			{
				std::this_thread::sleep_for(0ms);
			}
			else
			{
				break;
			}
		}
	}

	// print hashes and files
	void print_results()
	{
		for (const auto& string : data)
		{
			std::cout << string << std::endl;
		}
	}

};