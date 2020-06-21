#include "common.hpp"

int main(void)
{
	dir_queue::queue_up();

	std::cout << "[+] starting the timer" << std::endl;

	const auto timer = std::chrono::system_clock::now();

	dir_queue::launch_hashing_futures();

	dir_queue::print_hashes();

	std::cout << "[+] launching all the futures and hashing took " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - timer).count() << "ms" << std::endl;

	std::cin.get();

	return 0;
}
