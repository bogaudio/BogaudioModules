#!/usr/bin/env ruby

# To generate per-modules screenshots, in Rack dir: ./Rack -d -t 1

screens = [
  {
    file: 'modules1.png',
    crop: false,
    rows: [
      ['VCO', 'XCO', 'Additator', 'FMOp', '-', 'LFO', 'EightFO', 'LLFO'],
      ['DADSRH', 'DADSRHPlus', 'DGate', 'Shaper', 'ShaperPlus', 'AD', 'ADSR', 'Follow']
    ]
  },
  {
    file: 'modules2.png',
    crop: false,
    rows: [
      ['Mix8', 'Mix4', 'Mix1', 'VCM', 'Mute8', 'Pan', 'XFade', 'VCA', 'VCAmp'],
      ['AMRM', 'Pressor', 'Lmtr', 'Nsgt', '-', 'UMix', 'Matrix88']
    ]
  },
  {
    file: 'modules3.png',
    crop: false,
    rows: [
      ['Noise', 'SampleHold', 'Walk2', 'Walk', '-', 'EightOne', 'OneEight', 'AddrSeq'],
      ['VU', 'Analyzer', 'AnalyzerXL']
    ]
  },
  {
    file: 'modules4.png',
    crop: false,
    rows: [
      ['Mono', '-', 'Detune', 'Stack', 'Reftone', '-', 'Bool', 'Cmp', 'CVD', 'FlipFlop', 'Manual', 'Mult', 'Offset', 'Slew', 'Sums', 'Switch', '-', 'Blank3', 'Blank6']
    ]
  },

  {
    file: 'oscillators.png',
    crop: true,
    rows: [
      ['VCO', 'XCO', 'Additator', 'FMOp']
    ]
  },
  {
    file: 'lfos.png',
    crop: true,
    rows: [
      ['LFO', 'EightFO', 'LLFO']
    ]
  },
  {
    file: 'envelopes1.png',
    crop: true,
    rows: [
      ['DADSRH', 'DADSRHPlus', 'DGate']
    ]
  },
  {
    file: 'envelopes2.png',
    crop: true,
    rows: [
      ['Shaper', 'ShaperPlus', 'AD', 'ADSR', 'Follow']
    ]
  },
  {
    file: 'noise.png',
    crop: true,
    rows: [
      ['Noise', 'SampleHold', 'Walk2', 'Walk']
    ]
  },
  {
    file: 'mixers1.png',
    crop: true,
    rows: [
      ['Mix8', 'Mix4', 'Mix1', 'UMix', 'Matrix88']
    ]
  },
  {
    file: 'mixers2.png',
    crop: true,
    rows: [
      ['VCM', 'Mute8', 'Pan', 'XFade', 'VCA', 'VCAmp']
    ]
  },
  {
    file: 'effects.png',
    crop: true,
    rows: [
      ['AMRM', 'Pressor', 'Lmtr', 'Nsgt']
    ]
  },
  {
    file: 'sequencers.png',
    crop: true,
    rows: [
      ['EightOne', 'OneEight', 'AddrSeq']
    ]
  },
  {
    file: 'visualizers.png',
    crop: true,
    rows: [
      ['Analyzer', 'AnalyzerXL', 'VU']
    ]
  },
  {
    file: 'pitch.png',
    crop: true,
    rows: [
      ['Mono', '-', 'Detune', 'Stack', 'Reftone']
    ]
  },
  {
    file: 'utilities.png',
    crop: true,
    rows: [
      ['Bool', 'Cmp', 'CVD', 'FlipFlop', 'Manual', 'Mult', 'Offset', 'Slew', 'Sums', 'Switch']
    ]
  },
  {
    file: 'misc.png',
    crop: true,
    rows: [
      ['Blank3', 'Blank6']
    ]
  }
]

HP = 15
PAD_EDGE = 1 * HP
PAD_MODULE = 1 * HP
PAD_GROUP = 3 * HP
OUT_DIR = './doc/www'
BACKGROUND_FILE = './doc/rack_background.png'

# require 'chunky_png'
require 'oily_png'
require 'pp'

screens_dir = ARGV[0]
if screens_dir.nil?
  STDERR.puts "Usage: #{$0}: <rack screenshot output directory for plugin>"
  exit 1
end

unless Dir.exists?(screens_dir)
  STDERR.puts "No such screenshots directory: #{screens_dir}"
  exit 1
end

$row_background = nil
def background_for_rows(n)
  unless $row_background
    background = ChunkyPNG::Image.from_file(BACKGROUND_FILE)
    $row_background = background.crop(0, 31, 15 * ((background.dimension.width - 10) / 15), 380)
  end

  out = ChunkyPNG::Image.new($row_background.dimension.width, n * $row_background.dimension.height, ChunkyPNG::Color::TRANSPARENT)
  (0...n).each do |i|
    out.compose!($row_background, 0, i * $row_background.dimension.height)
  end
  out
end

screens.each do |screen|
  next unless screen

  rows = screen[:rows]
  out = background_for_rows(rows.size)
  x = 0
  rows.each_with_index do |row, i|
    x = PAD_EDGE
    row.each_with_index do |item, j|
      if item == '-'
        x += PAD_GROUP
        x -= PAD_MODULE if j > 0
      else
        image = ChunkyPNG::Image.from_file(File.join(screens_dir, "Bogaudio-#{item}.png"))
        out.compose!(image, x, i * $row_background.dimension.height)
        x += image.dimension.width
        x += PAD_MODULE
      end
    end
    x -= PAD_MODULE if rows.last.size > 0
    x += PAD_EDGE
  end

  out.crop!(0, 0, x, out.dimension.height) if screen[:crop]

  file = File.join(OUT_DIR, screen[:file])
  out.save(file)
  puts "wrote #{file}"
  # system("open #{file}")
end
