#!/usr/bin/env python
#
# check if a file has the proper rapidsvn-dev.el in it
# and adapt the path if necessary
#
# USAGE: check-rapidsvn-el.py [-C] path file1 file2 ... fileN
#
# If the script cannot be found, then the filename is printed to stdout.
# Typical usage:
#    $ find . -name "*.[ch]" | xargs check-rapidsvn-el.py > bad-files
#
# -C switch is used to change / insert the script. Typical usage:
#    $ find . -name "*.[ch]" | xargs check-rapidsvn-el.py -C
#

OLD_TEXT = '''\
/\* -----------------------------------------------------------------
 \* local variables:
 \* eval: \(load-file \"(.*)\"\)
 \* end:
 \*/
'''

import sys
import re

re_OLD = re.compile(OLD_TEXT)

def check_file(fname):
  s = open(fname).read()
  if not re_OLD.search(s):
    print fname

def new_text(path):

  NEW_TEXT='''\
/* -----------------------------------------------------------------
 * local variables:
 * eval: (load-file "''' + path + '''/rapidsvn-dev.el")
 * end:
 */
'''

  return NEW_TEXT

def change_text(fname,path):
  s = open(fname).read()
  m = re_OLD.search(s)
  if not m:
    insert_text(fname, path)
  else:
    s = s[:m.start()] + new_text(path) + s[m.end():]
    open(fname, 'w').write(s)
    print 'Changed:', fname

def insert_text(fname, path):
  s = open(fname).read()
  s = s + new_text(path)
  open(fname, 'w').write(s)
  print 'Inserted:', fname

if __name__ == '__main__':
  if sys.argv[1] == '-C':
    print 'Changing...'
    for f in sys.argv[3:]:
      change_text(f,sys.argv[2])
  else:
    for f in sys.argv[1:]:
      check_file(f)
