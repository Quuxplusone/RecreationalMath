==Wordle-based puzzles==

`WordList.py` is the official Wordle word lists: `targetWords` is the list of "common"
(potential solution) words, and `dictionary` is the entire list of allowed guesses.

`LongestGame.py` implements a non-exhaustive randomized search for long Wordle games
per ["What is the longest Wordle game?"](https://puzzling.stackexchange.com/questions/115707/what-is-the-longest-wordle-game/)
Ordinary hard-mode Wordle permits you to reuse letters you already saw as gray, and in fact
to make the exact same guess six times in a row if you want. This program's logic is stricter;
it doesn't allow you to make any guess that's logically inconsistent with what you've already
seen. (Yellow letters must appear in a different position; gray letters can't appear at all.)

`CountImages.py` implements a (slow) exhaustive count of sets of rows that can't all
appear in a single Wordle game, per ["How many Wordle images are there?"](https://puzzling.stackexchange.com/questions/114469/how-many-wordle-images-are-there)

`CountImages2.py` is very slightly smarter: For each possible target word, compute the set of
possible row-colorings. Then, for each (set of row-colorings), see if there's any
(set of row-colorings) that subsumes it; if so, discard the smaller set. Turns out this
discards only about 10% of the sets; we still have to brute-force through all the rest.
