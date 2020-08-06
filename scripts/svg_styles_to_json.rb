#!/usr/bin/env ruby

require 'css_parser'
require 'json'

def colors_for_styles(styles)
  out = {}
  parser = CssParser::Parser.new
  parser.load_string!(styles)

  out['background-fill'] = '#ddd'
  if parser.find_by_selector('.background-fill').last =~ /fill:\s+(#\w+);/
    out['background-fill'] = $1
  end

  out['path-stroke'] = '#333'
  if parser.find_by_selector('path').last =~ /stroke:\s+(#\w+);/
    out['path-stroke'] = $1
  end

  out['knob-center'] = '#333'
  if parser.find_by_selector('circle.knob-center').last =~ /fill:\s+(#\w+);/
    out['knob-center'] = $1
  end

  out['knob-rim'] = '#eee'
  if parser.find_by_selector('circle.knob-rim').last =~ /fill:\s+(#\w+);/
    out['knob-rim'] = $1
  end

  out['knob-tick'] = '#fff'
  if parser.find_by_selector('circle.knob-tick').last =~ /fill:\s+(#\w+);/
    out['knob-tick'] = $1
  end

  out
end

js = {}
rsdir = File.absolute_path(File.join(File.dirname($0), '..', 'res-src'))
if File.readable?(File.join(rsdir, 'styles.css'))
  base_styles = File.read(File.join(rsdir, 'styles.css'))
  js['light'] = colors_for_styles(base_styles)

  Dir.glob(File.join(rsdir, 'skin-*.css')) do |fn|
    if File.basename(fn) =~ /^skin-(\w+).css$/
      skin = $1
      skin_styles = File.read(fn)
      js[skin] = colors_for_styles(base_styles + "\n" + skin_styles)
    end
  end
end

js = JSON.generate(js, indent: '  ', space: ' ', object_nl: "\n")
# puts js
outfn = File.absolute_path(File.join(File.dirname($0), '..', 'res', 'skin_css_values.json'))
File.write(outfn, js + "\n")
puts "Wrote #{outfn}..."
