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
    num_tests = 2
    print("1..%d" % num_tests)
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
    blocks_free_line = None
    saw_summary = saw_error_counts = saw_heap_summary = False
    for line in actual_lines[-12:]:
        if not saw_summary and \
               re.match('\s*==\d*==\s*ERROR SUMMARY: .*', line):
            saw_summary = True
        elif not saw_error_counts and \
                 re.match('\s*==\d*==\s*For counts of detected and suppressed errors, rerun with.*', line):
            saw_error_counts = True
        elif not saw_heap_summary and \
                 re.match('\s*==\d*==\s*HEAP SUMMARY:\s*', line):
            saw_heap_summary = True

    return saw_summary and saw_error_counts and saw_heap_summary

actual = rerun_once()
actual_lines = actual.split('\n')

test1 = "1 - valgrind summary is in expected format"
if valgrind_format_okay(actual_lines):
    print("ok %s" % test1)
    summaries = actual_lines[-7:-4]
    no_leaks = False
    for line in summaries:
        if re.match('\s*==\d*==\s*All heap blocks were freed -- no leaks are possible', line):
            no_leaks = True
            break
    test2 = "2 - We should not leak memory"
    if no_leaks:
        print("ok %s" % test2)
    else:
        print("not ok %s" % test2)
        print("\n".join(["\t%s" % s for s in summaries]))
else:
    print("not ok %s" % test1)
    error = True
    print("\n".join(["\t%s" % s for s in actual_lines[-20:]]))

# TODO ./rerun SegFaults... write a test, it should print usage instead

# TODO figure out Popen and child exit code, add test
#if proc.returncode:
#    print("problem with Return code is %d" % proc.returncode)
