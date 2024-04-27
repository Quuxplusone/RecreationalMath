This directory relates to
["Farmer needs to get his word across the river"](https://puzzling.stackexchange.com/questions/86220/farmer-needs-to-get-his-word-across-the-river/)
from Puzzling StackExchange.

----

In the spirit of the classic [river-crossing puzzles](https://en.wikipedia.org/wiki/River_crossing_puzzle):

    The word PLANS is on the west shore of a river, while the word PAINTER is on the east shore.
    The farmer needs to take his word PLANS from the west side to the east side.
    However, he can only cross the river with one "chunk" at a time,
    and the words on both shores must be valid words at every intermediate step.    
    
    ----------------------------------------------------------------------------------------
    
    Solution:
    
     PLANS            PAINTER
       |            .   |
       |          .     |
       |        ER      |
       |      .         |
       v    .           v
    PLANERS           PAINT
       |    .           |
       |      .         |
       |       ERS      |
       |          .     |
       v            .   v
     PLAN             PAINTERS
       |            .   |
       |          .     |
       |       TER      |
       |      .         |
       v    .           v
    PLANTER           PAINS
       |    .           |
       |      .         |
       |       LANT     |
       |          .     |
       v            .   v
      PER             PLANTAINS
       |            .   |
       |          .     |
       |       TAIN     |
       |      .         |
       v    .           v
    PERTAIN           PLANS

A "chunk" is a string of consecutive letters taken from anywhere within the word.
It can also be inserted anywhere into the other word.  It must consist of at least
two letters (no single-letter chunks), but there is no length limit.  The chunk
does not need to be a valid word.
    
Notice that the farmer succeeded in moving his word PLANS from the west shore to
the east shore, but the other word did not survive in its original form.
There is no requirement to keep the other word in its original form.

Although the above example happens to show the movement of the chunks alternating
between east-to-west and west-to-east, this is not a requirement.  The farmer is
permitted to ferry an empty boat from one shore to the other whenever he deems
necessary.

Now you try it!

    Problem:
    
    The word RETESTED is on the west shore of a river, while the word INGRAIN is on the east shore.
    The farmer needs to take his word RETESTED from the west side to the east side.
    However, he can only cross the river with one "chunk" at a time,
    and the words on both shores must be valid words at every intermediate step.
