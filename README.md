# NAME

hmmm.c - A simpleminded word generator based on hidden markov models

# SYNOPSIS

     Usage:
       hmmm [-v] COUNT DEPTH SEED <WORDS
       where:
         -v    = be verbose
         COUNT = how many words to generate
         DEPTH = integer from 1..4 inclusive
         SEED  = a random integer
         WORDS = a list, one item per line, containing [A-Za-z ]

     Warning:
       needs nearly 70MB of RAM to run.

Given a list, one word per line, it will output "similar" words.

Presented purely for nostalgia.

# HISTORY

Way back in 1989, I tried to write something similar to this.
It tried to allocate about the same amount of memory (70MB).
By today's standards this is tiny. Even by Y2K standards it's okay.
By 1989 standards, the program would have consumed the entirety of
the memory of the mainframe I tried to run it on... For all users. Oops!

