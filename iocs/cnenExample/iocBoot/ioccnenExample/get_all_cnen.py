#!/usr/bin/env python3
from epics import caget

for i in range(1,17):
    print(caget(f"cnenExample:m{i}.CNEN"))
