Having GenIcon to work:
=======================

You will need the Bunny library - and its dependencies:

https://github.com/Damdoshi/LibLapin

You will also need a fully working display. If you do not have a display, read the following
section.

Having GenIcon to work on headless server:
==========================================

Install an headless X server:

```console
sudo apt-get install xvfb
```

Then copy the following in /etc/init.d/xvfb

```
#!/bin/sh
### BEGIN INIT INFO
# Provides:          xvfb
# Required-Start:    $all
# Required-Stop:
# Default-Start:     2 3 4 5
# Default-Stop:	     0 1 6
# Short-Description: to use software that required a display without display
### END INIT INFO

XVFB=/usr/bin/Xvfb
XVFBARGS=":1 -screen 0 1024x768x24 -ac +extension GLX +render -noreset -nolisten tcp"
PIDFILE=/var/run/xvfb.pid
case "$1" in
  start)
    echo -n "Starting virtual X frame buffer: Xvfb"
    start-stop-daemon --start --quiet --pidfile $PIDFILE --make-pidfile --background --exec $XVFB -- $XVFBARGS
    echo "."
    ;;
  stop)
    echo -n "Stopping virtual X frame buffer: Xvfb"
    start-stop-daemon --stop --quiet --pidfile $PIDFILE
    echo "."
    ;;
  restart)
    $0 stop
    $0 start
    ;;
  *)
        echo "Usage: /etc/init.d/xvfb {start|stop|restart}"
        exit 1
esac

exit 0
```

Then execute the following command:

```console
sudo systemctl enable xvfb
echo DISPLAY=":1" | sudo tee -a /etc/environment
```

If you need to remove it from autorun:

```console
sudo update-rc.d -f xvfb remove
```

