from epics import caput

prefix = "acsExample:"

for i in range(1,17):
    if i % 2:
        caput(f"{prefix}m{i}.CNEN", 1)


