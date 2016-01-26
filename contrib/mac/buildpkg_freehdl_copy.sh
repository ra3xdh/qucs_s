#!/bin/sh
#
# Prepare frehdl packaging.
# Copy all freehdl installed binaries to the 'payload_freehdl' directory
#
# It should from the final prefix '/usr/local',
# that is to avoid problems with harcode paths (created during configure).
#
# I got this list by installing freehdl into an exclusive prefix '~/myFreeHDL'
# and running 'find .'
#
# TODO automate this process of findind dirs and files
#
# This script is also not pretty, other ideas?

payload="payload_freehdl"

mkdir ${payload}

dirs=(
./bin
./include
./include/freehdl
./lib
./lib/freehdl
./lib/pkgconfig
./share
./share/freehdl
./share/freehdl/lib
./share/freehdl/lib/ieee
./share/freehdl/lib/std
./share/info
./share/info/dir
./share/man
./share/man/man1
./share/man/man5
)

# create subdirs on payload
for dir in "${dirs[@]}"; do
  mkdir ./${payload}/${dir}
done

files=(
./bin/freehdl-config
./bin/freehdl-gennodes
./bin/freehdl-v2cc
./bin/gvhdl
./include/freehdl/cdfggen-chunk.h
./include/freehdl/cdfggen-chunk.t
./include/freehdl/fire-chunk.h
./include/freehdl/fire-chunk.t
./include/freehdl/fire-types.h
./include/freehdl/fire.h
./include/freehdl/FlexLexer.h
./include/freehdl/kernel-acl.hh
./include/freehdl/kernel-attributes.hh
./include/freehdl/kernel-classes.hh
./include/freehdl/kernel-db.hh
./include/freehdl/kernel-delayed-procedure-base.hh
./include/freehdl/kernel-driver-info.hh
./include/freehdl/kernel-dump.hh
./include/freehdl/kernel-error.hh
./include/freehdl/kernel-fhdl-stream.hh
./include/freehdl/kernel-flags.hh
./include/freehdl/kernel-global-event-queue.hh
./include/freehdl/kernel-handle-info.hh
./include/freehdl/kernel-handle.hh
./include/freehdl/kernel-kernel-class.hh
./include/freehdl/kernel-list.hh
./include/freehdl/kernel-map-list.hh
./include/freehdl/kernel-name-stack.hh
./include/freehdl/kernel-pcounter.hh
./include/freehdl/kernel-persistent-cdfg-dump.hh
./include/freehdl/kernel-persistent-dump.hh
./include/freehdl/kernel-process-base.hh
./include/freehdl/kernel-reader-info.hh
./include/freehdl/kernel-register.hh
./include/freehdl/kernel-resolver-descriptor.hh
./include/freehdl/kernel-resolver-process.hh
./include/freehdl/kernel-sig-info.hh
./include/freehdl/kernel-sigacl-list.hh
./include/freehdl/kernel-signal-source-list-array.hh
./include/freehdl/kernel-source-descriptor.hh
./include/freehdl/kernel-stack-trace.hh
./include/freehdl/kernel-util.hh
./include/freehdl/kernel-wait-info.hh
./include/freehdl/kernel-winfo-item.hh
./include/freehdl/kernel-Xinfo-data-descriptor.hh
./include/freehdl/kernel-Xinfo-kind.hh
./include/freehdl/kernel.h
./include/freehdl/std-memory.hh
./include/freehdl/std-standard.hh
./include/freehdl/std-vhdl-types.hh
./include/freehdl/std.h
./include/freehdl/tree-supp.h
./include/freehdl/vaul-chunk.h
./include/freehdl/vaul-chunk.t
./include/freehdl/vaul-creator-base.h
./include/freehdl/vaul-dfile.h
./include/freehdl/vaul-dunit.h
./include/freehdl/vaul-dynarray.h
./include/freehdl/vaul-errors.h
./include/freehdl/vaul-lexer.h
./include/freehdl/vaul-list.h
./include/freehdl/vaul-mempool.h
./include/freehdl/vaul-parser.h
./include/freehdl/vaul-pool.h
./include/freehdl/vaul-printer.h
./include/freehdl/vaul-ref.h
./include/freehdl/vaul-types.h
./include/freehdl/vaul-util.h
./include/freehdl/vaul.h
./lib/freehdl/libieee.0.dylib
./lib/freehdl/libieee.a
./lib/freehdl/libieee.dylib
./lib/freehdl/libieee.la
./lib/libfreehdl-cdfggen.0.dylib
./lib/libfreehdl-cdfggen.a
./lib/libfreehdl-cdfggen.dylib
./lib/libfreehdl-cdfggen.la
./lib/libfreehdl-fire.0.dylib
./lib/libfreehdl-fire.a
./lib/libfreehdl-fire.dylib
./lib/libfreehdl-fire.la
./lib/libfreehdl-kernel.0.dylib
./lib/libfreehdl-kernel.a
./lib/libfreehdl-kernel.dylib
./lib/libfreehdl-kernel.la
./lib/libfreehdl-std.0.dylib
./lib/libfreehdl-std.a
./lib/libfreehdl-std.dylib
./lib/libfreehdl-std.la
./lib/libfreehdl-vaul.0.dylib
./lib/libfreehdl-vaul.a
./lib/libfreehdl-vaul.dylib
./lib/libfreehdl-vaul.la
./lib/pkgconfig/freehdl.pc
./share/freehdl/lib/ieee/math_real.vhdl
./share/freehdl/lib/ieee/numeric_bit.vhdl
./share/freehdl/lib/ieee/numeric_std.vhdl
./share/freehdl/lib/ieee/std_logic_1164.vhdl
./share/freehdl/lib/ieee/std_logic_arith.vhdl
./share/freehdl/lib/ieee/std_logic_signed.vhdl
./share/freehdl/lib/ieee/std_logic_unsigned.vhdl
./share/freehdl/lib/ieee/vital_timing.vhdl
./share/freehdl/lib/std/standard.vhdl
./share/freehdl/lib/std/textio.vhdl
./share/info/fire.info
./share/man/man1/freehdl-config.1
./share/man/man1/freehdl-gennodes.1
./share/man/man1/freehdl-v2cc.1
./share/man/man1/gvhdl.1
./share/man/man5/v2cc.libs.5
)

# copy files from prefix to payload
for file in "${files[@]}"; do
  cp /usr/local/${file} ./${payload}/${file}
done

# strip extended attributes
# otherwise a bunch of empy files starting with '._'
# are added to the list of files
xattr -c `find ${payload}`
