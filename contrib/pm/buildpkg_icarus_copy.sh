#!/bin/sh
#
# Prepare icarus-verilog for packaging.
# Copy all icarus installed binaries to the 'payload_icarus' directory
#
# It should from the final prefix '/usr/local',
# that is to avoid problems with harcode paths (created during configure).
# ex. vvp cannot fint its libraries.
#     It coupld be provided as $vv[ -M /path/to/lib (not nice)
#
# I got this list by installing icarus into an exclusive prefix '~/myIcarus'
# and running 'find .'
#
# TODO automate this process of findind dirs and files
#
# This script is also not pretty, other ideas?

payload="payload_icarus"

mkdir ${payload}

dirs=(
./bin
./include
./include/iverilog
./lib
./lib/ivl
./lib/ivl/include
./share
./share/man
./share/man/man1
)

# create subdirs on payload
for dir in "${dirs[@]}"; do
  mkdir ./${payload}/${dir}
done

files=(
./bin/iverilog
./bin/iverilog-vpi
./bin/vvp
./include/iverilog/_pli_types.h
./include/iverilog/acc_user.h
./include/iverilog/ivl_target.h
./include/iverilog/veriuser.h
./include/iverilog/vpi_user.h
./lib/ivl/cadpli.vpl
./lib/ivl/include/constants.vams
./lib/ivl/include/disciplines.vams
./lib/ivl/ivl
./lib/ivl/ivlpp
./lib/ivl/null-s.conf
./lib/ivl/null.conf
./lib/ivl/null.tgt
./lib/ivl/stub-s.conf
./lib/ivl/stub.conf
./lib/ivl/stub.tgt
./lib/ivl/system.sft
./lib/ivl/system.vpi
./lib/ivl/v2005_math.sft
./lib/ivl/v2005_math.vpi
./lib/ivl/va_math.sft
./lib/ivl/va_math.vpi
./lib/ivl/vhdl-s.conf
./lib/ivl/vhdl.conf
./lib/ivl/vhdl.tgt
./lib/ivl/vvp-s.conf
./lib/ivl/vvp.conf
./lib/ivl/vvp.tgt
./lib/libveriuser.a
./lib/libvpi.a
./share/man/man1/iverilog-vpi.1
./share/man/man1/iverilog.1
./share/man/man1/vvp.1
)

# copy files from prefix to payload
for file in "${files[@]}"; do
  cp /usr/local/${file} ./${payload}/${file}
done
