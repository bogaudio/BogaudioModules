#!/usr/bin/env ruby

require 'optparse'

options = {
  skins: false
}
option_parser = OptionParser.new do |opts|
  opts.banner = "Usage: #{$0} [options] [module slug, or any module .svg or source file]"
  opts.on('--skins', 'Open the skin res-pp/ files along with everything else') do
    options[:skins] = true
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
open('res-pp', "#{slug}-pp.svg")
if options[:skins]
  Dir.glob(File.absolute_path(File.join(File.dirname($0), '..', 'res-pp', "#{slug}-*-pp.svg"))).each do |fn|
    open('res-pp', File.basename(fn))
  end
end
open('src', "#{slug}.hpp")
open('src', "#{slug}.cpp")
