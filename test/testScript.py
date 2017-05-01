#Testing Script for CS475 Project3
#Jesse Thoren
#Description: Compiles/runs project3.cpp with a variety of thread quantities
#   (NUMT), and NUMPAD.

import os
for NUMT in [1, 2, 4, 8]:
    for NUMPAD in range(16):
        cmd = "g++ -DNUMT=%d -DNUMPAD=%d -DNUMTRIES=1 project3.cpp -o runProg -lm -fopenmp" % (NUMT, NUMPAD)
        os.system( cmd )
        cmd = "./runProg"
        os.system( cmd )
cmd = "rm -f runProg"
os.system( cmd )
