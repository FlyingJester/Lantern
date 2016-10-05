#include "cpio.h"

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

bool Lantern_ArchiveFileMatchesName(const struct Lantern_Archive *data, unsigned at, const char *name){
	const unsigned in_len = strlen(name),
		arc_len = Lantern_ArchiveFileNameLength(data, at);
	
	const char *const arc_name = ((char *)data->data) + at + LANTERN_ARCHIVE_FILE_HEADER_LENGTH;
	
	if(in_len != arc_len)
		return false;
	else
		return memcmp(arc_name, name, in_len) == 0;
}

static unsigned decode_octal(const char *str, unsigned size){
	unsigned len = 0, i = 0;

	while(i < size){
		len <<= 3;
		len += str[i++] - '0';
	}
	
	return len;
}

unsigned Lantern_ArchiveFileNameLength(const struct Lantern_Archive *data, unsigned at){
	const char *name_start = ((const char *)data->data) + at + 59;
	return decode_octal(name_start, 6);
}

unsigned Lantern_ArchiveFileLength(const struct Lantern_Archive *data, unsigned at){
	const char *size_start = ((const char *)data->data) + at + 65;
	return decode_octal(size_start, 11);
}

void Lantern_ArchiveSearch(const struct Lantern_Archive *LANTERN_RESTRICT data,
	struct Lantern_ArchiveEntry *LANTERN_RESTRICT in_out, unsigned num_entries){
	/* Keep track of how many have been found already. This is just an optimization. */
	unsigned num_found = 0, at = 0, clipped_size = num_entries, i;
	/* Mark all entries as not-found first. */
	for(i = 0; i < num_entries; i++){
		in_out[i].start = in_out[i].length = LANTERN_ARCHIVE_INVALID_SIZE;
	}
	
	while(at < data->size && num_found != num_entries &&
		!Lantern_ArchiveFileMatchesName(data, at, "TRAILER!!")){
		
		for(i = 0; i < clipped_size; i++){
			if(in_out[i].start != LANTERN_ARCHIVE_INVALID_SIZE && 
				in_out[i].length != LANTERN_ARCHIVE_INVALID_SIZE)
				continue;
			
			if(Lantern_ArchiveFileMatchesName(data, at, in_out[i].name)){
				
				in_out[i].start = at + LANTERN_ARCHIVE_FILE_HEADER_LENGTH +
					Lantern_ArchiveFileNameLength(data, at) + 1;
				in_out[i].length = Lantern_ArchiveFileLength(data, at);
				
				at = in_out[i].start + in_out[i].length;
				
				/* Trim the search array where possible. */
				if(i == 0){
					if(--clipped_size == 0)
						return;
					in_out++;
				}
				else if(i+1 == clipped_size){
					if(--clipped_size == 0)
						return;
				}
			}
		}
	}
}

bool Lantern_ArchiveEnumerate(const struct Lantern_Archive *LANTERN_RESTRICT data,
	lantern_enum_callback callback, void *LANTERN_RESTRICT arg){
	
	unsigned at;
	bool success = true;
	
	for(at = 0; at < data->size && !Lantern_ArchiveFileMatchesName(data, at, "TRAILER!!");){
		const unsigned name_size = Lantern_ArchiveFileNameLength(data, at);
		const unsigned data_size = Lantern_ArchiveFileLength(data, at);
		struct Lantern_ArchiveEntry file;
		
		if(memcmp(((char *)data->data) + at, "070707", 6) != 0){
			fputs("Invalid cpio signature\n", stderr);
			fflush(stderr);
			return false;
		}
		
		at += LANTERN_ARCHIVE_FILE_HEADER_LENGTH;
		
		assert(name_size < 0xFF);
		if(name_size >= 0xFF)
			return false;
		
		memcpy(file.name, ((char *)data->data) + at, name_size);
		file.name[name_size ] = 0;
		
		fputs("[cpio] Enumerationg file ", stdout);
		puts(file.name);
		printf("Size %i, Namesize %i\n", data_size, name_size);
		fflush(stdout);
		
		at += name_size;
		
		file.start = at;
		file.length = data_size;
		
		at += data_size;
		
		success = callback(arg, data, &file) && success;
	}
	return success;
}