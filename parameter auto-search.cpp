#include <iostream>
using namespace std;


bool print_codes = false, print_misses = false;
int miss_count = 0;
struct Matrix
{
	int value = -1;
	int i = -1, j = -1;
	int matrix_index = 0;  // 0 empty, 1 A, 2 B
	int set_index = -1;
	int group_index = -1;
	bool written = false;  // whether B[i][j] is written
	bool stored = false;  // whether A[i][j] is stored
};
Matrix A[256][256], B[256][256];
struct Cache_Line
{
	int matrix_index = 0;  // 0 empty, 1 A, 2 B
	int set_index = -1;
	int group_index = -1;
};
Cache_Line cache[32];


void init_matrices(int M, int N)
{
	int set_index = 0, group_index = 0, total = 0;
	for (int i = 0; i < N; i++)
		for (int j = 0; j < M; j++)
		{
			A[i][j].value = total;
			A[i][j].i = i;
			A[i][j].j = j;
			A[i][j].matrix_index = 1;
			A[i][j].set_index = set_index;
			A[i][j].group_index = group_index;
			total++;
			if (total % 8 == 0)
			{
				set_index++;
				if (set_index == 32)
				{
					set_index = 0;
					group_index++;
				}
			}
		}
	set_index = group_index = total = 0;
	for (int i = 0; i < M; i++)
		for (int j = 0; j < N; j++)
		{
			B[i][j].i = i;
			B[i][j].j = j;
			B[i][j].matrix_index = 2;
			B[i][j].set_index = set_index;
			B[i][j].group_index = group_index;
			total++;
			if (total % 8 == 0)
			{
				set_index++;
				if (set_index == 32)
				{
					set_index = 0;
					group_index++;
				}
			}
		}
}


void init_cache()
{
	for (int i = 0; i < 32; i++)
		cache[i].set_index = i;
}


void clear_cache()
{
	for (int i = 0; i < 32; i++)
	{
		cache[i].matrix_index = 0;
		cache[i].group_index = -1;
		cache[i].set_index = -1;
	}
}


bool is_transpose(int M, int N, Matrix A[256][256], Matrix B[256][256])
{
	for (int i = 0; i < N; i++)
		for (int j = 0; j < M; j++)
			if (A[i][j].value < 0 || B[j][i].value < 0)
				return false;
			else if (A[i][j].value != B[j][i].value)
				return false;
	return true;
}


// load A[i][j] to cache
void load(int i, int j, int a_index)
{
	int set_index = A[i][j].set_index;
	int group_index = A[i][j].group_index;
	Cache_Line cache_line = cache[set_index];
	A[i][j].stored = true;
	bool conflict = true;
	if (cache_line.matrix_index == 0)conflict = false;
	if (cache_line.matrix_index == 1 && cache_line.group_index == group_index)conflict = false;
	if (cache_line.matrix_index == 1 && cache_line.group_index != group_index)
		if (print_misses)cout << "// A conflict when loading: replace A" << cache_line.group_index << "-" << cache_line.set_index << " with A" << group_index << "-" << set_index << endl;
	if (cache_line.matrix_index == 2)
		if (print_misses)cout << "// B conflict when loading: replace B" << cache_line.group_index << "-" << cache_line.set_index << " with A" << group_index << "-" << set_index << endl;
	if (print_codes)cout << "a" << a_index << " = A[" << i << "][" << j << "]; " << endl;
	if (cache_line.matrix_index == 0 || conflict)  // load cache line
	{
		miss_count++;
		cache[set_index].matrix_index = 1;
		cache[set_index].group_index = group_index;
	}
}


// store A[i][j] to B[j][i]
void store(int i, int j, int a_index)
{
	int set_index = B[j][i].set_index;
	int group_index = B[j][i].group_index;
	Cache_Line cache_line = cache[set_index];
	B[j][i].value = A[i][j].value;
	B[j][i].written = true;
	bool conflict = true;
	if (cache_line.matrix_index == 0)conflict = false;
	if (cache_line.matrix_index == 2 && cache_line.group_index == group_index)conflict = false;
	if (cache_line.matrix_index == 2 && cache_line.group_index != group_index)
		if (print_misses)cout << "// B conflict when storing: replace B" << cache_line.group_index << "-" << cache_line.set_index << " with B" << group_index << "-" << set_index << endl;
	if (cache_line.matrix_index == 1)
		if (print_misses)cout << "// A conflict when storing: replace A" << cache_line.group_index << "-" << cache_line.set_index << " with B" << group_index << "-" << set_index << endl;
	if (print_codes)cout << "B[" << j << "][" << i << "] = a" << a_index << ";" << endl;
	if (cache_line.matrix_index == 0 || conflict)  // load cache line
	{
		miss_count++;
		cache[set_index].matrix_index = 2;
		cache[set_index].group_index = group_index;
	}
}


// calculate the number of extra conflicts when loading and storing A[i][j] -- A[i][j + 3]
int cal_conflict_count(int i, int j)
{
	int conflicts = 0;
	int A_set_index = A[i][j].set_index;
	int A_group_index = A[i][j].group_index;
	if (cache[A_set_index].matrix_index == 2)
	{
		bool finished = true;
		for (int m = 0; m < 60; m++)
			for (int n = 0; n < 68; n++)
				if (B[m][n].group_index == cache[A_set_index].group_index && B[m][n].set_index == A_set_index && !B[m][n].written)
				{
					finished = false;
					m = 2022;
					n = 2022;
				}
		if (!finished)conflicts++;
	}
	else if (cache[A_set_index].matrix_index == 1 && cache[A_set_index].group_index != A_group_index)
	{
		bool finished = true;
		for (int m = 0; m < 68; m++)
			for (int n = 0; n < 60; n++)
				if (A[m][n].group_index == cache[A_set_index].group_index && A[m][n].set_index == A_set_index && !A[m][n].stored)
				{
					finished = false;
					m = 2022;
					n = 2022;
				}
		if (!finished)conflicts++;
	}
	A[i][j].stored = true;
	A[i][j + 1].stored = true;
	A[i][j + 2].stored = true;
	A[i][j + 3].stored = true;
	for (int jj = j; jj < j + 4; jj++)
	{
		int B_set_index = B[jj][i].set_index;
		int B_group_index = B[jj][i].group_index;
		if (B_set_index == A_set_index)
		{
			bool finished = true;
			for (int m = 0; m < 68; m++)
				for (int n = 0; n < 60; n++)
					if (A[m][n].group_index == A_group_index && A[m][n].set_index == A_set_index && !A[m][n].stored)
					{
						finished = false;
						m = 2022;
						n = 2022;
					}
			if (!finished)conflicts++;
		}
		else if (cache[B_set_index].matrix_index == 2 && cache[B_set_index].group_index == B_group_index)continue;
		else if (cache[B_set_index].matrix_index == 0)continue;
		else if (cache[B_set_index].matrix_index == 2)
		{
			bool finished = true;
			for (int m = 0; m < 60; m++)
				for (int n = 0; n < 68; n++)
					if (B[m][n].group_index == cache[B_set_index].group_index && B[m][n].set_index == B_set_index && !B[m][n].written)
					{
						finished = false;
						m = 2022;
						n = 2022;
					}
			if (!finished)conflicts++;
		}
		else
		{
			bool finished = true;
			for (int m = 0; m < 68; m++)
				for (int n = 0; n < 60; n++)
					if (A[m][n].group_index == cache[B_set_index].group_index && A[m][n].set_index == B_set_index && !A[m][n].stored)
					{
						finished = false;
						m = 2022;
						n = 2022;
					}
			if (!finished)conflicts++;
		}
	}
	A[i][j].stored = false;
	A[i][j + 1].stored = false;
	A[i][j + 2].stored = false;
	A[i][j + 3].stored = false;
	return conflicts;
}


void print_matrix(int N, int M, Matrix A[256][256])
{
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < M; j++)
			cout << A[i][j].group_index << '-' << A[i][j].set_index << "\t";
		cout << endl;
	}
}


// Your parameters:
int p1 = 24, p2 = 20;
void transpose_submit(int M, int N)
{
	int i, j, k, l;
	if (M == 60 && N == 68)
		for (i = 0; i < 68; i += i % (p1 + p2) ? p2 : p1)
			for (j = 0; j < 60; j += 4)
				for (k = i; k < i + (i % (p1 + p2) ? p2 : p1) && k < 68; k += 2)
				{
					load(k, j, 0);
					load(k, j + 1, 1);
					load(k, j + 2, 2);
					load(k, j + 3, 3);
					load(k + 1, j, 4);
					load(k + 1, j + 1, 5);
					load(k + 1, j + 2, 6);
					load(k + 1, j + 3, 7);
					store(k, j, 0);
					store(k, j + 1, 1);
					store(k, j + 2, 2);
					store(k, j + 3, 3);
					store(k + 1, j, 4);
					store(k + 1, j + 1, 5);
					store(k + 1, j + 2, 6);
					store(k + 1, j + 3, 7);
				}
}


int main()
{
	int M = 60, N = 68;
	init_matrices(M, N);
	init_cache();

	// Parameter auto-search:
	int best_miss_count = 5000, best_p1 = 2, best_p2 = 2;
	for (p1 = 2; p1 < 40; p1 += 2)
		for (p2 = 2; p2 < 40; p2 += 2)
		{
			// remember to add the following two lines:
			clear_cache();
			miss_count = 0;

			transpose_submit(M, N);
			cout << "// Miss count: " << miss_count << endl;
			cout << "// Correctness: " << is_transpose(M, N, A, B) << endl;
			if (miss_count < best_miss_count)
			{
				best_p1 = p1;
				best_p2 = p2;
				best_miss_count = miss_count;
			}
		}
	cout << endl;
	cout << "// Search result:" << endl;
	cout << "// p1 = " << best_p1 << ", p2 = " << best_p2 << endl;
	cout << "// best miss count: " << best_miss_count << endl;
	return 0;
}
