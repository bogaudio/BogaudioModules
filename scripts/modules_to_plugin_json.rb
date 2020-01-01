#!/usr/bin/ruby

require 'JSON'

plugin_json = File.read("plugin.json")
plugin = JSON.parse(plugin_json)

flags = []
flags << '-DTEST=1' if ENV['TEST']
flags << '-DEXPERIMENTAL=1' if ENV['EXPERIMENTAL']
models = `c++ #{flags.join(' ')} -E src/bogaudio.cpp 2>&1 | grep addModel`
models = models.split.map do |s|
  s.sub(/^\s*p->addModel\(([^)]+)\);\s*$/, '\1')
end.reject { |m| m =~ /^model.*HP$/ }

modules = []
models.each do |model|
  create = `grep -in #{model} src/*cpp | grep createModel`
  if create =~ /createModel.*?\("([^"]+)",\s*"([^"]+)",\s*"([^"]*)"(?:,\s*([^)]*))?\);/
    slug = $1
    name = $2
    description = $3
    tags = $4
    if tags
      tags = tags.split(/,\s+/).map { |t| t.sub(/^\s*"([^"]+)"\s*$/, '\1') }
    else
      tags = []
    end

    modules << {
      slug: slug,
      name: name,
      description: description,
      tags: tags
    }
  else
    STDERR.puts "Failed to get create line for #{model}"
    exit 1
  end
end

plugin['modules'] = modules
File.write('plugin.json.bak', plugin_json)
puts 'wrote plugin.json.bak'
File.write('plugin.json', JSON.pretty_generate(plugin) + "\n")
puts "wrote plugin.json"
