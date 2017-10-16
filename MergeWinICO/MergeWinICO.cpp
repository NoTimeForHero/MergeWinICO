// MergeWinICO.cpp: определ€ет точку входа дл€ консольного приложени€.
//

#include "stdafx.h"
#include "structure.h"

WINICO_HEADER readHeader(FILE*);
int do_action(int, char*[]);

int main(int argc, char *argv[])
{
	if (argc < 3) {
		printf("%s\n",VERSION);
		printf("Usage: %s.exe target.ico input1.ico [input2.ico, ...]\n", EXE_NAME);
		return 1;
	}

	printf("Target file: %s\n", argv[1]);
	if (DEBUG) printf("\n");

	int status = do_action(argc, argv);
	return status;
}

int do_action(int argc, char *argv[]) {
	unsigned short entry_count = 0;
	WINICO_ENTRY *entries = (WINICO_ENTRY*) malloc(0);
	byte *data = (byte*)malloc(0);
	int len_data = 0;

	for (int i = 2; i < argc; i++) {
		FILE *curfile = fopen(argv[i], "rb");

		if (!curfile) {
			printf("File not found: %s", argv[i]);
			return 2;
		}
		printf("Processing file: %s\n", argv[i]);

		WINICO_HEADER header;
		fread(&header, sizeof header, 1, curfile);

		if (DEBUG) printf("There are %d icons in file '%s'\n", header.count, argv[i]);
		entry_count = entry_count + header.count;
		entries = (WINICO_ENTRY*)realloc(entries, LEN_ENTRY * entry_count);

		WINICO_ENTRY *curfile_entries = (WINICO_ENTRY*) malloc( LEN_ENTRY * header.count);
		fread(curfile_entries, LEN_ENTRY, header.count, curfile);
		memcpy(&entries[entry_count - header.count], curfile_entries, LEN_ENTRY * header.count);

		for (int hi = 0; hi < header.count; hi++) {
			int size   = curfile_entries[hi].size;
			int offset = curfile_entries[hi].offset;
			size_t readed = 0;
			byte *buffer = (byte*) malloc(sizeof(byte)*(size));
				
			if (DEBUG) printf("Icon %d params: size=%d,offset=%d\n", hi, size,offset);
			fseek(curfile, offset, SEEK_SET);
			readed = fread(buffer, sizeof(byte), size, curfile);
			if (readed != size) {
				printf("Error while reading icon %d from file '%s'\n",hi,argv[i]);
				return 16;
			}

			data = (byte*)realloc(data, sizeof(byte) * (len_data + size));
			memcpy(&data[len_data], buffer, size);
			len_data = len_data + size;
			free(buffer);
		}
		if (DEBUG) printf("Done.\n\n");

		free(curfile_entries);
		fclose(curfile);
	}

	// ѕозици€ начала данных в финальном ICO
	int offset_data = LEN_HEADER + LEN_ENTRY * entry_count;

	entries[0].offset = offset_data;
	for (int i = 1; i < entry_count; i++) {
		entries[i].offset = entries[i - 1].offset + entries[i - 1].size;
	}

	WINICO_HEADER header_output = { 0,1,entry_count };
	FILE *outfile = fopen(argv[1], "wb");
	if (!outfile) {
		printf("Can't write to file: %s",argv[1]);
		free(entries);
		return 8;
	}

	fwrite(&header_output, LEN_HEADER, 1, outfile);
	fwrite(entries, LEN_ENTRY, entry_count, outfile);
	fwrite(data, len_data, 1, outfile);

	free(data);
	free(entries);

	fclose(outfile);

	printf("Success!\n");
	return 0;
}