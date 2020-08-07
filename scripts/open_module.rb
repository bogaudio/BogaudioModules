#!/usr/bin/env ruby

require 'optparse'

options = {
  skin: [],
  skins_only: false
}
option_parser = OptionParser.new do |opts|
  opts.banner = "Usage: #{$0} [options] [module slug, or any module .svg or source file]"
  opts.on('--skin=[skin name]', 'Open the res-pp/ file for the given skin along with everything else') do |v|
    options[:skin] << v if v
  end
  opts.on('-o', '--skins_only', 'Only open specified skins (not default)') do
    options[:skins_only] = true
  end
  opts.on_tail('-h', '--help', 'Show this message') do
    puts opts
    exit
  end
end
begin
  option_parser.parse!
rescue => e
  STDERR.puts e.to_s
  STDERR.puts "\n"
  STDERR.puts option_parser.help
  exit 1
end

unless ARGV.size >= 1
  STDERR.puts option_parser.help
  exit 1
end
slug = ARGV[0]
unless slug =~ /^\w+$/
  if File.basename(slug) =~ /^(\w+)(-[\w-]+)?\.(svg|hpp|cpp)$/
    slug = $1
  else
    STDERR.puts "Invalid module slug or source file: #{slug}"
    exit
  end
end

def open(d, f)
  afn = File.absolute_path(File.join(File.dirname($0), '..', d, f))
  if File.readable?(afn)
    puts "Open #{File.join(d, f)}..."
  end

  out = `open #{afn}`
  unless $?.success?
    STDERR.puts "open failed:\n#{out}"
    exit 1
  end
end

open('res-src', "#{slug}-src.svg")
unless options[:skins_only]
  open('res-pp', "#{slug}-pp.svg")
end
options[:skin].each do |skin|
  open('res-pp', "#{slug}-#{skin}-pp.svg")
end
open('src', "#{slug}.hpp")
open('src', "#{slug}.cpp")
