#!/usr/bin/env ruby

require 'listen'
require 'nokogiri'
require 'optparse'

options = {
  reprocess: false,
  listen: false,
  module_prefixes: [],
  debug: false
}
option_parser = OptionParser.new do |opts|
  opts.banner = "Usage: #{$0} [options] [res-src/MODULE-src.svg]"
  opts.on('--reprocess', 'Process everything immediately, if no file specfied') do
    options[:reprocess] = true
  end
  opts.on('--listen', 'Listen for changes, if no file specfied; may be used with --reprocess') do
    options[:listen] = true
  end
  opts.on('--filter=[module prefix]', 'Limit --reprocess or --listen to modules matching prefix; may be used multiply') do |v|
    options[:module_prefixes] << v if v
  end
  opts.on('--debug', 'Print directory and file status information and exit') do
    options[:debug] = true
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

def parse_xml(s)
  Nokogiri::XML(s) do |config|
    config.norecover.strict
  end
end

def read_xml(fn)
  parse_xml(File.open(fn))
end

$src_dir = nil
$pp_dir = nil
def load_directories()
  unless $src_dir
    $src_dir = File.absolute_path(File.join(File.dirname($0), '..', 'res-src'))
    unless Dir.exists?($src_dir)
      STDERR.puts "Source directory doesn't exist: #{$src_dir}"
      exit 1
    end
  end

  unless $pp_dir
    $pp_dir = File.absolute_path(File.join(File.dirname($0), '..', 'res-pp'))
    unless Dir.exists?($pp_dir)
      STDERR.puts "Preprocess directory doesn't exist: #{$pp_dir}"
      exit 1
    end
  end
end

def main_styles_name()
  load_directories()
  File.join($src_dir, 'styles.css')
end

$styles_loaded = false
$main_styles = nil
$skins = {}
def load_styles(force = false)
  if force
    $styles_loaded = false
    $main_styles = nil
    $skins = {}
  end

  unless $styles_loaded
    $styles_loaded = true

    fn = main_styles_name()
    if File.readable?(fn)
      $main_styles = File.read(fn)
    end

    load_directories()
    Dir.glob(File.join($src_dir, 'skin-*.css')).each do |fn|
      m = File.basename(fn).match(/^skin-(.*)\.css/)
      $skins[m[1]] = [fn, File.read(fn)]
    end
  end
end

def defs_name()
  load_directories()
  File.join($src_dir, 'defs.svg')
end

$defs_loaded = false
$defs = {}
def load_defs(force = false)
  if force
    $defs_loaded = false
    $defs = {}
  end

  unless $defs_loaded
    $defs_loaded = true

    fn = defs_name()
    if File.readable?(fn)
      doc = read_xml(fn)
      doc.css('defs symbol').each do |n|
        id = n['id'].to_s
        unless id && !id.empty?
          STDERR.puts "Symbol in #{fn} with missing ID: #{n.to_s}"
          exit 1
        end
        $defs[id] = n
      end
    end
  end
end

def widget_from_filename(fn)
  File.basename(fn).sub(/^(.*)-src\.svg$/, '\1')
end

$module_names_loaded = false
$module_names = []
def load_module_names(force = false)
  if force
    $module_names_loaded = false
    $module_names = []
  end

  unless $module_names_loaded
    $module_names_loaded = true

    load_directories()
    $module_names = Dir.glob(File.join($src_dir, '*-src.svg')).map do |fn|
      widget_from_filename(fn)
    end.select do |name| # FIXME: figure out what to do about non-module widget sources.
      name =~ /^[A-Z]/
    end.sort
  end
end

def debug()
  puts

  load_directories()
  puts "Source directory: #{$src_dir}"
  puts "Preprocess directory: #{$pp_dir}"
  puts

  load_styles()
  puts "Main stylesheet (#{main_styles_name()}): #{$main_styles ? "#{$main_styles.size} bytes" : 'missing'}"
  if $skins.empty?
    puts "Skins: none"
  else
    $skins.keys.sort.each do |skin_name|
      skin = $skins[skin_name]
      puts "  - #{skin_name} (#{skin[0]}): #{skin[1].size} bytes"
    end
  end
  puts

  load_defs()
  if $defs.empty?
    puts "Defs (#{defs_name()}): missing"
  else
    puts "Defs (#{defs_name()}):"
    $defs.keys.sort.each do |def_id|
      puts "  - #{def_id}"
    end
  end
  puts

  load_module_names()
  puts "Modules:"
  $module_names.each do |name|
    puts "  - #{name}"
  end
  puts
end

def write_output(name, doc, styles)
  fn = File.join($pp_dir, "#{name}-pp.svg")
  doc.css('style').each do |n|
    n.content = styles
  end
  File.write(fn, doc.to_xml)
  puts "Wrote #{fn}"
end

def process_def(doc, n)
  id = n.attribute('href').to_s
  if id
    id.sub!(/^#/, '')
    d = $defs[id]
    if d
      nn = d.dup
      nn.node_name = 'svg'
      nn.delete('viewBox')
      nn.delete('id')

      g = Nokogiri::XML::Node.new('g', doc)
      g.add_child(nn)
      n.replace(g)

      n.each do |k, v|
        case k
        when 'id'
          if !v.to_s.empty?
            nn['id'] = v
          end
        when 'transform'
          if !v.to_s.empty?
            g['transform'] = v
          end
        when /^var-(\w+)$/
          g[k] = v
        end
      end

      nn.css('def').each do |dn|
        process_def(doc, dn)
      end
    else
      puts "WARN: no def defined for def ID '#{id}' in #{fn}"
      n.remove
    end
  else
    puts "WARN: def without ID in #{fn}: #{n.to_s}"
    n.remove
  end
end

def variable_value(k, vars, defaults)
  v = nil
  vars.reverse_each do |vs|
    if vs.key?(k)
      v = vs[k]
      break
    end
  end

  if v.nil?
    if defaults.key?(k)
      v = defaults[k]
    else
      v = 0.0
    end
  end

  v
end

def eval_expressions(s, vars, defaults)
  s.gsub!(/\$\{(\w+)\}/) do
    variable_value($1, vars, defaults)
  end

  s.gsub!(/\$(\w+)/) do
    variable_value($1, vars, defaults)
  end

  if s =~ /[\+\-\*\/]/
    s.gsub!(/\d+(\.\d+)?([\+\-\*\/]\d+(\.\d+)?)*/) do |m|
      eval(m)
    end
  end

  s
end

def process_variables(n, vars)
  default_vars = {}
  n.each do |k, v|
    if k =~ /^default-(\w+)$/
      default_vars[$1] = v
      n.delete(k)
    end
  end

  n.each do |k, v|
    n[k] = eval_expressions(v.to_s, vars, default_vars)
  end

  pop = false
  case n.node_name
  when 'g'
    vs = {}
    n.each do |k, v|
      if k =~ /^var-(\w+)$/
        vs[$1] = v
        n.delete(k)
      end
    end
    vars.push(vs)
    pop = true
  when 'text'
    n.traverse do |t|
      if t.text?
        t.content = eval_expressions(t.content, vars, default_vars)
      end
    end
  end

  n.elements.each do |nn|
    process_variables(nn, vars)
  end
  vars.pop if pop
end

def process(name)
  load_directories()
  load_styles()
  load_defs()

  fn = File.join($src_dir, File.basename(name))
  unless File.readable?(fn)
      STDERR.puts "No such file: #{fn}"
      exit 1
  end

  doc = read_xml(fn)
  hp = 3

  root = doc.at_css(':root')
  if root.node_name == 'module'
    if root['hp'] && !root['hp'].to_s.empty?
      hp = root['hp'].to_s.to_i
    end
    root.node_name = 'svg'
    root['xmlns'] = 'http://www.w3.org/2000/svg'
    root['xmlns:xlink'] = 'http://www.w3.org/1999/xlink'
    root['version'] = '1.1'
    root['width'] = (hp * 15).to_s
    root['height'] = '380'
    root['viewBox'] = "0 0 #{root['width']} #{root['height']}"
    root.delete('hp')

    doc = parse_xml(doc.to_xml)
  end

  doc.css('defs import').each do |n|
    id = n.attribute('id').to_s
    if id
      d = $defs[id]
       if d
        n.replace(d)
      else
        puts "WARN: no def for import ID '#{id}' in #{fn}"
        n.remove
      end
    else
      puts "WARN: import without ID in #{fn}: #{n.to_s}"
      n.remove
    end
  end

  doc.css('def').each do |n|
    process_def(doc, n)
  end

  doc.xpath('//comment()').each(&:remove)

  globals = {}
  globals['hp'] = hp
  globals['width'] = hp * 15.0
  globals['height'] = 380.0
  vars = [globals]
  process_variables(doc.at_css(':root'), vars)

  doc.css('g').each do |n|
    n.replace(n.children) if n.keys.empty?
  end

  name = widget_from_filename(fn)
  write_output(name, doc, $main_styles)
  $skins.each do |skin_name, skin|
    write_output("#{name}-#{skin_name}", doc, "#{$main_styles}\n\n#{skin[1]}")
  end
end

def reprocess(prefixes)
  load_directories()
  load_module_names()

  name_re = if prefixes.empty?
    /./
  else
    /^(#{prefixes.join('|')})/
  end
  $module_names.each do |name|
    if name =~ name_re
      process("#{name}-src.svg")
    end
  end
end

def listen(prefixes)
  load_directories()

  puts "Listening for changes in #{$src_dir}..."
  unless prefixes.empty?
    puts "(filering on #{prefixes.join(', ')})"
  end

  name_re = if prefixes.empty?
    /-src.svg$/
  else
    /^(#{prefixes.join('|')}).*-src.svg$/
  end

  listener = Listen.to($src_dir) do |modified, added, removed|
    modified.each do |fn|
      begin
        bn = File.basename(fn)
        if bn =~ name_re
          process(fn)
        elsif bn == 'styles.css' || bn =~ /^skin-.*\.css$/
          load_styles(true)
          reprocess(prefixes)
        elsif bn == 'defs.svg'
          load_defs(true)
          reprocess(prefixes)
        end
      rescue => e
        STDERR.puts "Error processing #{fn}:\n#{e}"
      end
    end

    added.each do |fn|
      begin
        bn = File.basename(fn)
        if bn =~ /-src\.svg$/
          load_module_names(true)
        end

        if bn =~ name_re
          process(fn)
        end
      rescue => e
        STDERR.puts "Error processing #{fn}:\n#{e}"
      end
    end

    removed.each do |fn|
      begin
        bn = File.basename(fn)
        if bn =~ /-src\.svg$/
          load_module_names(true)
        end

        if bn =~ name_re
          fs = [bn.sub(/-src.svg/, '-pp.svg')]
          $skins.keys.each do |skin_name|
            fs << bn.sub(/-src.svg/, "-#{skin_name}-pp.svg")
          end
          fs.each do |f|
            begin
              f = File.join($pp_dir, f)
              File.unlink(f)
            rescue Errno::ENOENT => e
            end
          end
        end
      rescue => e
        STDERR.puts "Error processing #{fn}:\n#{e}"
      end
    end
  end

  listener.start
  begin
    sleep
  rescue Interrupt => e
  end
end

if options[:debug]
  debug()
elsif ARGV.size >= 1
  process(ARGV[0])
elsif options[:reprocess] || options[:listen]
  reprocess(options[:module_prefixes]) if options[:reprocess]
  listen(options[:module_prefixes]) if options[:listen]
else
  STDERR.puts option_parser.help
  exit 1
end
