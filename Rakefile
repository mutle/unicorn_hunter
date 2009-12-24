task :build do
  puts "Compiling..."
  `rm -f unicorn_hunter; gcc src/utils.c src/configuration.c src/unicorn_control.c src/unicorn_hunter.c -g -o unicorn_hunter`
  puts "done"
end

task :run do
  exec "./unicorn_hunter"
end

task :debug do
  exec "gdb ./unicorn_hunter"
end

task :default => :build
