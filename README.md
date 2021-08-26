# stocksquid
technically a chess engine


Positions are evaluated on material value, taking into account a mobility heuristic. The algorithm performs a minimax search with alpha-beta pruning to choose a move, optimised through move ordering.

Board visualisation is simply done through text, and all interaction is via the console. Moves are input in standard notation, but without the use of captures symbols. Capital letters denote white pieces, lower case letters denote black pieces and this convention applies also when inputting a move (e.g. Be5 and be5 are different).
