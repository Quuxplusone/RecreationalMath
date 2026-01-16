This relates to the SeqFan thread
https://groups.google.com/g/seqfan/c/eNLqlI6Fpv8/m/YDMR-Z2TCAAJ

    Subject: Recreational mathematics: "stringy" primes
    From: Juuso Alasuutari <juuso.alasuutari@gmail.com>

The idea is that a string has the property Juuso called "being a stringy prime"
if its ASCII representation is prime when loaded as a big-endian number, *and*
is prime when loaded as a little-endian number. I suggested that this is not
a property of numbers but of ASCII strings (really, of byte-patterns), and
that the property could better be called "bi-endian primality", leaving the
door open for similar properties like "bi-endian abundance,"
"little-endian (only) primality," and so on.

`find-biendian-primes.py` is a Python program that filters an input list
of strings (one per line) and reports those that are bi-endian primes.

----

In a response, M F Hasler mentioned base64 encoding; therefore
`find-base64-reversals.py` takes an input list of strings and reports
those strings such that if you base64-encode them, reverse the bytes,
and base64-decode them, you get a(nother) string from the input list.

Because of how base64 does padding, a base64 reversal must have a
length that is a multiple of 3. Also, any even-length base64 reversal
must in fact be a base64 palindrome (it must reverse to itself).

One three-letter string can be the base64 reversal of another only
if the bits of the two strings match up as follows:

    ABCDEFGH IJKLMNOP QRSTUVWX
    STUVWXMN OPQRGHIJ KLABCDEF

ASCII letters always have the high-order bits 01:

    01...... 01...... 01......

Therefore the only three-byte bit-patterns that work are of the form:

    01cdefgh 0101gh01 0101cdef

For example, `AQQ` reverses to `DUP`, `AQR` reverses to `HUP`, `AUP` reverses
to itself, and so on; but the only letters that can appear in middle
position are `Q`, `U`, and `Y`, which really limits things.
`MUS` reverses to itself, as does `NYS`, but those are the only two
"words" that appear in any of my wordlists.

With six-letter words, we have these six-byte bit-patterns as candidates:

    01cdefgh 0101ij01 0101klmn 01klmnij 0101gh01 0101cdef

For example, `A]SOUP` is a base64 palindrome, as is `MURIUS`, as is `AUPAUP`.
