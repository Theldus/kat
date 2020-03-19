/*
 * MIT License
 *
 * Copyright (c) 2020 Davidson Francis <davidsondfgl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * This is Kat (cat in Japanese): A very simple syntax highlighter made in C
 * for C language.
 *
 * Despite its simplicity, it has some interesting features:
 * - Can work as a standalone program or as a library
 *   with only one major function `highlight_line()`.
 *
 * - Fast! Kat manages to catch up with the classic `cat` or even surpass it
 *   for certain types of source code. Also, it is *much* faster than
 *   similar programs, such as bat (https://github.com/sharkdp/bat).
 *
 * - Bloat free!. Kat has no additional dependencies and the only thing you
 *   need is the standard libc.
 *
 * - Not so bad/decent syntax highlight. Although with some limitations,
 *   Kat manages to deliver a visually pleasing, and similar or superior
 *   output to that found in programs that use GtkSourceView, such as
 *   Mousepad, GEdit.
 *
 * - Themes. Kat makes available a simple configuration scheme file that allows
 *   users to define your own themes.
 */

#define _POSIX_C_SOURCE 200809L
#include <unistd.h>
#include "highlight.h"

#define OPTPARSE_IMPLEMENTATION
#include "optparse.h"

/* External definitions. */
INLINE extern char* add_char_to_hl(char *, char);
INLINE extern char* add_str_to_hl(char *, const char *, size_t);

/**
 * Show usage.
 *
 * @param code Return code.
 */
void usage(int code)
{
	printf("Usage: highlight [file-name] [options]\n");
	printf("Options:\n");
	printf("-h --help          Show options available\n");
	printf("-t --theme [file]  Set a theme file\n");
	exit(code);
}

/**
 * Test
 */
int main(int argc, char **argv)
{
	FILE *fp;                /* File pointer.                     */
	char *line;              /* Current line.                     */
	size_t len;              /* Allocated size.                   */
	ssize_t read;            /* Bytes read.                       */
	char *hl;                /* Currently highlighted line.       */
	char *buff;              /* Buffer to be dumped.              */
	int option;              /* Current option.                   */
	struct optparse options; /* Optparse options.                 */
	char *theme_file = NULL; /* Theme file.                       */
	char *targ_file = NULL;  /* Target file, if any.              */
	struct highlighted_line *high; /* Highlighted line structure. */

	fp = stdin;

	if (argc > 1)
	{
		/* Current arguments list. */
		struct optparse_long longopts[] = {
			{"help",   'h',   OPTPARSE_NONE},
			{"theme",  't',   OPTPARSE_REQUIRED},
			{0,0,0}
		};

		optparse_init(&options, argv);
		while ((option = optparse_long(&options, longopts, NULL)) != -1)
		{
			switch (option)
			{
				case 'h':
					usage(0);
					break;
				case 't':
					theme_file = options.optarg;
					break;
				case '?':
					fprintf(stderr, "%s: %s\n\n", argv[0], options.errmsg);
					usage(1);
					break;
			}
		}

		/* Try to read the source. */
		targ_file = optparse_arg(&options);
		if (targ_file != NULL && strcmp(targ_file, "-") != 0)
		{
			fp = fopen(targ_file, "r");
			if (fp == NULL)
			{
				fprintf(stderr, "%s: cannot open the file %s, is it really exists?\n",
					argv[0], argv[1]);
				return (1);
			}
		}
	}

	line = NULL;
	buff = NULL;
	len  = 0;

	if (highlight_init(theme_file) < 0)
	{
		if (fp != stdin)
			fclose(fp);
		return (-1);
	}

	hl = highlight_alloc_line();
	buff = highlight_alloc_line();

	/* Read the entire file. */
	while ((read = getline(&line, &len, fp)) != -1)
	{
		/* Highlight. */
		hl = highlight_line(line, hl, read);
		high = ((struct highlighted_line *)hl - 1);

		/* Add our string into our buffer. */
		buff = add_str_to_hl(buff, hl, high->idx - 1);
	}

	buff = add_char_to_hl(buff, '\0');
	high = ((struct highlighted_line *)buff - 1);
	write(fileno(stdout), buff, high->idx - 1);

	highlight_free(hl);
	highlight_free(buff);
	free(line);

	if (fp != stdin)
		fclose(fp);

	highlight_finish();
}
