CC = g++
CFLAGS = -g -Wall -O2 -std=c++11

bruss=bruss/bruss
sys7d=sys7d/sys7d
sys7d_tdot=sys7d_tdot/sys7d_tdot
power_train=power_train/power_train
reversionary=reversionary/reversionary
const=const/const

.PHONY: all
all: bruss sys7d sys7d_tdot power_train

.PHONY: bruss
bruss:
	$(CC) $(CFLAGS) $(bruss).cpp -o $(bruss)

.PHONY: sys7d
sys7d:
	$(CC) $(CFLAGS) $(sys7d).cpp -o $(sys7d)

.PHONY: sys7d_tdot
sys7d_tdot:
	$(CC) $(CFLAGS) $(sys7d_tdot).cpp -o $(sys7d_tdot)

.PHONY: power_train
power_train:
	$(CC) $(CFLAGS) $(power_train).cpp -o $(power_train)

.PHONY: reversionary
reversionary:
	$(CC) $(CFLAGS) $(reversionary).cpp -o $(reversionary)

.PHONY: const
const:
	$(CC) $(CFLAGS) $(const).cpp -o $(const)
