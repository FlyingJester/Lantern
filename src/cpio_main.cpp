#include <vector>
#include <string>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

struct cpio_entry{
	std::string name;
	unsigned size;
	unsigned modtime;
	
	cpio_entry(const char *n, unsigned short s, unsigned short t)
	  : name(n), size(s), modtime(t) {}
	cpio_entry(const std::string &n, unsigned short s, unsigned short t)
	  : name(n), size(s), modtime(t) {}
};

#if defined _WIN32 || defined __CYGWIN__

#include <Windows.h>

static bool find_files(const char *dir, std::vector<cpio_entry>& in_out){
	WIN32_FIND_DATA data;
	HANDLE find;
	
	const std::string path = std::string(dir) + "\\*";
	
	if((find = FindFirstFile(path.c_str(), &data)) == INVALID_HANDLE_VALUE)
		return false;
	
	do{
		
		if(data.nFileSizeHigh != 0)
			continue;
		
		if(data.dwFileAttributes == FILE_ATTRIBUTE_NORMAL ||
			(data.dwFileAttributes & (FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_TEMPORARY | FILE_ATTRIBUTE_ARCHIVE)) == data.dwFileAttributes){
			const unsigned short high_time = data.ftLastWriteTime.dwHighDateTime / 10000000,
				low_time = data.ftLastWriteTime.dwLowDateTime / 10000000;
			const unsigned micro_time = (low_time) | (high_time << 16);
			in_out.push_back(cpio_entry(std::string(dir) + '/' + data.cFileName, data.nFileSizeLow, micro_time));
		}
		
	}while(FindNextFile(find, &data) != 0);
	
	return true;
}

#else
	
#endif

static void write_octal(unsigned size, unsigned value, FILE *const file){
	char buffer[11];
	unsigned buflen = 0;
	
	while(buflen < size){
		buffer[buflen++] = '0' + (value % 8);
		value >>= 3;
	}
	
	while(buflen--){
		fputc(buffer[buflen], file);
	}
}

static void write_header(const cpio_entry& entry, FILE *const file){
	
	fputs("070707", file); // magic
	
	fputs("000000", file); // dev
	fputs("000000", file); // ino
	fputs("000000", file); // mode
	fputs("000000", file); // uid
	fputs("000000", file); // gid
	fputs("000000", file); // nlink
	fputs("000000", file); // rdev

	// Mod Time
	write_octal(11, entry.modtime, file);
	// Name Size
	write_octal(6, entry.name.length(), file);
	// File Size
	write_octal(11, entry.size, file);
	
	fputs(entry.name.c_str(), file);
}

int main(int argc, char *argv[]){
	char buffer[0x1000];
	std::vector<cpio_entry> entries;
	
	if(argc < 2){
		puts("Creating a cpio archive called 'out.cpio' of all files in the current directory");
	}
	else if(strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0){
		puts("LanternCPIO - Copyright 2016 Martin McDonough");
		puts("Usage:");
		puts("\t lanterncpio DIRECTORY OUTPUTFILE");
		return EXIT_SUCCESS;
	}
	
	if(argc > 1)
		find_files(argv[1], entries);
	else
		find_files(".", entries);
	
	const char *outputname = "out.cpio";
	if(argc > 2)
		outputname = argv[2];
	
	FILE *const file = fopen(outputname, "wb");
	
	for(std::vector<cpio_entry>::const_iterator i = entries.begin(); i != entries.end(); i++){
		if(i->name == outputname || i->name == argv[0])
			continue;
		
		write_header(*i, file);
		fflush(file);
		
		FILE *const input = fopen(i->name.c_str(), "rb");
		
		unsigned readin = 0;
		while((!feof(input)) && (readin = fread(buffer, 1, sizeof(buffer), input)) != 0){
			fwrite(buffer, 1, readin, file);
		}
		
		fclose(input);
		
		fflush(file);
	}
	
	write_header(cpio_entry("TRAILER!!", 0, 0), file);
	
	fflush(file);
	fclose(file);
}
