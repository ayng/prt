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
    sh "mkdir test/ref_output/"
  end
  for scene in Dir.entries("scenes").select{|f| !File.directory? f} do
    output = scene.sub(".txt", "-ref.png")
    sh "./build/raytracer -r 200 -o #{output} < scenes/#{scene}"
  end
end

task :test do
  sh "python3 test/test.py"
end
