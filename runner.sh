clear
echo "--------------------------------------------"
echo "--------------------------------------------"
gcc $( pkg-config --cflags gtk4 ) -o output $1 $( pkg-config --libs gtk4 ) && ./output.exe

