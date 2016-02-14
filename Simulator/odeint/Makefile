CC = g++
CFLAGS = -w -O2

bruss=bruss/bruss
sys7d=sys7d/sys7d
sys7d_tdot=sys7d_tdot/sys7d_tdot

.PHONY: all
all: bruss sys7d sys7d_tdot

.PHONY: bruss
bruss:
	$(CC) $(CFLAGS) $(bruss).cpp -o $(bruss)

.PHONY: sys7d
sys7d:
	$(CC) $(CFLAGS) $(sys7d).cpp -o $(sys7d)

.PHONY: sys7d_tdot
sys7d_tdot:
	$(CC) $(CFLAGS) $(sys7d_tdot).cpp -o $(sys7d_tdot)
