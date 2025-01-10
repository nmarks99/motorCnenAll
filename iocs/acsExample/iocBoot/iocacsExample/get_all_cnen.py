#!/usr/bin/env python3
from epics import caget

for i in range(1,9):
    print(caget(f"acsExample:m{i}.CNEN"))
