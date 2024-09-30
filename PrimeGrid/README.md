
A.K. Dewdney's Computer Recreations column of July 1988:
Gordon Lee has constructed a 6-by-6 digit grid concealing 169 prime numbers
(170 if you count "1" as prime) in the style of a word search.
https://www.jstor.org/stable/24989170

A.K. Dewdney's Computer Recreations column of January 1989:
Stephen C. Root constructs a grid concealing 187 prime numbers
(188 if you count "1" as prime).
https://www.jstor.org/stable/24987117

Can we do better?

Well, not by brute force or random search, as this Python program proves!

It makes more sense to tackle it as a word-search-filling task: given
a fixed list of 78498 "words" (prime numbers), try to pack as many as possible
into the 6x6 grid, starting at the upper left corner and working your
way down (or suchlike).
