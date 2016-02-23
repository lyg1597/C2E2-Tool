CC = g++
CFLAGS = -w -O2
CAPD=`~/C2E2/capd/bin/capd-config --cflags --libs`

bruss=bruss/bruss
sys7d=sys7d/sys7d
power_train=power_train/power_train
totalmotion40s=totalmotion40s/totalmotion40s

.PHONY: all
all: bruss sys7d power_train

.PHONY: bruss
bruss:
	$(CC) $(CFLAGS) $(bruss).cpp $(CAPD) -o $(bruss)

.PHONY: sys7d
sys7d:
	$(CC) $(CFLAGS) $(sys7d).cpp $(CAPD) -o $(sys7d)

.PHONY: power_train
power_train:
	$(CC) $(CFLAGS) $(power_train).cpp $(CAPD) -o $(power_train)
