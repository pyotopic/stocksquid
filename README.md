# stocksquid
Like stockfish, but not quite as good... currently plays at about the 1000 level.

Positions are evaluated on material value, taking into account a mobility heuristic. The algorithm performs a minimax search with alpha-beta pruning to choose a move, optimised through move ordering.

Board visualisation is simply done through text, and all interaction is via the console. Moves are input as:

<pieceCharacter><square>

pieceCharacter is the same letter as in standard in English chess notation, using CAPITALS for white and LOWER-CASE for black.

This format applies for ALL moves, including captures, castles and pawn moves (e.g. if black plays Bxe5, this becomes be5, if white plays O-O, this becomes Kg1, if black plays g6, this becomes pg6).