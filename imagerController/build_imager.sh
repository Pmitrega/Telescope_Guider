cmake -S . -B build &&
cmake --build build --target ImagerController &&
cd build/app &&
sudo ./ImagerController &&
cd .. &&
cd ..