# cpp_async_file_hashing
Asynchronous MD5 / SHA1 file hashing in C++ using futures and wincrypt. 
Project configured for x64 use, shouldn't have much trouble running in x86. Keep in mind that this system cuts down times, but it takes as long to finish as the biggest file in the directory takes to finish hashing, give or take. PRs are welcomed.
