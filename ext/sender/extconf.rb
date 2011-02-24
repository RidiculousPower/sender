#!/usr/bin/env ruby

require 'mkmfmf'
require 'core-source'

target  = "sender"
  
required_core_headers    = [  "vm_core.h",
                              "iseq.h",
                              "eval_intern.h",
                              "version.h" ]

default_cflags  = "-march=x86-64 -g -fno-common -fsigned-char -pipe"

# Create our makefile from sources
if ensure_core_headers( required_core_headers )
  with_cflags( default_cflags ) do
    create_makefile( target )
   end
end
