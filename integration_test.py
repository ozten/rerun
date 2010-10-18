#!/usr/bin/env python2.6
import os
import re
import select
import signal
from subprocess import Popen, PIPE, STDOUT
import sys
import time


debug = False
error = False

def rerun_once():
    # TODO: valgrind captures all signals, so we can't SIGINT it...
    # so we use the --once flag to get rerun to exit

    cmd = "valgrind ./rerun --once . '*.js' 'echo \"A JavaScript File was modifed\"'"
    #cmd = "valgrind ./rerun . '*.js' 'echo \"A JavaScript File was modifed\"'"

    # OTOH rerun can use sigint to exit, so we could use this for other
    # integration tests
    #cmd = "./rerun . '*.js' 'echo \"A JavaScript File was modifed\"'"

    proc = Popen(cmd, shell=True, stdin=PIPE, stdout=PIPE, stderr=PIPE)

    run = True
    i=0
    output = ""
    while run:
        if True:
            # Read output or wait for up to 1 second and then trigger test code during inactivity
            (ins, outs, errs) = select.select([proc.stdout, proc.stderr], [], [], 1)
        #print("SELECTED %d %s %s %s" % (i, str(ins), str(_), str(errs)))
        if [] == ins and [] == outs and [] == errs:
            if debug:
                print "Wow, nothing doing %d, let's test some stuff" % i

            # First inactivity, touch .js
            if 0 == i:
                foo = open('foo.js', 'w')
                foo.write('test')
                foo.close()
    
            # Nothing left to test, end it
            if 5 == i:
                #proc.terminate()
                #os.kill(proc.pid, signal.SIGINT)
                run = False
            
            i += 1
        if errs:
            print("Error: %s" % str(errs))
            run = False
        else:
            if ins:
                index = 0
                # TODO, we should look at pos for stdout vs stderr
                for inp in ins:
                    o = inp.readline()
                    if o:
                        output += o
                    else:
                        run = False #EOF
                    index += 1
    return output

def valgrind_format_okay(actual_lines):
    return (re.match('\s*==\d*==\s*ERROR SUMMARY: .*', actual_lines[-2]) and
            re.match('\s*==\d*==\s*For counts of detected and suppressed errors, rerun with.*', actual_lines[-3]) and
            re.match('\s*==\d*==\s*HEAP SUMMARY:\s*', actual_lines[-9]))

actual = rerun_once()
actual_lines = actual.split('\n')

if valgrind_format_okay(actual_lines):
    if not re.match('\s*==\d*==\s*All heap blocks were freed -- no leaks are possible', actual_lines[-5]):
        error = True
        print("ERROR: We have a memory leak")
        print(actual_lines[-5])
else:
    error = True
    print(actual)
    print("ERROR: valgrind summary had unexpected format. ^^^")

if not error:
    print("OK")
# TODO figure out Popen and child exit code, add test
#if proc.returncode:
#    print("problem with Return code is %d" % proc.returncode)
