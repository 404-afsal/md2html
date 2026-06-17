# md2html

A lightweight, config-driven Markdown-to-HTML converter written in C.

## How it works

Symbols and their corresponding HTML tags are defined in `config.txt` — no hardcoding in the parser. The parser scans each line, collects all symbol positions as tickets, sorts them by position, then processes them in order using a stack to handle open/close state.

## Supported syntax

Symbols are single-character. Any unique single-character symbol defined in `config.txt` will work.

**Default config:**

| Symbol | Output | Type |
|--------|--------|------|
| `#` | `<h1>` | Line |
| `%` | `<blockquote>` | Line |
| `*` | `<em>` | Inline |
| `_` | `<i>` | Inline |
| `~` | `<del>` | Inline |
| `` ` `` | `<code>` | Inline |
| `---` | `<hr>` | Line (void) |
| `@` | `&nbsp;` | Inline (void) |

Inline tags support nesting as long as they close in the correct order (inner before outer).

## Limitations

- Only `h1` by default (no `h2`–`h6`)
- No bold, links, images, or lists out of the box
- Multi-character symbols not supported
- No HTML boilerplate in output (`<html>`, `<head>`, `<body>`)
- LINE tags apply to the whole line regardless of position

## Usage

```bash
./md2html input.md             # outputs to output.html
./md2html input.md custom.html # custom output file
```

## Build

```bash
make
make clean
```

## Config format

```
NON_VOID::<symbol>::<open_tag>::<close_tag>::LINE|INLINE
VOID::<symbol>::<tag>::LINE|INLINE
```