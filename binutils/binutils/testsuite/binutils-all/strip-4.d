#PROG: strip
#source: group-2.s
#readelf: -Sg --wide
#name: strip with section group 4

#...
  \[[ 0-9]+\] \.text.*[ \t]+PROGBITS[ \t0-9a-f]+AX[ \t]+.*
#...
  \[[ 0-9]+\] \.data.*[ \t]+PROGBITS[ \t0-9a-f]+WA[ \t]+.*
#...
There are no section groups in this file.
