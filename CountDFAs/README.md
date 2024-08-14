
`count-dfas.cpp` counts the number of distinct languages over {a,b} recognizable by n-state DFAs.
Compile the code, then pass `n` on the command line.

- http://list.seqfan.eu/pipermail/seqfan/2024-July/075162.html
- http://list.seqfan.eu/pipermail/seqfan/2024-July/075164.html
- https://cs.stackexchange.com/questions/48136/testing-two-dfas-generate-the-same-language-by-trying-all-strings-upto-a-certain
- https://quuxplusone.github.io/blog/2024/07/27/counting-dfas/

`count-dfas.py` counts the number of distinct languages recognizable by n-state DFAs, over languages of size `ALPHABET_SIZE`.
Change the value of `ALPHABET_SIZE` in the source code, then pass `n` on the command line.

`count-dfas-up-to-alphabet-recoloring.cpp` counts the number of distinct languages recognizable by n-state DFAs, over languages of size `ALPHABET_SIZE`,
"up to recoloring." For example, the languages `[ab]*` and `[ac]*` and `a*` and `.*` are all considered
equivalent up to recoloring, because you can match any of them with a DFA for `a*` and a preprocessing
step that recolors all the letters you care about into `a`s.
Compile with `-DALPHABET_SIZE=2`, then pass `n` on the command line.
Compile with `-DNO_RECOLORING` if you don't want recoloring (but then increasing the size of the alphabet
will naturally increase the number of languages, without bound, because `a*` and `b*` and `c*`... will all
be considered different languages).

`count-regexes.py` counts the number of distinct languages over {a,b} recognizable by n-character regexes.
Notice that common sense (and this count) disagree with Python as to whether `a**` is a valid regex.
Common sense says yes; Python says no, that's so redundant that it shouldn't even be considered well-formed.


