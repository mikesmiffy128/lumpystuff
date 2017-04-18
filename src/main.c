#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdnoreturn.h>
#include "bsp.h"
#include "util.h"
#include "unicode.h"
#include "endian.h"

static noreturn void printUsage(uchar *cmdName) {
	// TODO: write out the example usage thing
	fuprintf(stderr, U("\
Usage: \n\
    %s extract_bsp [input.bsp] [lump_id] > output.dat\n\
Or: %s extract_lmp [input.lmp] > output.dat\n\
Or: %s create_lump [output.lmp] [lump_id] [lump_ver] [map_rev] < input.dat\n\
Or: %s lump_params_bsp [input.bsp] [lump_id]\n\
Or: %s lump_params_lmp [input.lmp]\n\
\n\
- where:\n\
  * lump_id is a numeric identifier (for a list of valid IDs, see\n\
    https://developer.valvesoftware.com/wiki/Source_BSP_File_Format#Lump_types)\n\
  * lump_ver and map_rev are lump headers which should match those in the map\n\
    (these can be found using lump_params_*).\n\
\n\
The lump_params commands return space-separated values for lump_ver and\n\
map_rev. A batch file or shell script can pass these to create_lump.\n\
"),	cmdName, cmdName, cmdName, cmdName, cmdName);
	exit(1);
}

static void checkStdoutPipe(void) {
	if (isatty(fileno(stdout))) {
		fuputs(U("No output pipe was provided!\n"), stderr);
		exit(1);
	}
}

static void checkStdinPipe(void) {
	if (isatty(fileno(stdin))) {
		fuputs(U("No input pipe was provided!\n"), stderr);
		exit(1);
	}
}

static FILE *tryReadFile(uchar *name) {
	FILE *f = ufopen(name, U("rb"));
	if (!f) {
		fuprintf(stderr, U("Could not open file \"%s\" for reading!\n"), name);
		exit(1);
	}
	return f;
}

static FILE *tryWriteFile(uchar *name) {
	FILE *f = ufopen(name, U("wb"));
	if (!f) {
		fuprintf(stderr, U("Could not open file \"%s\" for writing!\n"), name);
		exit(1);
	}
	return f;
}

static long parseLumpNum(uchar *s) {
	uchar *endptr;
	long lumpNum = ustrtol(s, &endptr, 10);
	if (lumpNum == 0 && endptr == s) {
		fuputs(U("Lump ID is not a valid number!\n"), stderr);
		exit(1);
	}
	if (lumpNum > HEADER_LUMPS - 1 || lumpNum < 0) {
		fuputs(U("Lump ID is out of range! (0-60)\n"), stderr);
		exit(1);
	}
	return lumpNum;
}

static void readBSPHeader(FILE *f, struct BSPHeader *header) {
	if (!fread(header, sizeof(struct BSPHeader), 1, f)) {
		fuputs(U("Could not read BSP header!\n"), stderr);
		exit(1);
	}
	if (LESwap32(header->ident) != VBSPHEADER || LESwap32(header->version) <
			MINBSPVERSION || LESwap32(header->version) > BSPVERSION) {
		fuputs(U("Unsupported BSP header!\n"), stderr);
		exit(1);
	}
}

static void readLMPHeader(FILE *f, struct LMPHeader *header) {
	if (!fread(header, sizeof(struct LMPHeader), 1, f)) {
		fuputs(U("Could not read LMP header!\n"), stderr);
		exit(1);
	}
}

static void dumpLumpData(FILE *f, size_t length) {
	unsigned char buf[4096];
	size_t nread = 0;
	size_t nleft = length;
	do {
		nleft -= nread = fread(buf, 1,
				nleft < sizeof(buf) ? nleft : sizeof buf, f);
		if (fwrite(buf, 1, nread, stdout) != nread) {
			fuputs(U("Couldn't write to stdout!\n"), stderr);
			fclose(f);
			exit(1);
		}
	} while (nread && length);
	fclose(f);

	if (nleft) {
		fuputs(U("Unexpected end of file.\n"), stderr);
		exit(1);
	}
	fuprintf(stderr, U("Dumped %") U(PRIdPTR) U(" bytes.\n"), length);
}

int UFUNC(main)(int argc, uchar *argv[]) {
#ifdef _WIN32
	setmode(fileno(stdin), _O_BINARY);
	setmode(fileno(stdout), _O_BINARY);
	setmode(fileno(stderr), _O_U8TEXT);
#endif

	if (argc == 1) {
		fuputs(U("-- Michael's lump file tool v0.1 --\n"), stderr);
		printUsage(argv[0]);
	}

	if (!ustrcmp(argv[1], U("extract_bsp"))) {
		if (argc != 4) {
			printUsage(argv[0]);
		}
		checkStdoutPipe();


		long lumpNum = parseLumpNum(argv[3]);
		FILE *f = tryReadFile(argv[2]);

		struct BSPHeader header;
		readBSPHeader(f, &header);

		if (fseek(f, LESwap32(header.lumps[lumpNum].fileofs), 0)) {
			fuputs(U("Could not seek through BSP file!\n"), stderr);
			exit(1);
		}

		dumpLumpData(f, LESwap32(header.lumps[lumpNum].filelen));
	}
	else if (!ustrcmp(argv[1], U("extract_lmp"))) {
		if (argc != 3) {
			printUsage(argv[0]);
		}
		checkStdoutPipe();

		FILE *f = tryReadFile(argv[2]);

		struct LMPHeader header;
		readLMPHeader(f, &header);

		if (fseek(f, LESwap32(header.lumpOffset), 0)) {
			fuputs(U("Could not seek through LMP file!\n"), stderr);
			exit(1);
		}

		dumpLumpData(f, LESwap32(header.lumpLength));
	}
	else if (!ustrcmp(argv[1], U("create_lump"))) {
		if (argc != 6) {
			printUsage(argv[0]);

		}
		checkStdinPipe();

		long lumpId = parseLumpNum(argv[3]);
		uchar *endptr;
		// TODO: This is a bit messy, maybe move into a function?
		long lumpVer = ustrtol(argv[4], &endptr, 10);
		if (lumpVer == 0 && endptr == argv[4]) {
			fuputs(U("Lump version is not a valid number!\n"), stderr);
			exit(1);
		}
		if (lumpVer > INT32_MAX) {
			fuputs(U("Lump version is too high!\n"), stderr);
			exit(1);
		}
		if (lumpVer < INT32_MIN) {
			fuputs(U("Lump version is too low!\n"), stderr);
			exit(1);
		}
		long mapRev = ustrtol(argv[5], &endptr, 10);
		if (lumpVer == 0 && endptr == argv[5]) {
			fuputs(U("Map revision is not a valid number!\n"), stderr);
			exit(1);
		}
		if (lumpVer > INT32_MAX) {
			fuputs(U("Map revision is too high!\n"), stderr);
			exit(1);
		}
		if (lumpVer < INT32_MIN) {
			fuputs(U("Map revision is too low!\n"), stderr);
			exit(1);
		}

		FILE *f = tryWriteFile(argv[2]);
		// We have to go back and write the header once we know the length.
		if (fseek(f, 20, 0)) {
			fuputs(U("Could not seek into output file!\n"), stderr);
		}

		unsigned char buf[4096];
		size_t total = 0;
		size_t nread = 0;
		for (;;) {
			total += nread = fread(buf, 1, sizeof(buf), stdin);

			if (!nread) {
				break;
			}

			if (fwrite(buf, 1, nread, f) != nread) {
				fuputs(U("Couldn't write to file!\n"), stderr);
				fclose(f);
				exit(1);
			}

			// TODO: Not sure if there's a sensible lower limit here!?
			if (total > INT32_MAX) {
				fuputs(U("Input data is far too large, aborting!\n"), stderr);
				fclose(f);
				exit(1);
			}
		}

		if (ferror(stdin)) {
			fuputs(U("Couldn't read from stdin!\n"), stderr);
			fclose(f);
			exit(1);
		}
		fuprintf(stderr, U("Saved %") U(PRIdPTR) U(" bytes.\n"), total);

		if (fseek(f, 0, 0)) {
			fuputs(U("Could not seek to start of output file!\n"), stderr);
		}
		struct LMPHeader header = {
			.lumpOffset	 = LESwap32(sizeof(header)),
			.lumpID		 = LESwap32(lumpId),
			.lumpVersion = LESwap32(lumpVer),
			.lumpLength	 = LESwap32((int32_t)total),
			.mapRevision = LESwap32(mapRev)
		};
		if (fwrite(&header, sizeof(header), 1, f)) {
			fuputs(U("Successfully wrote header.\n"), stderr);
			fclose(f);
		}
		else {
			fuputs(U("Failed to write header!\n"), stderr);
			fclose(f);
			exit(1);
		}
	}
	else if (!ustrcmp(argv[1], U("lump_params_bsp"))) {
		if (argc != 4) {
			printUsage(argv[0]);
		}

		long i = parseLumpNum(argv[3]);
		FILE *f = tryReadFile(argv[2]);

		struct BSPHeader header;
		readBSPHeader(f, &header);
		printf("%"PRId32 " %"PRId32, LESwap32(header.lumps[i].version),
				LESwap32(header.mapRevision));
	}
	else if (!ustrcmp(argv[1], U("lump_params_lmp"))) {
		if (argc != 3) {
			printUsage(argv[0]);
		}

		FILE *f = tryReadFile(argv[2]);

		struct LMPHeader header;
		readLMPHeader(f, &header);
		printf("%"PRId32" %"PRId32, LESwap32(header.lumpVersion),
				LESwap32(header.mapRevision));
	}
	else {
		printUsage(argv[0]);
	}

	return 0;
}
