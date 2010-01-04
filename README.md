Unicorn Hunter
==============

Compiling
---------

Just run
    rake build
in the unicorn_hunter directory.

Usage
-----

    unicorn_hunter <application> {start,restart,stop,deploy,worker,status}

### Start

Starts the application and starts monitoring the app. This also works when the app is running.

If there is another unicorn_hunter instance already monitoring the app it will be replaced with the new one.

### Restart

Tells the currently running unicorn to restart all workers. If you use preload_app you need to use "deploy"
for restarting the app.

### Stop

Stops the application and the monitoring unicorn_hunter.

### Deploy

Performs a no-downtime restart by replacing the master process with a new one.

### Worker

Add or remove workers.

### Status

Shows the status of the application.

Configuration
-------------

To get started copy unicorn.conf.example to /etc/unicorn/unicorn_APPNAME.conf.

You also need to put unicorn.rb.example into your applications config directory.

All configuration options from the .conf file will be passed along to the unicorn master process as environment variables.

Configuration Options used by Unicorn Hunter
--------------------------------------------

*APP_ENV* (defaults to "production") - environment of the application

*APP_ROOT* (*required*) - path to your application's root directory

*APP_RUBY* (defaults to "ruby") - override default ruby version

*APP_TYPE* (defaults to "rails") - can be "rails" or "rack"

*BUNDLER* (defaults to "false") - set to "true" if you're using gem bundler (and need to run bin/unicorn)

*UNICORN_LISTEN* (defaults to 3000) - port or socket to listen on (example: "/var/run/unicorn/unicorn_test.sock" or "80")

*UNICORN_PID_DIR* (defaults to /var/run/unicorn) - path to store unicorn.<app name>.pid and unicorn.<app name>.hunter.pid

*UNICORN_WORKERS* (defaults to 3) - number of unicorn workers to start
