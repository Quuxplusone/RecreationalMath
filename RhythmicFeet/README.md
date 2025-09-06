These opinionated word lists contain English nouns with a given stress pattern.
"uiu-nouns.txt" contains amphribrachs; "iuu-nouns.txt" contains dactyls;
and "uiuu-nouns.txt" contains secundi.

These word lists were derived from [Ashley Bovan's](http://www.ashley-bovan.co.uk/)
"rhythmicfeet.zip", which you can find in the Wayback Machine
[here](https://web.archive.org/web/20160317102818/http://sites.google.com/site/ashleybovan/pdf/rhythmicfeet.zip).
I manually stripped out:

- the non-nouns
- hundreds of unremarkable surnames
- any plural whose singular was already present (e.g. remove UNITIES, but not
    ESSENCES, from the list of dactyls)
- words where Bovan's computer analysis had given a clearly wrong answer
    (e.g. remove NASOPHARYNX and OXYMORON from the list of secundi)
- words whose stress-pattern might be subject to reasonable variation
    (e.g. remove KILOMETER, PSYCHOPATHY from the list of secundi)
- words whose number of syllables might be subject to reasonable variation
    (e.g. remove VIOLENCE, GARDENER, NURSERY, TIDDLYWINKS
    from the list of dactyls)
- words whose secondary stress in my opinion makes it more of an antibacchius
    than a dactyl (e.g. remove CUCUMBER, INVOICES, VAMPIRE
    from the list of dactyls; remove SUBCONTRACTOR, RELOCATING from the
    list of secundi)
- most gerunds (e.g. GLITTERING, GLISTENING), arbitrarily preserving some
    in the plural (e.g. WANDERINGS, MUTTERINGS) and others (GATHERING,
    OFFERING, LEAVENING)
- most adjectives-turned-nouns (e.g. POSITIVE, PRIMITIVE, PURGATIVE,
    DECIMAL), arbitrarily preserving some (LAXATIVE, SEDATIVE, FUGITIVE)
