require 'hoe'
require 'rake/extensiontask'

Hoe.spec 'sender' do
  developer( 'Asher', 'asher@ridiculouspower.com' )
  self.rubyforge_name   = 'asher'
  self.version          = File.open( 'VERSION.rdoc' ).readline
  self.readme_file			= 'README.rdoc'
  self.history_file     = 'CHANGELOG.rdoc'
  self.extra_rdoc_files	= FileList['*.rdoc']
  self.spec_extras			= { :extensions => ["ext/sender/extconf.rb"] }
  self.extra_dev_deps << ['rake-compiler', '>= 0']  
  self.extra_deps << ['core-source', '>= 0'] << ['mkmfmf', '>= 0']
  Rake::ExtensionTask.new( 'sender', spec ) do |ext|
    ext.lib_dir = File.join('lib', 'sender')
  end	
end

Rake::Task[:test].prerequisites << :compile

task :cultivate do
  system "touch Manifest.txt; rake check_manifest | grep -v \"(in \" | patch"
  system "rake debug_gem | grep -v \"(in \" > sender.gemspec"
end

