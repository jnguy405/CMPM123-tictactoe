# Tic-Tac-Toe | CMPM123

C++ basic implementation of Tic-Tac-Toe with ImGui debug logger. Logs can be found in `game_log.txt` which include move counters, state strings, and additional player/game info. Design process encompassed analyzing and creating pseudocode followed by basic function implementations.

**Note**: no additional class changes besides `TicTacToe.cpp` and `Application.cpp`.

---

## AI Functionality

Utilizes the Negamax algorithm, with Alpha-beta Pruning, to evaluate both AI and player decision spaces by recursively simulating future game states. Each possible move is assigned a heuristic score that reflects its strategic advantage or risk. AI scoring and possible moves are logged to the Dear ImGui debug logs: Options -> Show Scores.

---

## Citations & References

1. [ImGui](https://github.com/ocornut/imgui/tree/docking)
2. [Personal ImGui](https://github.com/jnguy405/CMPM123-imgui-starter)
3. [Base Minimax Reference](https://www.youtube.com/watch?v=trKjYdBASyQ)
4. [Negamax](https://en.wikipedia.org/wiki/Negamax#:~:text=Negamax%20search%20is%20a%20variant,the%20value%20to%20player%20B.)
5. [Alpha-beta Pruning](https://en.wikipedia.org/wiki/Alpha%E2%80%93beta_pruning)