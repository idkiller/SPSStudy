
enum Direction { Top, Right, Bottom, Left, Size};

static int N;
static int M;
static int K;

static int Reverse[Direction::Size] = { Direction::Bottom, Direction::Left, Direction::Top, Direction::Right };

struct Piece {
	int index;
	int picks;
	int edges[Direction::Size];
	Piece *matchs[Direction::Size][6]; // matched index of the piece (K + 1 <= 6)
	int match_size[Direction::Size] = { 0, 0, 0, 0 }; // size of matches
	bool visited;

	inline Piece(): visited(false), picks(0), index(0) {}
	inline Piece(int* piece): Piece() {
		int i, v, offset;
		visited = false;
		picks = 0;
		for (int d = Direction::Top; d < Direction::Bottom; d++) {
			offset = d * M;
			for (v = 0, i = 0; i < M; i++) {
				v <<= 2;
				v += (piece[offset + i] + 1);
				if (piece[offset + i] == 1) picks++;
			}
			this->edges[d] = v;
		}
		for (int d = Direction::Bottom; d < Direction::Size; d++) {
			offset = d * M;
			for (v = 0, i = 0; i < M; i++) {
				v <<= 2;
				v += (piece[offset + M - i - 1] - 1) * -1;
				if (piece[offset + M - i - 1] == 1) picks++;
			}
			this->edges[d] = v;
		}

	}

	inline void compare(Piece *piece) {
		for (int i = Direction::Right; i < Direction::Size; i++) {
			if (this->edges[i] == piece->edges[Reverse[i]]) {
				this->matchs[i][this->match_size[i]] = piece;
				this->match_size[i]++;
			}
			if (piece->edges[i] == this->edges[Reverse[i]]) {
				piece->matchs[i][piece->match_size[i]] = this;
				piece->match_size[i]++;
			}
		}
	}
};

static Piece pieces[49 * 49 + 5];
static int piece_count = 0;
static int piece_index = 0;
static Piece *map[49][49];

bool dfs(int x, int y) {
	Piece *left, *top, *right, *bottom;
	bool result = false;
	if (y == N) {
		result = true;
	}
	else if (y == 0) {
		left = map[x - 1][y];
		for (int i = 0; i < left->match_size[Direction::Right]; i++) {
			right = left->matchs[Direction::Right][i];
			if (!right->visited) {
				right->visited = true;
				map[x][y] = right;
				if (x + 1 == N) {
					result = dfs(0, y + 1);
				}
				else {
					result = dfs(x + 1, y);
				}
				if (!result) {
					map[x][y] = nullptr;
				}
				else {
					return result;
				}
				right->visited = false;
			}
		}
	}
	else if (x == 0) {
		top = map[x][y - 1];
		for (int i = 0; i < top->match_size[Direction::Bottom]; i++) {
			bottom = top->matchs[Direction::Bottom][i];
			if (!bottom->visited) {
				bottom->visited = true;
				map[x][y] = bottom;
				result = dfs(x + 1, y);
				if (!result) {
					map[x][y] = nullptr;
				}
				else {
					return result;
				}
				bottom->visited = false;
			}
		}
	}
	else {
		left = map[x - 1][y];
		top = map[x][y - 1];
		for (int i = 0; i < left->match_size[Direction::Right]; i++) {
			right = left->matchs[Direction::Right][i];
			for (int j = 0; j < top->match_size[Direction::Bottom]; j++) {
				bottom = top->matchs[Direction::Bottom][j];
				if (right == bottom) {
					if (!right->visited) {
						right->visited = true;
						map[x][y] = right;
						if (x + 1 == N) {
							result = dfs(0, y + 1);
						}
						else {
							result = dfs(x + 1, y);
						}
						if (!result) {
							map[x][y] = nullptr;
						}
						else {
							return result;
						}
						right->visited = false;
					}
				}
			}
		}
	}
	return result;
}

void init(int n, int m, int k) {
	N = n;
	M = m;
	K = k;
	piece_count = N * N + K;
	piece_index = 0;
}

void addPiece(int piece[]) {
	pieces[piece_index] = piece;
	pieces[piece_index].index = piece_index;
	piece_index++;
}

int getTreasure() {
	bool completed = false;

	for (int i = 0; i < piece_count; i++) {
		for (int j = i + 1; j < piece_count; j++) {
			pieces[i].compare(&pieces[j]);
		}
	}
	for (int i = 0; i < piece_count; i++) {
		if (!pieces[i].visited) {
			pieces[i].visited = true;
			map[0][0] = &pieces[i];
			completed = dfs(1, 0);
			pieces[i].visited = false;
			if (completed) break;
		}
	}
	return map[N / 2][N / 2] == nullptr ? -1 : map[N / 2][N / 2]->picks;
}
