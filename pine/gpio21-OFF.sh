echo "75" > /sys/class/gpio/export 
echo "out" > /sys/class/gpio/gpio75/direction
echo "0"   > /sys/class/gpio/gpio75/value
