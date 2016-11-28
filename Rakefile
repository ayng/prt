task :default do
  if !Dir.exists?("build")
    sh "mkdir build"
  end
  Dir.chdir("build") do
    sh "cmake .."
    sh "make"
  end
end

task :clean do
  sh "rm -rf build"
end

task :generate_ref_output do
  if !Dir.exists?("test")
    sh "mkdir test"
  end
  if Dir.exists?("test/ref_output")
    sh "rm -rf test/ref_output/"
  end
  sh "mkdir test/ref_output/"
  for scene in Dir.entries("scenes").select{|f| !File.directory? f} do
    output = "test/ref_output/#{scene.sub(".txt", "-ref.png")}"
    sh "./build/raytracer -o #{output} -r 40 < scenes/#{scene}"
  end
end

task :test, :verbose do |t, args|
  verbose = args.key?(:verbose) || ""
  if verbose != "-v" and verbose != ""
    sh "echo 'Must pass the option -v or nothing'"
  end
  sh "python3 test/test.py #{verbose}"
end
