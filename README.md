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

To get started copy unicorn.conf.example to
/etc/unicorn/unicorn_APPNAME.conf.

You also need to put unicorn.rb.example into your applications config
directory.
