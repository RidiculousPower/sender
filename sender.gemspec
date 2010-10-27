
Gem::Specification.new do |spec|

  spec.name                      =  'sender'
  spec.rubyforge_project         =  'sender'
  spec.version                   =  '1.5.5'

  spec.summary                   =  "Adds :__sender__ and :__caller__ to the built-in :__callee__ and :__method__ methods in Ruby 1.9.1."
  spec.description               =  "Also provides object-oriented :backtrace supporting n-levels backward, :each_backtrace_frame for iteration, :backtrace_includes?, and :backtrace_includes_one_of? for context inspection, and :backtrace_frame_with and :backtrace_frames_with, which return matching frame information for the frame(s) matching the given description."
  
  spec.authors                   =  [ 'Asher' ]
  spec.email                     =  'asher@ridiculouspower.com'
  spec.homepage                  =  'http://rubygems.org/gems/sender'
  
  spec.add_dependency(              'core-source', '>= 0.1' )

  spec.date                      =  Date.today.to_s
  
  # ensure the gem is built out of versioned files
  spec.files                     = Dir[ '{bin,lib,man,test,spec}/**/*',
                                        'Rakefile', 
                                        'README*', 
                                        'LICENSE*'] & `git ls-files -z`.split("\0")

end
