#!/usr/bin/ruby

INKSCAPE = '/Applications/Inkscape.app/Contents/MacOS/inkscape'
TMP_FILE = '/tmp/svg_render_tmp.svg'

require 'nokogiri'

if ARGV.empty?
  STDERR.puts "Usage: #{$0} <res-pp/MODULE-pp.svg>"
  exit 1
end

svg_file = ARGV[0]
unless File.readable?(svg_file)
  STDERR.puts "No such file: #{svg_file}"
  exit 1
end

name = nil
if File.basename(svg_file) =~ /^(.*)-pp.svg$/
  name = $1
else
  STDERR.puts "Input file must be named *-pp.svg"
  exit 1
end
dir = File.dirname(svg_file)

def read_xml(fn)
  Nokogiri::XML(File.open(fn)) do |config|
    config.norecover.strict
  end
end

Dir.glob(File.join(File.dirname(svg_file), "#{name}*-pp.svg")).each do |fn|
  puts "Processing #{fn}..."
  doc = read_xml(fn)

  doc.css('use').each do |n|
    id = n.attribute('id')
    n.remove if id && id.to_s =~ /_(PARAM|INPUT|OUTPUT|LIGHT|WIDGET)$/
  end
  File.write(TMP_FILE, doc.to_xml)

  out = `#{INKSCAPE} -g --actions='EditSelectAll;SelectionUnGroup;EditSelectAll;EditUnlinkClone;EditSelectAll;ObjectToPath;FileSave;FileQuit' '#{TMP_FILE}' 2>&1`
  unless $?.success?
    STDERR.puts "Calling Inkscape failed:\n#{out}"
    exit 1
  end

  doc = read_xml(TMP_FILE)
  doc.css('style').each(&:remove)

  out_file = File.join(dir.sub(/-pp\/?$/, ''), File.basename(fn).sub(/-pp.svg$/, '.svg'))
  File.write(out_file, doc.to_xml)
  File.unlink(TMP_FILE)
  puts "Wrote #{out_file}"
end
