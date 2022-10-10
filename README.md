# Markdown Utils

## Installation

Run the following command which will require your root password to install at
`/usr/bin`:

```sh
curl -s https://raw.githubusercontent.com/carlos157oliveira/markdown-utils/main/install.pl | sudo perl
```

## Example usage

```sh
md fmt [--max-line-length=80] [--output=formated-file.md] markdown-file.md
        Default maximum line length: 80 8-bit characters
        Default output: stdout.
        The first non-option value ends the argument list.
md toc markdown-file.md
md -h|--help
```