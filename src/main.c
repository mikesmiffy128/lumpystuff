#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <unistd.h>
#include <inttypes.h>
#include "bsp.h"
#include "util.h"
#include "uchar.h"

__declspec(noreturn) static void printUsage(uchar *cmdName) {
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

static void warnIfNotPiped(void) {
	if (isatty(fileno(stdout))) {
		fuputs(U("Remember to pipe the output somewhere!\n"), stderr);
		exit(1); // TODO haven't decided whether to still continue
	}
}

static void dumpLumpData(FILE *f, size_t length) {
	unsigned char buf[4096];
	size_t remaining = length;
	size_t nread = 0;
	do {
		nread = fread(buf, 1, sizeof(buf), f);
		remaining -= nread;

		if (fwrite(buf, 1, nread, stdout) != nread) {
			fuputs(U("\nCouldn't write to stdout!\n"), stderr);
			fclose(f);
			exit(1);
		}
	} while (nread > 0 && remaining > 0);

	fclose(f);
	if (remaining > 0) {
		fuputs(U("Unexpected end of file.\n"), stderr);
		exit(1);
	}
	fuprintf(stderr, U("Dumped %") U(PRIdPTR) U(" bytes.\n"), length);
}

int UFUNC(main)(int argc, uchar *argv[]) {
#ifdef _WIN32
	setmode(fileno(stdout), _O_BINARY);
	setmode(fileno(stderr), _O_U8TEXT);
#endif

	if (argc == 1) {
		fuputs(U("-- Michael's lump file tool v0.1 --\n")
				U("Some quick haxx for tweaking map entity data\n"),
				stderr);
		printUsage(argv[0]);
	}

	if (!ustrcmp(argv[1], U("extract_bsp"))) {
		if (argc != 4) {
			printUsage(argv[0]);
		}
		warnIfNotPiped();

		uchar *endptr;
		long lumpNum = ustrtol(argv[3], &endptr, 10);
		if (lumpNum == 0 && endptr == argv[3]) {
			fuputs(U("Lump number is invalid!"), stderr);
			exit(1);
		}
		if (lumpNum > 60 || lumpNum < 0) {
			fuputs(U("Lump number out of range! (0-60)\n"), stderr);
			exit(1);
		}

		FILE *f = ufopen(argv[2], U("rb"));
		if (!f) {
			fuprintf(stderr, U("Could not open file \"%s\"!\n"), argv[2]);
			exit(1);
		}

		struct BSPHeader header;
		if (!fread(&header, sizeof(header), 1, f)) {
			fuputs(U("Could not read BSP header!\n"), stderr);
			exit(1);
		}
		if (header.ident != VBSPHEADER || header.version < MINBSPVERSION ||
				header.version > BSPVERSION) {
			fuputs(U("Unsupported BSP header!\n"), stderr);
			exit(1);
		}

		if (fseek(f, header.lumps[lumpNum].fileofs, 0)) {
			fuputs(U("Could not seek through BSP file!\n"), stderr);
			exit(1);
		}

		dumpLumpData(f, header.lumps[lumpNum].filelen);
	}
	else if (!ustrcmp(argv[1], U("extract_lmp"))) {
		if (argc != 3) {
			printUsage(argv[0]);
		}
		warnIfNotPiped();

		FILE *f = ufopen(argv[2], U("rb"));
		if (!f) {
			fuprintf(stderr, U("Could not open file \"%s\"!\n"), argv[2]);
			exit(1);
		}

		struct LMPHeader header;
		if (!fread(&header, sizeof(header), 1, f)) {
			fuputs(U("Could not read LMP header!\n"), stderr);
			exit(1);
		}

		if (fseek(f, header.lumpOffset, 0)) {
			fuputs(U("Could not seek through LMP file!\n"), stderr);
			exit(1);
		}

		dumpLumpData(f, header.lumpLength);
	}
	else if (!ustrcmp(argv[1], U("create_lump"))) {
		fuputs(U("Not yet implemented! ((\n"), stderr);
		exit(1);
	}
	else if (!ustrcmp(argv[1], U("lump_params_bsp"))) {
		fuputs(U("Not yet implemented! ((\n"), stderr);
		exit(1);
	}
	else if (!ustrcmp(argv[1], U("lump_params_lmp"))) {
		fuputs(U("Not yet implemented! ((\n"), stderr);
		exit(1);
	}
	else {
		printUsage(argv[0]);
	}

	return 0;
}
