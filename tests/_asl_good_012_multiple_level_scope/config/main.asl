function int foo(int x = 0)
{
	int z = 4;
	while (x) {
		int z = 12 - x;
		{
			int x = 1;
			int z = x + 1;
		}
		x = x - 1;
	}
}

int z = 6;

foo(z);

