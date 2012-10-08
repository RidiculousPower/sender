require 'date'

Gem::Specification.new do |spec|

  spec.name                      =  'sender'
  spec.rubyforge_project         =  'sender'
  spec.version                   =  '1.5.10'

  spec.summary                   =  "Adds :__sender__ and :__caller__ to the built-in :__callee__ and :__method__ methods in Ruby 1.9."
  spec.description               =  "Adds :__sender__ and :__caller__ to the built-in :__callee__ and :__method__ methods in Ruby 1.9. Also provides object-oriented :backtrace supporting n-levels backward, :each_backtrace_frame for iteration, :backtrace_includes?, and :backtrace_includes_one_of? for context inspection, and :backtrace_frame_with and :backtrace_frames_with, which return matching frame information for the frame(s) matching the given description."
  
  spec.authors                   =  [ 'Asher' ]
  spec.email                     =  'asher@ridiculouspower.com'
  spec.homepage                  =  'http://rubygems.org/gems/sender'

  spec.required_ruby_version     = ">= 1.9.1"
  
  spec.add_dependency               'core-source'
  spec.add_dependency               'mkmfmf'
  spec.add_dependency               'rake-compiler'

  spec.date                      =  Date.today.to_s
  
  spec.extensions                << 'ext/sender/extconf.rb'
  
  # ensure the gem is built out of versioned files
  # also make sure we include the bundle since we exclude it from git storage
  spec.files                     = Dir[ '{lib,spec,ext,test}/**/*',
                                        'README*', 
                                        'LICENSE*' ] & `git ls-files -z`.split("\0")

end
