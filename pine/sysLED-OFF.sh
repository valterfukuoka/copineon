echo "359" > /sys/class/gpio/export 
echo "out" > /sys/class/gpio/gpio359/direction
echo "1"   > /sys/class/gpio/gpio359/value
