STRINGS-CP932(1)

# NAME

strings-cp932 - extract printable windows-31j/cp932 strings

# SYNOPSIS

*strings-cp932* [-t _format_] [_file_...]

# DESCRIPTION

_Strings-cp932_ finds and prints strings containing printable
Windows-31J-encoded characters in a file, default standard input. Printable
characters are taken to be ASCII characters from blank through tilde
(hexadecimal 20 through 7E), inclusive, and all other characters defined
in the Windows-31J character set. If _-t_ is passed, _strings-cp932_ prepends
file offsets to found strings. The _format_ argument passed to _-t_ can be
_d_ for decimal offsets, _x_ for hexadecimal offsets or _o_ for octal offsets.

# BUGS

*-n* is not supported for now; even 1-character strings are printed. You can
pipe the output of _strings-cp932_ to _awk 'length($0)>n'_  (or _awk
'length($0)-length($1)-1>n'_ if you're using *-t*) where _n_ is the desired
minimum length if you want to filter out shorter strings.

# SEE ALSO

_https://www.unicode.org/Public/MAPPINGS/VENDORS/MICSFT/WINDOWS/CP932.TXT_ -
Windows-31J mapping table used by strings-cp932
