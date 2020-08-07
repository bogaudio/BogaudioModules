#!/usr/bin/env ruby

require 'optparse'

options = {
  type: nil,
  name: nil,
  hp: 3,
  width: 30,
  height: 30,
  noskin: false,
  force: false
}
option_parser = OptionParser.new do |opts|
  opts.banner = "Usage: #{$0} [options] <module slug or widget name, becomes output file prefix>"
  opts.on('--module', 'Creates a module stub; --module or --widget is required') do
    options[:type] = 'module'
  end
  opts.on('--widget', 'Creates a widget stub; --widget or --module is required') do
    options[:type] = 'widget'
  end
  opts.on('--name=[module name]', 'With --module, specifies the module name/title; defaults to upcased slug') do |v|
    options[:name] = v if v
  end
  opts.on('--hp=[module width in HP]', "With --module, sets the module width; default=#{options[:hp]}") do |v|
    options[:hp] = v.to_i if v
  end
  opts.on('--width=[widget width]', "With --widget, sets the widget width; default=#{options[:width]}") do |v|
    options[:width] = v.to_f if v
  end
  opts.on('--height=[widget height]', "With --widget, sets the widget height; default=#{options[:height]}") do |v|
    options[:height] = v.to_f if v
  end
  opts.on('--noskin', 'Mark the module or widget as not skinnable') do
    options[:noskin] = true
  end
  opts.on('-f', '--force', 'Overwrite an existing .svg for the specified slug') do
    options[:force] = true
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

unless ARGV.size >= 1 && (options[:type] == 'module' || options[:type] == 'widget')
  STDERR.puts option_parser.help
  exit 1
end
slug = ARGV[0]
unless slug =~ /^\w+$/
  STDERR.puts "Invalid slug '#{slug}'; the slug must be only alphanumeric characters and underscore."
  exit 1
end
if options[:type] == 'module' && slug !~ /^[A-Z]/
  STDERR.puts "Invalid slug '#{slug}'; a module slug must start with a capital letter"
  exit 1
end
if options[:type] == 'widget' && slug !~ /^[a-z]/
  STDERR.puts "Invalid slug '#{slug}'; a widget slug must start with a lower-case letter"
  exit 1
end
options[:name] = slug.upcase unless options[:name]

out_fn = File.absolute_path(File.join(File.dirname($0), '..', 'res-src', "#{slug}-src.svg"))
if File.exist?(out_fn) && !options[:force]
  STDERR.puts "Output file exists; use -f to overwrite: #{out_fn}"
  exit 1
end

out = nil
if options[:type] == 'module'
  href = '#module'
  case options[:hp]
  when 0..3
    href += '3'
  when 4..5
    href += '5'
  when 6
    href += '6'
  when 7..8
    href += '8'
  end

  out = %Q{<module hp="%d"%s>
  <style/>

  <def xlink:href="%s" var-name="%s"/>
</module>}
  out = out % [options[:hp], options[:noskin] ? ' noskin="true"' : '', href, options[:name]]
elsif options[:type] == 'widget'
  out = %Q{<widget width="%0.1f" height="%0.1f">
  <style/>
</widget>}
  out = out % [options[:width], options[:height]]
end

# puts out
File.write(out_fn, out)
puts "Wrote #{out_fn}"
