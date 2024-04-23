# File Search and Operation Utility
This is a command-line utility written in C for searching, copying, moving, and archiving files based on various criteria such as filename, extension, and directory structure. The utility utilizes the nftw function from the ftw.h library to traverse directories recursively.

# Features
- File Search: Search for a specific file by its filename within a specified directory.
- File Copying and Moving: Copy or move a file to a specified destination directory.
- Archiving: Archive files with a specific extension into a tar archive.

# Usage
Search for a File

./file_util <root_directory> <filename>
Searches for a file named <filename> within the specified <root_directory>.

Copy or Move a File

./file_util <root_directory> <storage_directory> <-cp | -mv> <filename>
Copies or moves a file named <filename> from <root_directory> to <storage_directory>.

Archive Files by Extension

./file_util <root_directory> <storage_directory> <extension>
Archives files with the specified <extension> found within <root_directory> into a tar archive stored in <storage_directory>.

# Usage Examples
Search for a File

./file_util /path/to/directory myfile.txt
Copy a File

./file_util /path/to/source /path/to/destination -cp myfile.txt
Move a File

./file_util /path/to/source /path/to/destination -mv myfile.txt
Archive Files

./file_util /path/to/source /path/to/destination .txt
