# load all ruby files
Dir["#{File.dirname(__FILE__)}/lib/**/*.rb"].each do |f|
  require(f)
end

def main
  Dir['*.pcgl'].each do |input_file|
    visitor = PCGL::Loader.new(input_file).process
    base = File.basename(input_file).sub('.pcgl', '')
    File.open("#{base}.h", 'w') do |output_file|
      output_file.puts visitor.h_file
    end
    File.open("#{base}.cpp", 'w') do |output_file|
      output_file.puts visitor.cpp_file
    end
  end
end

main
