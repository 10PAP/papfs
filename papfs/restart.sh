cd test
fusermount -u mount
cd ..
rm -rf test
make clean
make
make test
cd test
./papfs root mount
cat mount/data.bin
cat papfs.log

