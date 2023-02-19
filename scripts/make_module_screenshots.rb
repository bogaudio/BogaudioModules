#!/usr/bin/env ruby

require 'fileutils'

base_dir = File.absolute_path(File.join(File.dirname($0), '..'))

skins = ['default']
Dir.glob(File.join(base_dir, 'res-src', 'skin-*.css')).each do |fn|
  if File.basename(fn) =~ /^skin-(\w+).css$/
    skins << $1
  end
end

screens_dir = File.join(base_dir, 'doc', 'module_screenshots')
if Dir.exist?(screens_dir)
  skins.each do |skin|
    skin_dir = File.join(screens_dir, skin)
    if Dir.exist?(skin_dir)
      puts "Deleting #{skin_dir}..."
      FileUtils.rm_rf(skin_dir)
    end
  end
else
  puts "Creating #{screens_dir}..."
  FileUtils.mkdir(screens_dir)
end

rack_dir = File.absolute_path(File.join(base_dir, '..', '..'))
prefs_file = File.join(rack_dir, 'Bogaudio.json')
backup_prefs_file = File.join(rack_dir, 'Bogaudio.json.bak')
if File.exist?(prefs_file)
  puts "Backing up #{prefs_file} to #{backup_prefs_file}..."
  File.write(backup_prefs_file, File.read(prefs_file))
end

rack_output_dir = File.join(rack_dir, 'screenshots', 'Bogaudio')
skins.each do |skin|
  if Dir.exist?(rack_output_dir)
    puts "Deleting #{rack_output_dir}..."
    FileUtils.rm_rf(rack_output_dir)
  end

  puts "Generating screens for skin #{skin}..."
  if skin == 'default'
    FileUtils.rm_f(prefs_file)
  else
    File.write(prefs_file, %Q|{ "skins": { "default": "#{skin}" } }|)
  end

  FileUtils.cd(rack_dir)
  out = `./Rack -d -t 1 2>&1`
  unless $?.success?
    STDERR.puts "Calling Rack failed:\n#{out}"
    exit 1
  end

  unless Dir.exist?(rack_output_dir) && File.readable?(File.join(rack_output_dir, 'Bogaudio-ADSR.png'))
    STDERR.puts "Rack produced no screenshots?  Check #{rack_output_dir}...\nRack output was:\n#{out}"
    exit 1
  end

  skin_dir = File.join(screens_dir, skin)
  unless Dir.exist?(skin_dir)
    puts "Creating #{skin_dir}..."
    FileUtils.mkdir(skin_dir)
  end

  puts "Copying screens to #{skin_dir}..."
  count = 0
  Dir.glob(File.join(rack_output_dir, '*.png')).each do |fn|
    count += 1
    out_fn = File.join(skin_dir, File.basename(fn).sub(/^Bogaudio-/, ''))
    FileUtils.cp(fn, out_fn)
  end
  puts "...copied #{count}"
end

if File.exist?(backup_prefs_file)
  puts "Restoring #{prefs_file}, deleting backup..."
  File.write(prefs_file, File.read(backup_prefs_file))
  FileUtils.rm_f(backup_prefs_file)
end
