# -*- encoding: utf-8 -*-

Gem::Specification.new do |s|
  s.name = %q{sender}
  s.version = "1.5.4"

  s.required_rubygems_version = Gem::Requirement.new(">= 0") if s.respond_to? :required_rubygems_version=
  s.authors = ["Asher"]
  s.date = %q{2010-08-24}
  s.description = %q{Adds :__sender__ and :__caller__ to the built-in :__callee__ and :__method__ methods in Ruby 1.9.1.

Also provides object-oriented :backtrace supporting n-levels backward, :each_backtrace_frame for iteration, :backtrace_includes?, 
and :backtrace_includes_one_of? for context inspection, and :backtrace_frame_with and :backtrace_frames_with, which return
matching frame information for the frame(s) matching the given description.}
  s.email = ["asher@ridiculouspower.com"]
  s.extensions = ["ext/sender/extconf.rb"]
  s.extra_rdoc_files = ["Manifest.txt", "CHANGELOG.rdoc", "README.rdoc", "VERSION.rdoc"]
  s.files = [".autotest", "CHANGELOG.rdoc", "Manifest.txt", "README.rdoc", "Rakefile", "VERSION.rdoc", "ext/sender/RPSender_internal.c", "ext/sender/RPSender_internal.h", "ext/sender/RubySourceSupport.c", "ext/sender/RubySourceSupport.h", "ext/sender/extconf.rb", "ext/sender/rb_Global.c", "ext/sender/rb_Global.h", "ext/sender/rb_Global_internal.h", "ext/sender/rb_Kernel.c", "ext/sender/rb_Kernel.h", "ext/sender/rb_Kernel_internal.h", "ext/sender/sender.c", "lib/VERSION.rdoc", "lib/sender.rb", "lib/sender/sender.bundle", "mkmf.log", "sender.gemspec", "test/test_sender.rb"]
  s.homepage = %q{http://rubygems.org/gems/sender}
  s.rdoc_options = ["--main", "README.rdoc"]
  s.require_paths = ["lib"]
  s.rubyforge_project = %q{asher}
  s.rubygems_version = %q{1.3.7}
  s.summary = %q{Adds :__sender__ and :__caller__ to the built-in :__callee__ and :__method__ methods in Ruby 1.9.1}
  s.test_files = ["test/test_sender.rb"]

  if s.respond_to? :specification_version then
    current_version = Gem::Specification::CURRENT_SPECIFICATION_VERSION
    s.specification_version = 3

    if Gem::Version.new(Gem::VERSION) >= Gem::Version.new('1.2.0') then
      s.add_runtime_dependency(%q<core-source>, [">= 0"])
      s.add_runtime_dependency(%q<mkmfmf>, [">= 0"])
      s.add_development_dependency(%q<rubyforge>, [">= 2.0.4"])
      s.add_development_dependency(%q<rake-compiler>, [">= 0"])
      s.add_development_dependency(%q<hoe>, [">= 2.6.1"])
    else
      s.add_dependency(%q<core-source>, [">= 0"])
      s.add_dependency(%q<mkmfmf>, [">= 0"])
      s.add_dependency(%q<rubyforge>, [">= 2.0.4"])
      s.add_dependency(%q<rake-compiler>, [">= 0"])
      s.add_dependency(%q<hoe>, [">= 2.6.1"])
    end
  else
    s.add_dependency(%q<core-source>, [">= 0"])
    s.add_dependency(%q<mkmfmf>, [">= 0"])
    s.add_dependency(%q<rubyforge>, [">= 2.0.4"])
    s.add_dependency(%q<rake-compiler>, [">= 0"])
    s.add_dependency(%q<hoe>, [">= 2.6.1"])
  end
end
