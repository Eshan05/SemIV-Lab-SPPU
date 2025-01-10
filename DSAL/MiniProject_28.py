# 28. Design a mini project to implement Snake and Ladders Game using Python.

import random
from typing import List, Optional, Tuple


SNAKES = {
	16: 6,
	46: 25,
	49: 11,
	62: 19,
	64: 60,
	74: 53,
	89: 68,
	92: 88,
	95: 75,
	99: 80,
}

LADDERS = {
	2: 38,
	7: 14,
	8: 31,
	15: 26,
	21: 42,
	28: 84,
	36: 44,
	51: 67,
	71: 91,
	78: 98,
	87: 94,
}


def read_int(prompt: str, lo: Optional[int] = None, hi: Optional[int] = None) -> int:
	while True:
		s = input(prompt).strip()
		try:
			x = int(s)
			if lo is not None and x < lo:
				print(f"Value must be >= {lo}.")
				continue
			if hi is not None and x > hi:
				print(f"Value must be <= {hi}.")
				continue
			return x
		except Exception:
			print("Invalid number. Try again.")


def read_yes_no(prompt: str, default: bool) -> bool:
	while True:
		s = input(prompt).strip().lower()
		if s == "":
			return default
		if s in ("y", "yes"):
			return True
		if s in ("n", "no"):
			return False
		print("Enter y/n.")


def print_mappings() -> None:
	print("\n--- Ladders (bottom -> top) ---")
	for k in sorted(LADDERS.keys()):
		print(f"{k:>3} -> {LADDERS[k]:>3}")
	print("\n--- Snakes (head -> tail) ---")
	for k in sorted(SNAKES.keys()):
		print(f"{k:>3} -> {SNAKES[k]:>3}")


def show_positions(names: List[str], pos: List[int]) -> None:
	print("\nPositions:")
	for i in range(len(names)):
		print(f"  {names[i]}: {pos[i]}")


def apply_snake_or_ladder(p: int) -> Tuple[int, str]:
	if p in LADDERS:
		return (LADDERS[p], "ladder")
	if p in SNAKES:
		return (SNAKES[p], "snake")
	return (p, "")


def play_game(
	names: List[str],
	seed: Optional[int],
	exact_finish: bool,
	extra_turn_on_6: bool,
	auto_play: bool,
) -> None:
	rng = random.Random(seed)
	pos = [0] * len(names)
	rolls = [0] * len(names)
	turns = 0
	cur = 0

	print("\nGame start! First to reach 100 wins.")
	show_positions(names, pos)

	while True:
		name = names[cur]
		if not auto_play:
			cmd = input(f"\n{name}'s turn (at {pos[cur]}). Press Enter to roll (q to quit): ").strip().lower()
			if cmd.startswith("q"):
				print("Game aborted.")
				return

		dice = rng.randint(1, 6)
		rolls[cur] += 1
		turns += 1
		print(f"{name} rolled: {dice}")

		next_pos = pos[cur] + dice
		if exact_finish and next_pos > 100:
			print(f"Exact finish rule: {name} stays at {pos[cur]}.")
			next_pos = pos[cur]
		elif next_pos > 100:
			next_pos = 100

		if next_pos != pos[cur]:
			print(f"{name} moved to {next_pos}.")

		landed = next_pos
		final_pos, kind = apply_snake_or_ladder(landed)
		if kind == "ladder":
			print(f"Ladder! {name} climbs from {landed} to {final_pos}.")
		elif kind == "snake":
			print(f"Snake! {name} slides from {landed} to {final_pos}.")

		pos[cur] = final_pos
		show_positions(names, pos)

		if pos[cur] == 100:
			print(f"\nWinner: {name}!")
			print(f"Total turns taken (all players combined): {turns}")
			print("Roll counts:")
			for i in range(len(names)):
				print(f"  {names[i]}: {rolls[i]}")
			return

		if extra_turn_on_6 and dice == 6:
			print(f"Extra turn for {name} (rolled 6).")
		else:
			cur = (cur + 1) % len(names)


def start_new_game() -> None:
	print("\n--- New Game ---")
	n = read_int("Enter number of players (2-4): ", 2, 4)
	names: List[str] = []
	for i in range(n):
		nm = input(f"Player {i + 1} name: ").strip()
		if nm == "":
			nm = f"P{i + 1}"
		names.append(nm)

	seed_in = input("Random seed (blank for random): ").strip()
	seed = None
	if seed_in != "":
		try:
			seed = int(seed_in)
		except Exception:
			seed = None
			print("Invalid seed; using true randomness.")

	exact_finish = read_yes_no("Exact finish required? (y/n) [y]: ", True)
	extra_turn_on_6 = read_yes_no("Extra turn on rolling 6? (y/n) [n]: ", False)
	auto_play = read_yes_no("Auto-play (no Enter prompt)? (y/n) [n]: ", False)

	play_game(names, seed, exact_finish, extra_turn_on_6, auto_play)


def main() -> None:
	while True:
		print("\n===== MiniProject 28: Snake and Ladders =====")
		print("1. Start new game")
		print("2. Show snakes & ladders")
		print("3. Exit")
		ch = input("Enter choice: ").strip()
		if ch == "1":
			start_new_game()
		elif ch == "2":
			print_mappings()
		elif ch == "3":
			break
		else:
			print("Invalid choice.")


if __name__ == "__main__":
	main()
