#!/usr/bin/ruby

INKSCAPE = '/Applications/Inkscape.app/Contents/MacOS/inkscape'

require 'nokogiri'

def read_xml(fn)
  Nokogiri::XML(File.open(fn)) do |config|
    config.norecover.strict
  end
end

if ARGV.empty?
  STDERR.puts "Usage: #{$0} <res-pp/MODULE-pp.svg> [...more files...]"
  exit 1
end

svg_filenames = {}
svg_files = []
ARGV.each do |file|
  unless File.readable?(file)
    STDERR.puts "No such file: #{file}"
    exit 1
  end

  name = nil
  if File.basename(file) =~ /^(.*)-pp.svg$/
    name = $1
  else
    STDERR.puts "Input file #{file} must be named *-pp.svg"
    exit 1
  end

  Dir.glob(File.join(File.dirname(file), "#{name}*-pp.svg")).each do |fn|
    unless svg_filenames.key?(fn)
      svg_filenames[fn] = true

      puts "Preparing #{fn}..."
      doc = read_xml(fn)
      doc.css('use, svg svg').each do |n|
        id = n.attribute('id')
        n.remove if id && id.to_s =~ /_(PARAM|INPUT|OUTPUT|LIGHT|WIDGET)$/
      end
      tmp_name = File.join('/tmp', "svg_render_#{File.basename(fn)}")
      File.write(tmp_name, doc.to_xml)

      svg_files << {
        name: fn,
        out_name: File.join(File.dirname(fn).sub(/-pp\/?$/, ''), File.basename(fn).sub(/-pp.svg$/, '.svg')),
        tmp_name: tmp_name
      }
    end
  end
end

puts 'Calling Inkscape...'
files = svg_files.map { |f| "'#{f[:tmp_name]}'" }.join(' ')
out = `#{INKSCAPE} -g --batch-process --actions='EditSelectAll;SelectionUnGroup;EditSelectAll;EditUnlinkClone;EditSelectAll;ObjectToPath;FileSave' #{files} 2>&1`
unless $?.success?
  STDERR.puts "Calling Inkscape failed:\n#{out}"
  exit 1
end

puts 'Cleaning up...'
svg_files.each do |sf|
  fn = sf[:tmp_name]
  doc = read_xml(fn)
  doc.css('style').each(&:remove)
  File.write(sf[:out_name], doc.to_xml)
  File.unlink(fn)
end
