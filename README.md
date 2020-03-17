# kat 🐱
A simple cat-like/library syntax highlighter made in C

## Introduction
Kat is a C-only syntax highlighter made in C that comes in two flavors: library and a companion program. The former allows
the user to easily highlight code snippets (or entire source code) line by line. The latter is a cat-like tool that reads
from _stdin_ or a file and outputs a C highlighted code using ANSI Escape Sequences.

Kat was born as a [PBD](https://github.com/Theldus/PBD) feature that highlights the lines that changed as soon as
possible. So I thought: why I wouldn't do a library with it? some people might find it useful as well.

## Library
Kat library is pretty simple and consists in a few set of functions:

<table>
<tr>
<td> <b>Function</b> </td> <td> <b>Description</b> </td>
</tr>
<tr>
<td>

```c
int highlight_init(const char *theme_file);
```

</td>
<td>

This function initializes the highlight context and its global state. It should be called before
any other routines present here. 

If <i>theme_file</i>
is not NULL, Kat will read a theme from the specified
file. More about themes latter.

</td>
</tr>
<tr>
<td>

```c
void highlight_finish(void);

```

</td>
<td>
This function just finishes the dynamically
allocated structures and reset the global
state.
</td>
<tr>
<td>

```c
char *highlight_line(const char *line, char *hl);
```

</td>
<td>

Highlight the given _line_ and returns a
dynamically allocated string highlighted.

Note that the _hl_ parameter is a pointer to
a dynamically allocated string. This option
is useful for reusing a string pointer
returned from this same function. If NULL,
_highlight_line_ will allocate a dynamic
string and returns it.

</td>
<tr>
<td>

```c
void highlight_free(char *line);
```

</td>
<td>

Deallocates a previously allocated string
from `highlight_line`.

</td>
</tr>
</table>

### Example
A simple example as follows (complete source in example/ex1.c):
```c
...
/* Initialize the Kat global state. */
if (highlight_init(NULL) < 0)
	quit(-1, "Unable to initialize the highlight!\n");

/* Highlight some lines. */
line = highlight_line("#include <stdio.h>", NULL);
puts(line);
line = highlight_line("int main(int argc, char **argv){ /* Comment. */", line);
puts(line);
line = highlight_line("    printf(\"Hello World \%s\\n\", argv[1]);", line);
puts(line);
line = highlight_line("}", line);
puts(line);

/* Free the allocated line and finalizes the highlighter context. */
highlight_free(line);
highlight_finish();
...
```
will produce the following output below:
<p align="center">
<img align="center" src="https://i.imgur.com/2m2pKK5.png" alt="Kat example">
<br>
Kat example
</p>

## Program
Kat also ships with a cat-like program that reads from _stdin_ or a file and outputs a highlighted C code to _stdout_.
Note, however, that Kat does not intend to entirely substitute a standard `cat(1)` utility. If you _really_ need a
replacement, check out the amazing [bat(1)](https://github.com/sharkdp/bat) project.

## Customization
Despite its simplicity, Kat (with some restrictions) easily allows the addition of new keywords and symbols.
Moreover, Kat has 2 color schemes by default (8-colors and 256-colors) and also supports reading external themes.

The theme layout is as follows: eight numbers separated by commas, spaces, tabs and/or line breaks from 0 to 255
that represents the range of colors supported by a 256-color* terminal.

Each position represents exactly:
- Preprocessor color
- Type color
- Keywords color
- Number color (including integer, floats ...)
- String / Char color
- Comment color
- Function call color
- Symbols color

As an example, a text file with the content: `47, 160, 57, 226, 207, 241, 69, 208` is valid and will configure
respectively: 47 (preprocessor, green), 160 (types, red), 57 (keywords, purple), 226 (number, yellow) and so on...

You can select an external theme file via _theme_file_ parameter, in `int highlight_init(const char *theme_file);` or via
the complimentary program, via `-t/--theme` parameter.

*To convert an RGB value to the nearest approximated value in a 256-color palette, please check [colortrans.py](https://gist.github.com/MicahElliott/719710/)

## Performance

Although not the main focus, Kat is surprisingly fast.

Our methodology consisted of using three programs with the following parameters (cat, kat, and [bat(1)](https://github.com/sharkdp/bat)):
- `cat [file] > /dev/null`
- `kat [file] > /dev/null` (Kat always uses ANSI Escape Sequences, regardless of whether the output is tty or a file)
- `bat --color=always -p [file] > /dev/null` (`--color=always` keeps the highlight on even for a file, and `-p`
  removes extra decorators).

The following files were tested and for each one, they were executed 8 times and obtained the average of the times:
- 1) **First test:** sqlite3.c (amalgamated version): 4.5MiB file and 133.611 lines of code.

<p align="center">
<img align="center" src="https://i.imgur.com/Sdn91sc.png" alt="cat vs kat vs bat with sqlite3.c">
<br>
cat vs kat vs bat with sqlite3.c
</p>

- 2) **Second test:** highlight.c (from Kat's own source): 22.1kB file and 813 lines of code.

<p align="center">
<img align="center" src="https://i.imgur.com/27DjDnd.png" alt="cat vs kat vs bat with highlight.c">
<br>
cat vs kat vs bat with highlight.c
</p>

Thanks to its simplicity, it is even possible to overcome the `cat` for small inputs and stay close even for large inputs.
The benchmarks above can be executed with: `make bench`.

## Limitations
Kat has some limitations that might be useful or not:
- Only C highlight support: However, it's easy to add 'support' to similar languages, like C++, Java, since most of the
  changes would be related to keywords and symbols. **(library limitation)**

- No Regular Expressions support **(library limitation)**

- Full Buffered: Meaning that Kat will keep in memory all the input until find an EOF and only after that, parses
and prints the output. While this could sounds unreasonable, Kat is made to highlight source codes and most of them
are just a few kilobytes and in the worst case, few megabytes, so I'm not expecting that a user will want to see an
entire huge source tree in the terminal. If you're planning to see all the Linux source tree in Kat, you are crazy,
really. **(Kat program limitation)**

- Not a `cat(1)` substitute. Although the Kat program can be improved, this is not in my plans. Because you can always
use cat in conjunction with kat, e.g: `cat file1 file2 | kat`. **(Kat program limitation)**

- Kat is not a _pager_ and does not have this type of functionality, _however_, it is perfectly fine to pipe to one,
such as: `kat file | less -R`. **(Kat program limitation)**

## Building
Building Kat program (kat) and library (libkat.ar) is as simple as possible, just clone the repository and invoke make:
```bash
git clone https://github.com/Theldus/kat
cd kat/
make
```
## License and Authors
Kat is licensed under MIT License. Written by Davidson Francis and (hopefully) others
[contributors](https://github.com/Theldus/kat/graphs/contributors).
