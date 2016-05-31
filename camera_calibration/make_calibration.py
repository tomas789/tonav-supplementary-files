#!env python3.5

import os
import sys

def main():
  if len(sys.argv) != 2:
    print("Usage: make_calibration.py <dir>")
    return

  dirname = sys.argv[1]
  dircontent = [f for f in os.listdir(dirname) if f[-4:] == '.png']
  with open('dataset.xml', 'w') as o:
    o.write('<?xml version="1.0"?>\n')
    o.write('<opencv_storage>\n')
    o.write('<images>\n')
    for fname in dircontent:
      o.write("{}/{}\n".format(dirname, fname))
    o.write('</images>\n')
    o.write('</opencv_storage>\n')

if __name__ == '__main__':
  main()
