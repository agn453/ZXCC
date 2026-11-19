/* zxcc CP/M emulator program that avoids zxcc argument translation */

#include "zxcc.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Allocates a new string with '-' prepended and all characters uppercased */
static char *dup_dash_upper(const char *src) {
    size_t len = strlen(src);
    char *out = malloc(len + 2); /* 1 for '-', 1 for '\0' */
    if (!out) {
        perror("zxcpm: malloc");
        exit(EXIT_FAILURE);
    }

    out[0] = '-';
    for (size_t i = 0; i < len; i++) {
        out[i + 1] = (char)toupper((unsigned char)src[i]);
    }
    out[len + 1] = '\0';
    return out;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: zxcpm COMMAND [ ARG ... ]\n");
        return EXIT_FAILURE;
    }

    /* Allocate array for: ["zxcc", argv[1], "-ARG2", ..., NULL] */
    char **new_argv = malloc((size_t)(argc + 1) * sizeof(char *));
    if (!new_argv) {
        perror("zxcpm: malloc");
        return EXIT_FAILURE;
    }

    new_argv[0] = "zxcc";
    new_argv[1] = argv[1]; /* Kept verbatim for Linux filesystem lookup */

    for (int n = 2; n < argc; n++) {
        new_argv[n] = dup_dash_upper(argv[n]);
    }
    new_argv[argc] = NULL;

    /* Replace current process with zxcc (automatically preserves exit status) */
    execvp("zxcc", new_argv);

    /* execvp only returns if launching zxcc fails */
    perror("zxcpm: execvp zxcc");
    for (int n = 2; n < argc; n++) {
        free(new_argv[n]);
    }
    free(new_argv);
    return EXIT_FAILURE;
}
