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

#include <stdio.h>
#include "highlight.h"

/**
 * Abort execution with the errorcode @p errcode
 * and print a message @p msg on the screen.
 *
 * @param errcode Return error code.
 * @param msg Message to be printed.
 */
static inline void quit(int errcode, const char *msg)
{
	puts(msg);
	exit(errcode);
}

/**
 * Main routine.
 */
int main()
{
	char *line;

	if (highlight_init(NULL) < 0)
		quit(-1, "Unable to initialize the highlight!\n");

	/*
	 * The main operation resides in `highlight_line` routine:
	 * char *highlight_line(const char *line, char *hl);
	 *
	 * line:
	 *   NULL terminated string representing a single line to be
	 *   highlighted
	 *
	 * hl:
	 *   hl is an (optional) expandable string that Kat uses
	 *   internally. The user can pre-allocate one with:
	 *      char* highlight_alloc_line(void);
	 *   or pass NULL as parameter.
	 *
	 * str_size:
	 *   str_size is an (optional if eq 0) parameter that informs
	 *   in beforehand the string size; it is useful for constant
	 *   strings that are used frequently (avoids unnecessaries strlen
	 *   calls).
	 *
	 * In case of hl == NULL, Kat will allocate a new expandable line
	 * and returns it, if not NULL, Kat will return the previous buffer
	 * with the new highlighted line.
	 *
	 * Also note that subsequent calls to highlight_line will overwrite
	 * the contents of the buffer hl.
	 *
	 * ---
	 * Although highlight_line parses the source per line, it does not mean
	 * that Kat is unable to handle things that expands across lines, such
	 * as multiline comments. Kat keeps a global state to handle these
	 * cases.
	 */
	line = highlight_line("#include <stdio.h>", NULL, 0);
	puts(line);
	line = highlight_line("int main(int argc, char **argv){ /* Comment. */", line, 0);
	puts(line);
	line = highlight_line("    printf(\"Hello World \%s\\n\", argv[1]);", line, 0);
	puts(line);
	line = highlight_line("}", line, 0);
	puts(line);

	highlight_free(line);
	highlight_finish();
	return (0);
}
